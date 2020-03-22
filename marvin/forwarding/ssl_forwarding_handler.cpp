#include <marvin/http/message_factory.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/forwarding/ssl_forwarding_handler.hpp>
#include <marvin/forwarding/forward_helpers.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

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
    LogTorTrace();
}

#pragma mark - handle upgrade request

#if 0
void ForwardingHandler::handleRequest(
        ServerContext&          server_context,
        MessageReaderSPtr       request,
        MessageWriterSPtr       responseWriter,
        ISocketSPtr             clientConnectionPtr,
        HandlerDoneCallbackType done
){
    LogTrace("from downstream", Marvin::Http::traceMessage(*(request.get())));
    m_request_sptr = request;
    m_response_writer_sptr = responseWriter;
    m_done_callback = done;
   
    Marvin::Uri tmp_uri(request->uri());
    m_host = tmp_uri.server();
    m_port = (int)tmp_uri.port();
    m_scheme = tmp_uri.scheme();
    assert( ! m_request_sptr->hasHeader("Upgrade") );
    p_round_trip_upstream(request, [this]( Marvin::ErrorType& err, MessageBaseSPtr downMsg){
        /// get here with a message suitable for transmission to down stream client
        m_response_sptr = downMsg;
        LogTrace("for downstream", Marvin::Http::traceMessage(*downMsg));
        Marvin::BufferChainSPtr responseBodySPtr = downMsg->getContentBuffer();
        /// perform the MITM collection
        
        m_collector_ptr->collect(m_scheme, m_host, m_request_sptr, m_response_sptr);
        
        /// write response to downstream client
        m_response_writer_sptr->asyncWrite(m_response_sptr, responseBodySPtr, [this](Marvin::ErrorType& err){
//            LogWarn("error: ", err.value(), err.category().name(), err.category().message(err.value()));
            LogTrace("after write downstream", " err:", Marvin::make_error_description(err));
            auto pf = std::bind(m_done_callback, err, (! err) );
            m_io.post(pf);
        });

    });
}
/// \brief Perform the proxy forwarding process; and produces a response suitable
/// for downstream transmission; the result of this method is a response to send back to the client
/// \param req : MessageReaderSPtr the request from the original client - has same value
///                                 as class property m_request_sptr
/// \param upstreamCb : called when the round trip has finished
///
void ForwardingHandler::p_round_trip_upstream(
        MessageReaderSPtr req,
        std::function<void(Marvin::ErrorType& err, MessageBaseSPtr downstreamReplyMsg)> upstreamCb
){
    /// a client object to manage the round trip of request and response to
    /// the final destination
    Marvin::Uri uri(req->uri()); /// a proxy request must have an absolute uri
    m_scheme = uri.scheme();
    m_host = uri.server();
    
    m_port = (int)uri.port();
    m_upstream_client_uptr = std::unique_ptr<Client>(new Client(m_io, m_scheme, m_host, std::to_string(m_port)));
    /// the MessageBase that will be the up stream request
    m_upstream_request_msg_sptr = std::shared_ptr<MessageBase>(new MessageBase());
    /// format upstream msg for transmission
    helpers::makeUpstreamRequest(m_upstream_request_msg_sptr, req);
    assert( ! m_request_sptr->hasHeader("Upgrade") );
    Marvin::BufferChainSPtr content = req->getContentBuffer();
    
    LogTrace("upstream request", Marvin::Http::traceMessage(*m_upstream_request_msg_sptr));
    
    m_upstream_client_uptr->asyncWrite(m_upstream_request_msg_sptr, content, [this, upstreamCb](Marvin::ErrorType& ec, MessageReaderSPtr upstrmRdr)
    {
        if (ec || (upstrmRdr == nullptr)) {
            LogWarn("async write failed");
            // TODO: how to handle error
        } else {
            LogTrace("upstream rresponse", Marvin::Http::traceMessage(*(upstrmRdr.get())));
            m_downstream_msg_sptr = std::make_shared<MessageBase>();
            m_response_body_sptr = upstrmRdr->getContentBuffer();
            helpers::makeDownstreamResponse(m_downstream_msg_sptr, upstrmRdr, ec);
            upstreamCb(ec, m_downstream_msg_sptr);
        }
    });
    
};
#endif
void SSLForwardingHandler::p_on_complete(Marvin::ErrorType& err)
{
    if( err ){
//       LogWarn("error: ", err.value(), err.category().name(), err.category().message(err.value()));
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
    LogTrace("from downstream", Marvin::Http::traceMessage(*(request.get())));
    m_request_reader_sptr = request;
    m_response_writer_sptr = responseWriter;
    m_done_callback = done;
    m_downstream_connection = clientConnectionPtr;
    
    Marvin::Uri tmp_uri(request->uri());
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
