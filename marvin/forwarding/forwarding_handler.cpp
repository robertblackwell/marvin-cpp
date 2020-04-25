#include <marvin/http/message_factory.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/forwarding/forwarding_handler.hpp>
#include <marvin/forwarding/forward_helpers.hpp>
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

namespace Marvin {

std::string traceForwardingHandler(ForwardingHandler* fh_ptr)
{
    std::stringstream ss;
    ss << "FWRDH[" << std::hex << (long)(void*)fh_ptr << std::dec << "]: ";
    return ss.str();
}
std::string traceForwardingHandler(ForwardingHandlerUPtr fh_ptr)
{
    return traceForwardingHandler(fh_ptr.get());
//    std::stringstream ss;
//    ss << "FWRDH[" << std::hex << (long)(void*)fh_ptr.get() << std::dec << "]: ";
//    return ss.str();
}
std::string traceFowardingHandler(ForwardingHandlerSPtr fh_ptr)
{
    return traceForwardingHandler(fh_ptr.get());
//    std::stringstream ss;
//    ss << "FWRDH[" << std::hex << (long)(void*)fh_ptr.get() << std::dec << "]: ";
//    return ss.str();
}


enum class ConnectAction{
    TUNNEL=11,
    MITM,
    REJECT
};


std::vector<std::regex> ForwardingHandler::s_https_hosts = std::vector<std::regex>();


std::vector<int> ForwardingHandler::s_https_ports = std::vector<int>();



void ForwardingHandler::configSet_HttpsHosts(std::vector<std::regex> re)
{
    s_https_hosts = re;
}


void ForwardingHandler::configSet_HttpsPorts(std::vector<int> ports)
{
    s_https_ports = ports;
}

#pragma mark - Forward handler class

ForwardingHandler::ForwardingHandler(
    boost::asio::io_service& io,
    ICollector* collector_ptr
): RequestHandlerBase(io), m_collector_ptr(collector_ptr)
{
   TROG_TRACE_CTOR();
    m_https_hosts = s_https_hosts;
    m_https_ports = s_https_ports;
}


ForwardingHandler::~ForwardingHandler()
{
   TROG_TRACE_CTOR();
}

#pragma mark - handle upgrade request


void ForwardingHandler::p_handle_upgrade()
{
    // deny the upgrade

}

#pragma mark - handle connect request

/// @description Handles a CONNECT request.
///
/// If the request is a HTTPS tunnel request then test to see
/// if this is a host of interest and in that case
/// try for a mitm/https link (more on this elsewhere).
///
/// Other wise try to establish a tunnel. First try to establish a connection with the
/// intended destination host, if successful reply to client with a 200 else reply with a
/// 502. If target host is connected that hand off to a tunnel class.
///
/// NOTE !!! If it looks like a tunnel will be established MUST call done(true)
///
/// BEFORE this method returns so that the server does not close the client connection.
///
/// done(true) signals to the server that this method is "hijacking" the connection

void ForwardingHandler::handleConnect(
        ServerContext&              server_context,
        MessageReaderSPtr           request,    // the initial request as a MessageReader
        MessageWriterSPtr           responseWriter,
        ISocketSPtr                 clientConnectionSPtr,// the connection to the client
        HandlerDoneCallbackType     done
){
   TROG_TRACE3(traceForwardingHandler(this), Marvin::traceMessage(*(request.get())));
    m_request_sptr = request;
    m_downstream_connection  = clientConnectionSPtr;
    m_done_callback = done;
    m_response_writer_sptr = responseWriter;
    m_initial_response_buf = std::unique_ptr<Marvin::MBuffer>(new Marvin::MBuffer(1000));
    //
    // Parse the url to determine were we have to send the "upstream" request
    //
    
    std::string tmp_url = m_request_sptr->uri();
    http::url tmp_u = http::ParseHttpUrl(tmp_url);
   
    m_scheme = tmp_u.protocol;
    m_host = tmp_u.host;
    m_port = tmp_u.port;
    m_done_callback = done;

    ConnectAction action = p_determine_connection_action(m_host, m_port);
    ConnectionSPtr conn_sptr = std::dynamic_pointer_cast<Connection>(clientConnectionSPtr);

    switch(action){
        case ConnectAction::TUNNEL :
            p_initiate_tunnel();
            break;
        case ConnectAction::MITM :
            m_ssl_handler_sptr = std::make_shared<SSLForwardingHandler>(m_io, m_collector_ptr);
            m_ssl_handler_sptr->handleSSLRequest(server_context, request, responseWriter, conn_sptr, done);
            break;
        case ConnectAction::REJECT :
            assert(false);
            break;
    };
    
};

void ForwardingHandler::p_initiate_tunnel()
{
    // first lets try and connect to the upstream host
    // to do that we need an upstream connection
//    m_response_sptr->setWriteSock(m_downstream_connection.get());
//    m_resp = std::shared_ptr<MessageWriter>(new MessageWriter(m_io, false));

   TROG_TRACE3("scheme:", m_scheme, " host:", m_host, " port:", m_port);

    m_upstream_connection = socketFactory(m_io, m_scheme, m_host, std::to_string(m_port));
    m_upstream_connection->asyncConnect([this](Marvin::ErrorType& err, ISocket* conn){
        if( err ) {
            TROG_WARN("initiateTunnel: FAILED scheme:", this->m_scheme, " host:", this->m_host, " port:", this->m_port);
            m_response_sptr = std::make_shared<MessageBase>();
            makeResponse502Badgateway(*m_response_sptr);

            m_response_writer_sptr->asyncWrite(m_response_sptr, [this](Marvin::ErrorType& err){
                TROG_INFO("");
                if( err ) {
                    TROG_WARN("error: ", err.value(), err.category().name(), err.category().message(err.value()));
                    // got an error sending response to downstream client - what can we do ? Nothing
                    auto pf = std::bind(m_done_callback, err, false);
                    m_io.post(pf);
                } else {
                    auto pf = std::bind(m_done_callback, err, true);
                    m_io.post(pf);
                }
            });
        } else {
           TROG_TRACE3("initiateTunnel: connection SUCCEEDED scheme:", traceForwardingHandler(this), " scheme:",this->m_scheme, " host:", this->m_host, " port:", this->m_port);
            m_response_sptr = std::make_shared<MessageBase>();
            makeResponse200OKConnected(*m_response_sptr);
            m_response_writer_sptr->asyncWrite(m_response_sptr, [this](Marvin::ErrorType& err){
                TROG_INFO("");
                if( err ) {
                    TROG_WARN("error: ", err.value(), err.category().name(), err.category().message(err.value()));
                    // got an error sending response to downstream client - what can we do ? Nothing
                    auto pf = std::bind(m_done_callback, err, false);
                    m_io.post(pf);
                } else {
                    m_tunnel_handler = std::make_shared<TunnelHandler>(m_io, m_downstream_connection, m_upstream_connection);
                   TROG_TRACE3("start tunnel", traceForwardingHandler(this), traceTunnel(m_tunnel_handler));
                    m_tunnel_handler->start([this](Marvin::ErrorType& err){
//                        m_done_callback(err, false);
                       TROG_TRACE3("tunnel complete OK", traceForwardingHandler(this), traceTunnel(m_tunnel_handler), " err: ", Marvin::make_error_description(err));
                        auto pf = std::bind(m_done_callback, err, false);
                        m_io.post(pf);
                    });
                }
            });
        }
    });
}
#pragma mark - handle a "normal" request
///
/// @description Handles a normal (not CONNECT) http request contained in req of type MessageReaderSPtr
/// (a shared_ptr to a messageReader).
/// Sends the ulimate response back to the origninal client via a MessageWriter pointed to by
/// the shared_ptr resp
/// After all is over call done(true) to signal that the server shell can destroy the connection
/// and the reader and writer
/// @param req
///

void ForwardingHandler::handleRequest(
        ServerContext&          server_context,
        MessageReaderSPtr       request,
        MessageWriterSPtr       responseWriter,
        ISocketSPtr             clientConnectionPtr,
        HandlerDoneCallbackType done
){
   TROG_TRACE3("from downstream", Marvin::traceMessage(*(request.get())));
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
       TROG_TRACE3("for downstream", Marvin::traceMessage(*downMsg));
        Marvin::BufferChainSPtr responseBodySPtr = downMsg->getContentBuffer();
        /// perform the MITM collection
        
        m_collector_ptr->collect(m_scheme, m_host, m_request_sptr, m_response_sptr);
        
        /// write response to downstream client
        m_response_writer_sptr->asyncWrite(m_response_sptr, responseBodySPtr, [this](Marvin::ErrorType& err){
//            TROG_WARN("error: ", err.value(), err.category().name(), err.category().message(err.value()));
           TROG_TRACE3("after write downstream", " err:", Marvin::make_error_description(err));
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
    Helpers::makeUpstreamRequest(m_upstream_request_msg_sptr, req);
    assert( ! m_request_sptr->hasHeader("Upgrade") );
    Marvin::BufferChainSPtr content = req->getContentBuffer();
    
   TROG_TRACE3("upstream request", Marvin::traceMessage(*m_upstream_request_msg_sptr));
    
    m_upstream_client_uptr->asyncWrite(m_upstream_request_msg_sptr, content, [this, upstreamCb](Marvin::ErrorType& ec, MessageReaderSPtr upstrmRdr)
    {
        if (ec || (upstrmRdr == nullptr)) {
            TROG_WARN("async write failed");
            // TODO: how to handle error
        } else {
           TROG_TRACE3("upstream rresponse", Marvin::traceMessage(*(upstrmRdr.get())));
            m_downstream_msg_sptr = std::make_shared<MessageBase>();
            m_response_body_sptr = upstrmRdr->getContentBuffer();
            Helpers::makeDownstreamResponse(m_downstream_msg_sptr, upstrmRdr, ec);
            upstreamCb(ec, m_downstream_msg_sptr);
        }
    });
    
};

void ForwardingHandler::p_on_complete(Marvin::ErrorType& err)
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
#pragma mark - bodies of utility functions

ConnectAction ForwardingHandler::p_determine_connection_action(std::string host, int port)
{
    std::vector<std::regex>  regexs = this->m_https_hosts;
    std::vector<int>         ports  = this->m_https_ports;
    /// !!! this needs to be upgraded
    if (port == 443) {
        return ConnectAction::MITM;
    }
    return ConnectAction::TUNNEL;
}
}
//
