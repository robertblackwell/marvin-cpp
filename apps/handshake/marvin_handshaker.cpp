#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include "marvin_handshaker.hpp"


#include <openssl/ossl_typ.h>           // for X509_STORE
#include <cert/cert_authority.hpp>      // for AuthoritySPtr
#include <cert/cert_certificate.hpp>    // for Certificate
#include <cert/cert_identity.hpp>       // for Identity
#include <cert/cert_store_locator.hpp>  // for LocatorSPtr
#include <cert/cert_store_store.hpp>    // for StoreSPtr


#include <marvin/connection/socket_factory.hpp>
#include <marvin/certificates/certificates.hpp>
#include <marvin/helpers/mitm.hpp>
#include <marvin/http/message_factory.hpp>


namespace Marvin {

Handshaker::Handshaker(boost::asio::io_service& io, std::string host) : m_io(io), m_scheme("https"), m_host(host), m_port("443")
{
    m_socket_sptr = socket_factory(m_io, m_scheme, m_host, m_port);
}
Handshaker::~Handshaker()
{

}

void Handshaker::handshake(std::function<void(ErrorType err)> cb)
{
    m_callback = cb;
    m_socket_sptr->async_connect([this](ErrorType &err, ISocket *conn)
                                 {
                                     if (err) {
                                         auto d = make_error_description(err);
                                         ErrorType marvin_error = err;
                                         p_on_connect_error(marvin_error);
                                     } else {
                                         p_handshake_upstream();
                                     }
                                 });
}
Cert::Certificate Handshaker::getServerCertificate()
{
    return m_server_certificate;
}
void Handshaker::p_handshake_upstream()
{
    Certificates& certificates = Certificates::getInstance();
    X509_STORE* x509_store_ptr = certificates.get_X509_STORE_ptr();
    m_socket_sptr->become_secure_client(x509_store_ptr);
    m_socket_sptr->async_handshake([this, &certificates](const boost::system::error_code &err)
                                   {
                                       if (err) {
                                           auto d = make_error_description(err);
                                           ErrorType marvin_error = err;
                                           TROG_ERROR("upstream handshake err: ", d);
                                           p_on_handshake_error(marvin_error);
                                       } else {
                                           m_server_certificate = m_socket_sptr->get_server_certificate();
#ifdef MARVIN_HTTPS_TRACE
                                           /// examine the original certificate
                                           std::string ss1 = m_server_certificate.getIssuerNameAsOneLine();
                                           std::string ss2 = m_server_certificate.getSubjectNameAsOneLine();
                                           std::string ss3 = m_server_certificate.getSubjectAlternativeNamesAsString();
                                           TROG_TRACE3("scheme: ", m_upstream_scheme);
                                           TROG_TRACE3("host: ", m_upstream_host);
                                           TROG_TRACE3("port: ", m_upstream_port);
                                           TROG_TRACE3("original certificate subjectname: ", ss2);
                                           TROG_TRACE3("original certificate issuer     : ", ss1);
                                           TROG_TRACE3("original certificate altnames   : ", ss3);
#endif

                                           m_mitm_identity = certificates.build_server_mitm_certificate(m_host,
                                                                                                        m_server_certificate);

#ifdef MARVIN_HTTPS_TRACE
                                           Cert::Certificate cc{m_mitm_identity.getX509()};
                                           std::string ssm1 = cc.getIssuerNameAsOneLine();
                                           std::string ssm2 = cc.getSubjectNameAsOneLine();
                                           std::string ssm3 = cc.getSubjectAlternativeNamesAsString();
                                           TROG_TRACE3("mitm certificate subjectname: ", ssm2);
                                           TROG_TRACE3("mitm certificate issuer     : ", ssm1);
                                           TROG_TRACE3("mitm certificate altnames   : ", ssm3);
#endif
                                           p_on_handshake_ok();
                                       }
                                   });
}

void Handshaker::p_on_handshake_error(ErrorType& err)
{
    m_callback(err);
}
void Handshaker::p_on_handshake_ok()
{
    m_callback(Marvin::make_error_ok());
}
void Handshaker::p_on_connect_error(ErrorType& err)
{
    m_callback(err);
}

} // namespace