#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/server/mitm_tunnel.hpp>

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

MitmTunnel::MitmTunnel(
        MitmApp& mitm_app,
        ISocketSPtr socket_sptr,
        MessageReaderV2::SPtr rdr,
        MessageWriter::SPtr wrtr,
        std::string scheme,
        std::string host,
        std::string port

) : m_mitm_app(mitm_app), m_io(socket_sptr->get_io_context()), m_strategy(TunnelAllocator()), m_factory(m_strategy)
{
    TROG_TRACE_CTOR();
    m_downstream_rdr_sptr = rdr;
    m_downstream_socket_sptr = socket_sptr;
    m_downstream_wrtr_sptr = wrtr;
    m_upstream_scheme = scheme;
    m_upstream_host = host;
    m_upstream_port = port;
}
MitmTunnel::~MitmTunnel()
{
    TROG_TRACE_CTOR();
}

void MitmTunnel::handle()
{
    p_initiate_tunnel();
}

void MitmTunnel::p_initiate_tunnel()
{
   TROG_TRACE3("scheme:", m_upstream_scheme, " host:", m_upstream_host, " port:", m_upstream_port);

    m_upstream_connection_sptr = socket_factory (m_io, m_upstream_scheme, m_upstream_host, m_upstream_port);
    m_upstream_connection_sptr->async_connect ([this] (Marvin::ErrorType &err, ISocket *conn)
    {
        if (err) {
            TROG_WARN("initiateTunnel: FAILED scheme:",
            this->m_upstream_scheme, " host:",
            this->m_upstream_host, " port:", this->m_upstream_port);
            m_downstream_response_sptr = std::make_shared<MessageBase> ();
            make_response_502_badgateway (*m_downstream_response_sptr);

            m_downstream_wrtr_sptr->async_write (m_downstream_response_sptr, [this] (Marvin::ErrorType &err)
            {
                TROG_INFO("");
                if (err) {
                    TROG_WARN("error: ", err.value (), err.category ().name (), err.category ().message (err.value ()));
                    m_mitm_app.p_on_downstream_write_error (err);
                } else {
                    m_mitm_app.p_on_tunnel_completed ();
                }
            });
        } else {
            TROG_TRACE3("initiateTunnel: connection SUCCEEDED scheme:",
            " scheme:", this->m_upstream_scheme, " host:",
            this->m_upstream_host, " port:",
            this->m_upstream_port);
            m_downstream_response_sptr = std::make_shared<MessageBase> ();
            make_response_200_OK_connected (*m_downstream_response_sptr);
            m_downstream_wrtr_sptr->async_write (m_downstream_response_sptr, [this] (Marvin::ErrorType &err)
            {
                TROG_INFO("");
                if (err) {
                    TROG_WARN("error: ", err.value (), err.category ().name (), err.category ().message (err.value ()));
                    m_mitm_app.p_on_downstream_write_error (err);
                } else {
                    m_tunnel_handler_sptr = std::make_shared<TunnelHandler> (
                    m_io,
                    m_downstream_socket_sptr,
                    m_upstream_connection_sptr);
                    m_tunnel_handler_sptr->start (
                    [this] (
                    Marvin::ErrorType &err)
                    {
                        if (err) {
                            m_mitm_app.p_on_tunnel_error (err);
                        } else {
                            m_mitm_app.p_on_tunnel_completed ();
                        }
                    });
                }
            });
        }
    });
}

void MitmTunnel::p_on_request_completed()
{
    m_mitm_app.p_connection_end();
}


}