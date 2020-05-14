#include <marvin/http/message_factory.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/forwarding/ssl_forwarding_handler.hpp>
#include <marvin/forwarding/forward_helpers.hpp>
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

namespace Marvin {

std::string traceSSLForwardingHandler(SSLForwardingHandler* fh_ptr)
{
    std::stringstream ss;
    ss << "FWRDH[" << std::hex << (long)(void*)fh_ptr << std::dec << "]: ";
    return ss.str();
}
std::string traceSSLForwardingHandler(SSLForwardingHandlerUPtr fh_ptr)
{
    return traceSSLForwardingHandler(fh_ptr.get());
//    std::stringstream ss;
//    ss << "FWRDH[" << std::hex << (long)(void*)fh_ptr.get() << std::dec << "]: ";
//    return ss.str();
}
std::string traceSSLFowardingHandler(SSLForwardingHandlerSPtr fh_ptr)
{
    return traceSSLForwardingHandler(fh_ptr.get());
//    std::stringstream ss;
//    ss << "FWRDH[" << std::hex << (long)(void*)fh_ptr.get() << std::dec << "]: ";
//    return ss.str();
}


#pragma mark - Forward handler class

SSLForwardingHandler::SSLForwardingHandler(
    boost::asio::io_service& io,
    ICollector* collector_ptr
): m_io(io), m_collector_ptr(collector_ptr)
{

}


SSLForwardingHandler::~SSLForwardingHandler()
{
   TROG_TRACE_CTOR();
}

#pragma mark - handle upgrade request

void SSLForwardingHandler::p_on_complete(Marvin::ErrorType& err)
{
    if( err ){
//       TROG_WARN("error: ", err.value(), err.category().name(), err.category().message(err.value()));
        // got an error sending response to downstream client - what can we do ? Nothing
        auto pf = std::bind(m_done_callback, err, false);
        m_io.post(pf);
    }else{
        auto pf = std::bind(m_done_callback, err, true);
        m_io.post(pf);
    }
}
#pragma mark - SSL functions
/**
* What does this need to return if the round-trip does not successfully complete
*/
void SSLForwardingHandler::handleSSLRequest(
        ServerContext&          server_context,
        MessageReaderSPtr       request,
        MessageWriterSPtr       responseWriter,
        ConnectionSPtr          clientConnectionPtr,
        HandlerDoneCallbackType done
){
   TROG_TRACE3("from downstream", Marvin::traceMessage(*(request.get())));
    m_request_reader_sptr = request;
    m_response_writer_sptr = responseWriter;
    m_done_callback = done;
    m_downstream_connection = clientConnectionPtr;
    
    Marvin::Uri tmp_uri(request->target());
    m_host = tmp_uri.server();
    m_port = (int)tmp_uri.port();
    m_scheme = tmp_uri.scheme();
    ConnectionSPtr upstream_conn_sptr;
    ConnectionSPtr downstream_conn_sptr; // need to get the downstream connection
    
    auto next_step = std::bind(&SSLForwardingHandler::p_on_handshake_complete, this, std::placeholders::_1);
    p_ssl_handshake_upstream(next_step);

}
/***************************************************************************************************************/
void SSLForwardingHandler::p_ssl_handshake_upstream(std::function<void(const boost::system::error_code& err)> cb)
{
    ConnectionSPtr conn_sptr = std::make_shared<Connection>(m_io, m_scheme, m_host, std::to_string(m_port));
    X509_STORE* certificate_store;
    conn_sptr->becomeSecureClient(certificate_store);
    auto next_step = std::bind(&SSLForwardingHandler::p_on_handshake_complete, this, std::placeholders::_1);
    conn_sptr->asyncHandshake([this](const boost::system::error_code& err) {
    });
}
void SSLForwardingHandler::p_on_handshake_complete(const boost::system::error_code& err)
{
    if (err) {
        // this is wrong - should send rejection downstream
        // wait for it to send
        // close downstream or wait for eof
        // need to check what the forwarding handler does to see if this needs to retunr faailure
        // and then return failure
        auto pf = std::bind(m_done_callback, err, false); m_io.post(pf);
        return;
    }
    // get the server certificate and build the mitm certificate
    // save the mitm certificate in a class property
    auto next_step = std::bind(&SSLForwardingHandler::p_on_send_OK_complete, this, std::placeholders::_1);
    p_ssl_send_OK_downstream(next_step);
}
/***************************************************************************************************************/

void SSLForwardingHandler::p_ssl_send_OK_downstream(std::function<void(Marvin::ErrorType& err)> cb)
{
    // prepare the ok message and send it
}
void SSLForwardingHandler::p_on_send_OK_complete(Marvin::ErrorType& err)
{
    if (err) {
        // close upstream, return failure
        auto pf = std::bind(m_done_callback, err, false); m_io.post(pf);
        return;
    }
    auto next_step = std::bind(&SSLForwardingHandler::p_on_become_secure_downstream_complete , this, std::placeholders::_1);
    p_ssl_become_secure_downstream(next_step);
    
}
/***************************************************************************************************************/

void SSLForwardingHandler::p_ssl_become_secure_downstream(std::function<void(Marvin::ErrorType& err)> cb)
{
    // set up an ssl_ctx for down stream, and set the mitm certificate in the dowstream ctx
    // handshake with the downstream
}
void SSLForwardingHandler::p_on_become_secure_downstream_complete(Marvin::ErrorType& err)
{
    if (err) {
        auto pf = std::bind(m_done_callback, err, false); m_io.post(pf);
    }
    auto next_step = std::bind(&SSLForwardingHandler::p_on_read_request_downstream_complete, this, std::placeholders::_1);
    p_ssl_read_request_from_downstream(next_step);
}
/***************************************************************************************************************/

void SSLForwardingHandler::p_ssl_read_request_from_downstream(std::function<void(Marvin::ErrorType& err)> cb)
{
    // wrap the downstream connection in a messagereader, save the messagereadersptr in a class property
    // and read a message from downstream
    
}
void SSLForwardingHandler::p_on_read_request_downstream_complete(Marvin::ErrorType& err)
{
    if (err) {
        auto pf = std::bind(m_done_callback, err, false); m_io.post(pf);
    }
    auto next_step = std::bind(&SSLForwardingHandler::p_on_read_request_downstream_complete, this, std::placeholders::_1);
    p_ssl_roundtrip_upstream(next_step);
}

/***************************************************************************************************************/

void SSLForwardingHandler::p_ssl_roundtrip_upstream(std::function<void(Marvin::ErrorType& err)> cb)
{
    // get the downstream request from the downstream message reader
    // build an upstream request frm that message
    // wrap the upstream connection in a client() and save a ref to the client in a class property
    // write the request and read the response
}
void SSLForwardingHandler::p_on_rountrip_complete(Marvin::ErrorType& err)
{
    if (err) {
        auto pf = std::bind(m_done_callback, err, false); m_io.post(pf);
    }
    // get the response from the client
    // build a final downstream response
    // write that response
    // decide whether to close of go again -- have not figures this out yet
}
/***************************************************************************************************************/
} // namespace