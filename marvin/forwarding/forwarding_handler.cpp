#include "forwarding_handler.hpp"
#include "forward_helpers.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

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
    ICollector& collector
): RequestHandlerBase(io), m_collector(collector)
{
    LogTorTrace();
    m_https_hosts = s_https_hosts;
    m_https_ports = s_https_ports;
}


ForwardingHandler::~ForwardingHandler()
{
    LogTorTrace();
}

#pragma mark - handle upgrade request


void ForwardingHandler::p_handle_upgrade()
{
    // deny the upgrade
#if 0
    _resp->setStatus("Forbidden");
    _resp->setStatusCode(403);
    std::string n("");
    _resp->setContent(n);
    _resp->asyncWrite([this](Marvin::ErrorType& err){
        _doneCallback(err, false);
    });
#endif
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
        MessageReaderSPtr           req,
        ISocketSPtr     connPtr,
        HandlerDoneCallbackType     done
){
    m_req = req;
    m_downstream_connection  = connPtr;
    m_done_callback = done;
    m_initial_response_buf = std::unique_ptr<Marvin::MBuffer>(new Marvin::MBuffer(1000));
    //
    // Parse the url to determine were we have to send the "upstream" request
    //
    
    std::string tmp_url = m_req->uri();
    http::url tmp_u = http::ParseHttpUrl(tmp_url);
   
    m_scheme = tmp_u.protocol;
    m_host = tmp_u.host;
    m_port = tmp_u.port;
    m_done_callback = done;
//    m_collector = TCollector::getInstance(m_io);
    ConnectAction action = p_determine_connection_action(m_host, m_port);

    switch(action){
        case ConnectAction::TUNNEL :
            p_initiate_tunnel();
            break;
        case ConnectAction::MITM :
            assert(false);
            break;
        case ConnectAction::REJECT :
            assert(false);
            break;
    };
    
};

void ForwardingHandler::p_initiate_tunnel()
{
#if 0
    // first lets try and connect to the upstream host
    // to do that we need an upstream connection
    m_resp = std::shared_ptr<MessageWriter>(new MessageWriter(m_io, false));
    m_resp->setWriteSock(_downStreamConnection.get());
    
    LogInfo("scheme:", m_scheme, " host:", m_host, " port:", m_port);
    m_upstream_connection =
        std::shared_ptr<TCPConnection>(new TCPConnection(m_io, m_scheme, m_host, std::to_string(m_port)));
    
    m_upstream_connection->asyncConnect([this](Marvin::ErrorType& err, ISocket* conn){
        if( err ){
            LogWarn("initiateTunnel: FAILED scheme:", this->_scheme, " host:", this->_host, " port:", this->_port);
            response502Badgateway(*_resp);
            _resp->asyncWrite([this](Marvin::ErrorType& err){
                LogInfo("");
                if( err ){
                    LogWarn("error: ", err.value(), err.category().name(), err.category().message(err.value()));
                    // got an error sending response to downstream client - what can we do ? Nothing
                    auto pf = std::bind(_doneCallback, err, false);
                    _io.post(pf);
                }else{
                    auto pf = std::bind(_doneCallback, err, true);
                    _io.post(pf);
                }
            });
        }else{
            response200OKConnected(*_resp);
            _resp->asyncWrite([this](Marvin::ErrorType& err){
                LogInfo("");
                if( err ){
                    LogWarn("error: ", err.value(), err.category().name(), err.category().message(err.value()));
                    // got an error sending response to downstream client - what can we do ? Nothing
                    auto pf = std::bind(_doneCallback, err, false);
                    _io.post(pf);
                }else{
                    _tunnelHandler = std::shared_ptr<TunnelHandler>(new TunnelHandler(_downStreamConnection, _upstreamConnection));
                    _tunnelHandler->start([this](Marvin::ErrorType& err){
                        _doneCallback(err, false);
                    });
                }
            });
        }
    });
#endif
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
        ServerContext&   server_context,
        MessageReaderSPtr req,
        MessageWriterSPtr respWrtr,
        HandlerDoneCallbackType done
){
    m_req = req;
    m_resp_wrtr = respWrtr;
    m_done_callback = done;
   
    auto u = helpers::decodeUri(req);
    m_host = u.host;
    m_port = u.port;
    m_scheme = u.protocol;
    assert( ! m_req->hasHeader("Upgrade") );
    p_round_trip_upstream(req, [this]( Marvin::ErrorType& err, MessageBaseSPtr downMsg){
        /// get here with a message suitable for transmission to down stream client
        m_resp = downMsg;
        Marvin::BufferChainSPtr responseBodySPtr = downMsg->getBody();
        /// perform the MITM collection
        m_collector.collect(m_scheme, m_host, m_req, m_resp);
        /// write response to downstream client
        m_resp_wrtr->asyncWrite(m_resp, responseBodySPtr, [this](Marvin::ErrorType& err){
//            LogWarn("error: ", err.value(), err.category().name(), err.category().message(err.value()));
            auto pf = std::bind(m_done_callback, err, (! err) );
            m_io.post(pf);
        });

    });
}
/// \brief Perform the proxy forwarding process; and produces a response suitable
/// for downstream transmission; the result of this method is a response to send back to the client

void ForwardingHandler::p_round_trip_upstream(
        MessageReaderSPtr req,
        std::function<void(Marvin::ErrorType& err, MessageBaseSPtr downstreamReplyMsg)> upstreamCb
){
    /// a client object to manage the round trip of request and response to
    /// the final destination
    m_upstream_client_uptr = std::unique_ptr<Client>(new Client(m_io, m_scheme, m_host, std::to_string(m_port)));
    /// the MessageBase that will be the up stream request
    m_upstream_request_msg_sptr = std::shared_ptr<MessageBase>(new MessageBase());
    /// format upstream msg for transmission
    helpers::makeUpstreamRequest(m_upstream_request_msg_sptr, req);
    assert( ! m_req->hasHeader("Upgrade") );
    Marvin::BufferChainSPtr content = req->getBody();
    
    m_upstream_client_uptr->asyncWrite(m_upstream_request_msg_sptr, content, [this, upstreamCb](Marvin::ErrorType& ec, MessageReaderSPtr upstrmRdr)
    {
        m_downstream_msg_sptr = std::make_shared<MessageBase>();
        m_response_body_sptr = upstrmRdr->getBody();
        helpers::makeDownstreamResponse(m_downstream_msg_sptr, upstrmRdr, ec);
        upstreamCb(ec, m_downstream_msg_sptr);
    });
    
};



void ForwardingHandler::p_on_complete(Marvin::ErrorType& err)
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
#pragma mark - bodies of utility functions


ConnectAction ForwardingHandler::p_determine_connection_action(std::string host, int port)
{
    std::vector<std::regex>  regexs = this->m_https_hosts;
    std::vector<int>         ports  = this->m_https_ports;
    /// !!! this needs to be upgraded
    return ConnectAction::TUNNEL;
}

#if 0

void ForwardingHandler::response403Forbidden(MessageWriter& writer)
{
    writer.setStatus("Forbidden");
    writer.setStatusCode(403);
    std::string n("");
    writer.setContent(n);
}

void ForwardingHandler::response200OKConnected(MessageWriter& writer)
{
    writer.setStatus("OK");
    writer.setStatusCode(200);
    std::string n("");
    writer.setContent(n);
}

void ForwardingHandler::response502Badgateway(MessageWriter& writer)
{
    writer.setStatus("BAD GATEWAY");
    writer.setStatusCode(503);
    std::string n("");
    writer.setContent(n);
}
#endif
