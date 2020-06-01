#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/server/mitm_https.hpp>

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

MitmHttps::MitmHttps(
        MitmApp& mitm_app,
        ISocketSPtr socket_sptr,
        MessageReaderV2::SPtr rdr,
        MessageWriterSPtr wrtr,
        std::string scheme,
        std::string host,
        std::string port,
        ICollectorSPtr collector_sptr

) : m_mitm_app(mitm_app), m_io(socket_sptr->get_io_context())
{
    TROG_TRACE_CTOR();
    m_downstream_rdr_sptr = rdr;
    m_downstream_socket_sptr = socket_sptr;
    m_downstream_request_sptr = rdr->get_message_sptr();
    m_downstream_wrtr_sptr = wrtr;
    m_upstream_scheme = scheme;
    m_upstream_host = host;
    m_upstream_port = port;
    m_collector_sptr = collector_sptr;
    m_upstream_socket_sptr = socket_factory(mitm_app.m_io, m_upstream_scheme, m_upstream_host, m_upstream_port);
}
MitmHttps::~MitmHttps()
{
    TROG_TRACE_CTOR();
}

void MitmHttps::handle()
{
    m_upstream_socket_sptr->async_connect([this](ErrorType &err, ISocket *conn)
    {
    if (err) {
          auto d = make_error_description(err);
          ErrorType marvin_error = err;
          p_on_upstream_connect_handshake_error(marvin_error);
      } else {
          p_handshake_upstream();
      }
    });
}
void MitmHttps::p_handshake_upstream()
{
    Certificates& certificates = Certificates::getInstance();
    X509_STORE* x509_store_ptr = certificates.get_X509_STORE_ptr();
    m_upstream_socket_sptr->become_secure_client(x509_store_ptr);
    m_upstream_socket_sptr->async_handshake([this, &certificates](const boost::system::error_code &err)
    {
        if (err) {
            auto d = make_error_description(err);
            ErrorType marvin_error = err;
            TROG_ERROR("upstream handshake err: ", d);
            m_mitm_app.p_on_upstream_error(marvin_error);
        } else {
            m_server_certificate = m_upstream_socket_sptr->get_server_certificate();

            m_mitm_identity = certificates.build_server_mitm_certificate(
                m_upstream_host, m_server_certificate);


            m_downstream_response_sptr = std::make_shared<MessageBase>();
            make_response_200_OK_connected(*m_downstream_response_sptr);

            m_downstream_wrtr_sptr->async_write(m_downstream_response_sptr,[this](Marvin::ErrorType &err)
            {
               if (err) {
                   TROG_WARN("error: ", err.value (), err.category ().name (), err.category ().message (err.value ()));
                   m_mitm_app.p_on_downstream_write_error(
                       err);
               } else {
                   m_downstream_socket_sptr->become_secure_server(
                       m_mitm_identity);
                   m_downstream_socket_sptr->async_handshake([this] (const boost::system::error_code &err)
                   {
                       if (err) {
                           ErrorType marvin_error = err;
                           m_mitm_app.p_on_upstream_error (
                               marvin_error);
                       } else {
                           p_downstream_read_message ();
                       }
                   });
               }
            });
        }
    });
}

void MitmHttps::p_downstream_read_message()
{
    m_downstream_rdr_sptr = std::make_shared<MessageReaderV2>(m_downstream_socket_sptr);
    m_downstream_rdr_sptr->async_read_message([this](Marvin::ErrorType err)
    {
        if (err) {
            m_mitm_app.p_on_downstream_read_error(err);
        } else {
        #if MARVIN_HANDLE_HTTPS_WEBSOCKET_UPGRADE
          if (isWebSocketUpgrade(m_downstream_rdr_sptr)) {
              initiate tunnel
          } else {
              /* code */
          }
        #endif
            MessageBaseSPtr x = m_downstream_rdr_sptr->get_message_sptr();
            m_downstream_request_sptr = x;
            p_initiate_upstream_roundtrip();
        }
    });
}
void MitmHttps::p_initiate_upstream_roundtrip()
{
   
    m_upstream_socket_sptr = socket_factory(m_io, m_upstream_scheme, m_upstream_host, m_upstream_port);

    Certificates certificates = Certificates::getInstance();
    m_upstream_socket_sptr->become_secure_client(certificates.get_X509_STORE_ptr());
    TROG_TRACE3("initiate upstream")
    m_upstream_socket_sptr->async_connect([this](ErrorType &err, ISocket *conn)
    {
        TROG_TRACE3("connected upstream")
        if (err) {
            auto x = make_error_description(err);
            ErrorType marvin_error = err;
            m_mitm_app.p_on_upstream_roundtrip_error(marvin_error);
        } else {
            m_upstream_client_uptr = std::unique_ptr<Client>(new Client(m_io, m_upstream_socket_sptr));

            p_roundtrip_upstream(m_downstream_request_sptr,[this](MessageBaseSPtr downMsg)
            {
                /// get here with a message suitable for transmission to down stream client
                m_downstream_response_sptr = downMsg;
                if (downMsg->status_code() != 200) {
                   TROG_WARN("STATUS_CODE:",
                             downMsg->status_code())
                }
                TROG_TRACE3("for downstream",
                           trace_message(*downMsg));
                Marvin::BufferChain::SPtr responseBodySPtr = downMsg->get_body_buffer_chain();
                /// perform the MITM collection

                m_collector_sptr->collect(m_upstream_scheme, m_upstream_host, m_downstream_request_sptr, m_downstream_response_sptr);
                /// write response to downstream client
                m_downstream_wrtr_sptr->async_write (m_downstream_response_sptr, responseBodySPtr, [this, responseBodySPtr] (Marvin::ErrorType &err)
                {
                    TROG_TRACE3("downstream write complete");
                    if (err) {
                        m_mitm_app.p_on_downstream_write_error(err);
                    } else {
                        p_on_request_completed();
                   }
               });

            });
        }
    });

}
/// \brief Perform the proxy forwarding process; and produces a response suitable
/// for downstream transmission; the result of this method is a response to send back to the client
/// \param req : MessageReaderSPtr the request from the original client - has same value
///                                 as class property m_request_sptr
/// \param upstreamCb : called when the round trip has finished
///
void MitmHttps::p_roundtrip_upstream(
        MessageBase::SPtr req,
        std::function<void(MessageBaseSPtr downstreamReplyMsg)> upstreamCb
){
    /// a client object to manage the round trip of request and response to
    /// the final destination. m_host m_scheme and m_port already setup

    // m_upstream_client_uptr = std::unique_ptr<Client>(new Client(m_io, m_scheme, m_host, m_port));
    /// the MessageBase that will be the up stream request
    m_upstream_request_sptr = std::make_shared<MessageBase>();
    /// format upstream msg for transmission
    Helpers::make_upstream_https_request(m_upstream_request_sptr, req);
    Marvin::BufferChain::SPtr content = req->get_body_buffer_chain();
    TROG_TRACE3("start roundtrip upstream")
    m_upstream_client_uptr->async_write(m_upstream_request_sptr, content, [this, upstreamCb](Marvin::ErrorType& ec, MessageBase::SPtr upstrm_resp)
    {
        if (ec || (upstrm_resp == nullptr)) {
            std::string desc = make_error_description(ec);
            TROG_WARN("async write failed ", make_error_description(ec));
            m_mitm_app.p_on_upstream_roundtrip_error(ec);
            // TODO: how to handle error
        } else {
           TROG_TRACE3("upstream response", trace_message(*(upstrm_resp.get())));
            m_downstream_response_sptr = std::make_shared<MessageBase>();
            m_upstream_response_body_sptr = upstrm_resp->get_body_buffer_chain();
            Helpers::make_downstream_response(m_downstream_response_sptr, upstrm_resp, ec);
            upstreamCb(m_downstream_response_sptr);
        }
    });
    
};
void MitmHttps::p_on_request_completed()
{
    TROG_TRACE3("complete");
    if ((is_connection_keep_alive(*m_downstream_request_sptr) && is_connection_keep_alive(*m_downstream_response_sptr))) {
        TROG_TRACE3("reading another");
        p_downstream_read_message();
    } else {
        TROG_TRACE3("end");
        m_mitm_app.p_connection_end();
    }

}
void MitmHttps::p_on_upstream_connect_handshake_error(ErrorType& err)
{
    m_downstream_response_sptr = std::make_shared<MessageBase>();
    make_response_502_badgateway(*m_downstream_response_sptr);

    m_downstream_wrtr_sptr->async_write(m_downstream_response_sptr, [this](Marvin::ErrorType& err){
        TROG_INFO("");
        if( err ) {
            TROG_WARN("error: ", err.value(), err.category().name(), err.category().message(err.value()));
            m_mitm_app.p_on_downstream_write_error(err);
        } else {
            m_mitm_app.p_connection_end();
        }
    });

}

}