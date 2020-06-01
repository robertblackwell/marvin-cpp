#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/server/mitm_http.hpp>

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

MitmHttp::MitmHttp(
        MitmApp& mitm_app,
        ISocketSPtr socket_sptr,
        MessageReaderV2::SPtr rdr,
        MessageWriter::SPtr   wrtr,
        std::string scheme,
        std::string host,
        std::string port,
        ICollectorSPtr collector_sptr

) : m_mitm_app(mitm_app), m_io(socket_sptr->get_io_context())
{
    m_downstream_rdr_sptr = rdr;
    m_downstream_socket_sptr = socket_sptr;
    m_downstream_request_sptr = m_downstream_rdr_sptr->get_message_sptr();
    m_downstream_wrtr_sptr = wrtr;
    m_upstream_scheme = scheme;
    m_upstream_host = host;
    m_upstream_port = port;
    m_collector_sptr = collector_sptr;
    m_upstream_socket_sptr = socket_factory(mitm_app.m_io, m_upstream_scheme, m_upstream_host, m_upstream_port);
}
MitmHttp::~MitmHttp()
{

}

void MitmHttp::handle()
{
    p_initiate_upstream_roundtrip();
}

void MitmHttp::p_initiate_upstream_roundtrip()
{
   
    m_upstream_client_uptr = std::unique_ptr<Client>(new Client(m_io, m_upstream_scheme, m_upstream_host, m_upstream_port));
    p_roundtrip_upstream(m_downstream_request_sptr, [this](MessageBaseSPtr downMsg){
        /// get here with a message suitable for transmission to down stream client
        m_downstream_response_sptr = downMsg;
       TROG_TRACE3("for downstream", trace_message(*downMsg));
        Marvin::BufferChain::SPtr responseBodySPtr = downMsg->get_body_buffer_chain();
        /// perform the MITM collection
        
        m_collector_sptr->collect(m_scheme, m_host, m_downstream_request_sptr, m_downstream_response_sptr);
        
        /// write response to downstream client
        m_downstream_wrtr_sptr->async_write(m_downstream_response_sptr, responseBodySPtr, [this](Marvin::ErrorType& err) {
            if (err) {
                m_mitm_app.p_on_downstream_write_error(err);
            } else {
                p_on_request_completed();
            }
        });

    });
}
/// \brief Perform the proxy forwarding process; and produces a response suitable
/// for downstream transmission; the result of this method is a response to send back to the client
/// \param req : MessageReaderSPtr the request from the original client - has same value
///                                 as class property m_request_sptr
/// \param upstreamCb : called when the round trip has finished
///
void MitmHttp::p_roundtrip_upstream(
        MessageBase::SPtr req,
        std::function<void(MessageBaseSPtr downstreamReplyMsg)> upstreamCb
){
    /// a client object to manage the round trip of request and response to
    /// the final destination. m_host m_scheme and m_port already setup

    // m_upstream_client_uptr = std::unique_ptr<Client>(new Client(m_io, m_scheme, m_host, m_port));
    /// the MessageBase that will be the up stream request
    m_upstream_request_sptr = std::make_shared<MessageBase>();
    /// format upstream msg for transmission
    Helpers::make_upstream_request(m_upstream_request_sptr, req);
    Marvin::BufferChain::SPtr content = req->get_body_buffer_chain();
    
    m_upstream_client_uptr->async_write(m_upstream_request_sptr, content, [this, upstreamCb](Marvin::ErrorType& ec, MessageBase::SPtr upstrmRdr)
    {
        if (ec || (upstrmRdr == nullptr)) {
            std::string desc = make_error_description(ec);
            TROG_WARN("async write failed ", make_error_description(ec));
            m_mitm_app.p_on_upstream_roundtrip_error(ec);
            // TODO: how to handle error
        } else {
           TROG_TRACE3("upstream response", trace_message(*(upstrmRdr.get())));
            m_downstream_response_sptr = std::make_shared<MessageBase>();
            m_upstream_response_body_sptr = upstrmRdr->get_body_buffer_chain();
            Helpers::make_downstream_response(m_downstream_response_sptr, upstrmRdr, ec);
            upstreamCb(m_downstream_response_sptr);
        }
    });
    
};
void MitmHttp::p_downstream_read_message()
{
    m_downstream_rdr_sptr->async_read_message([this](Marvin::ErrorType err)
    {
      if (err) {
          m_mitm_app.p_on_downstream_read_error(err);
      } else {
          p_initiate_upstream_roundtrip();
      }
    });
}

void MitmHttp::p_on_request_completed()
{
    if ((is_connection_keep_alive(*m_downstream_request_sptr) && is_connection_keep_alive(*m_downstream_response_sptr))) {
        p_downstream_read_message();
    } else {
        m_mitm_app.p_connection_end();
    }

}

}