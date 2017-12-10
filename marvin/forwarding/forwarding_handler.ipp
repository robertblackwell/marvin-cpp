
enum class ConnectAction{
    TUNNEL=11,
    MITM,
    REJECT
};

template<class TCollector>
std::vector<std::regex> ForwardingHandler<TCollector>::__httpsHosts = std::vector<std::regex>();

template<class TCollector>
std::vector<int> ForwardingHandler<TCollector>::__httpsPorts = std::vector<int>();


template<class TCollector>
void ForwardingHandler<TCollector>::configSet_HttpsHosts(std::vector<std::regex> re)
{
    __httpsHosts = re;
}

template<class TCollector>
void ForwardingHandler<TCollector>::configSet_HttpsPorts(std::vector<int> ports)
{
    __httpsPorts = ports;
}

#pragma mark - Forward handler class
template<class TCollector>
ForwardingHandler<TCollector>::ForwardingHandler(
    boost::asio::io_service& io
): RequestHandlerBase(io)
{
    LogTorTrace();
    _httpsHosts = __httpsHosts;
    _httpsPorts = __httpsPorts;
}

template<class TCollector>
ForwardingHandler<TCollector>::~ForwardingHandler()
{
    LogTorTrace();
}

#pragma mark - handle upgrade request

template<class TCollector>
void ForwardingHandler<TCollector>::handleUpgrade()
{
    // deny the upgrade
    _resp->setStatus("Forbidden");
    _resp->setStatusCode(403);
    std::string n("");
    _resp->setContent(n);
    _resp->asyncWrite([this](Marvin::ErrorType& err){
        _doneCallback(err, false);
    });
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
template<class TCollector>
void ForwardingHandler<TCollector>::handleConnect(
        MessageReaderSPtr           req,
        ConnectionInterfaceSPtr     connPtr,
        HandlerDoneCallbackType     done
){
    LogInfo("");
    _req = req;
    _downStreamConnection  = connPtr;
    _doneCallback = done;
    _initialResponseBuf = std::unique_ptr<MBuffer>(new MBuffer(1000));
    int x = 2;
    //
    // Parse the url to determine were we have to send the "upstream" request
    //
    
    std::string __url = _req->uri();
    http::url _u = http::ParseHttpUrl(__url);
    LogDebug(" uri:", _req->uri());
    LogDebug(" scheme:", _u.protocol);
    LogDebug(" host:", _u.host);
    LogDebug(" port:", _u.port);
    LogDebug(" path:", _u.path);
    LogDebug(" query:", _u.search);
    
    _scheme = _u.protocol;
    _host = _u.host;
    _port = _u.port;
    _doneCallback = done;
    _collector = TCollector::getInstance(_io);
    ConnectAction action = determineConnecAction(_host, _port);

    switch(action){
        case ConnectAction::TUNNEL :
            initiateTunnel();
            break;
        case ConnectAction::MITM :
            assert(false);
            break;
        case ConnectAction::REJECT :
            assert(false);
            break;
    };
    
};
template<class TCollector>
void ForwardingHandler<TCollector>::initiateTunnel()
{
    // first lets try and connect to the upstream host
    // to do that we need an upstream connection
    _resp = std::shared_ptr<MessageWriter>(new MessageWriter(_io, false));
    _resp->setWriteSock(_downStreamConnection.get());
    
    LogInfo("scheme:", _scheme, " host:", _host, " port:", _port);
    _upstreamConnection =
        std::shared_ptr<TCPConnection>(new TCPConnection(_io, _scheme, _host, std::to_string(_port)));
    
    _upstreamConnection->asyncConnect([this](Marvin::ErrorType& err, ConnectionInterface* conn){
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
template<class TCollector>
void ForwardingHandler<TCollector>::handleRequest(
        MessageReaderSPtr req,
        MessageWriterSPtr resp,
        HandlerDoneCallbackType done
){
    LogInfo("");
    _req = req;
    _resp = resp;
    _doneCallback = done;
    _collector = TCollector::getInstance(_io);
    handleRequest_Upstream(req, [this, req, resp](Marvin::ErrorType& err){

        handleUpstreamResponseReceived(err);

        _collector->collect(_scheme, _host, _req, _resp);

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

    });
}
/// This method kicks off the forwarding process by pasing the request upstream
template<class TCollector>
void ForwardingHandler<TCollector>::handleRequest_Upstream(
        MessageReaderSPtr req,
        std::function<void(Marvin::ErrorType& err)> upstreamCb
){
    LogInfo("");
    //
    // Parse the url to determine were we have to send the "upstream" request
    //
    std::string __url = _req->uri();
    http::url _u = http::ParseHttpUrl(__url);
    LogDebug(" uri:", _req->uri());
    LogDebug(" scheme:", _u.protocol);
    LogDebug(" host:", _u.host);
    LogDebug(" port:", _u.port);
    LogDebug(" path:", _u.path);
    LogDebug(" query:", _u.search);
    
    _host = _u.host;
    _scheme = _u.protocol;
    
    _upStreamRequestUPtr = RequestUPtr(new Request(_io));
    LogInfo("",traceReader(*_req));
    
//    _req->dumpHeaders(std::cerr);
    
    // filter out upgrade requests
    if( _req->hasHeader("Upgrade") ){
        handleUpgrade();
        return;
    }
    
    // set the method
    _upStreamRequestUPtr->setMethod(_req->method());
    // copy the headers
    // should also test for manditory Host header
    //
    auto hdrs = _req->getHeaders();

    HttpHeaderFilterSetType dontCopyList{
        HttpHeader::Name::Host,
        HttpHeader::Name::ProxyConnection,
        HttpHeader::Name::Connection
    };
    
    HttpHeader::filterNotInList(hdrs, dontCopyList, [this]( HttpHeadersType& hdrs,
                                                        std::string k,
                                                         std::string v)
    {
        this->_upStreamRequestUPtr->setHeader(k,v);
    });
    // set the uri and host header
    _upStreamRequestUPtr->setUrl(_req->uri());
    // no keep alive
    _upStreamRequestUPtr->setHeader("Connection", "close");
    _upStreamRequestUPtr->setHeader("Accept-encoding", "identity");
    // Http versions defaults to 1.1, so force it to the same as the request
    _upStreamRequestUPtr->setHttpVersMinor(_req->httpVersMinor());
    // now attach the body
    _upStreamRequestUPtr->setContent(_req->getBody());
    _upStreamRequestUPtr->go([this, upstreamCb](Marvin::ErrorType& err){
        upstreamCb(err);
    });
    
};

template<class TCollector>
void ForwardingHandler<TCollector>::handleUpstreamResponseReceived(Marvin::ErrorType& err)
{
    LogInfo("",traceRequest(*_upStreamRequestUPtr));

    if( err ){
        // this means we got an error NOT a response wit an error status code
        // so we have to consttruct a response
        makeDownstreamErrorResponse(err);
        LogTrace(Marvin::make_error_description(err));
    }else{
        // Got a response from the upstream server
        // use it to create the downstream response
        makeDownstreamResponse();
    }
    _upStreamRequestUPtr->end(); ///!!!
}

template<class TCollector>
void ForwardingHandler<TCollector>::makeDownstreamResponse()
{
    LogInfo("");
    MessageReader& upStreamResponse = _upStreamRequestUPtr->getResponse();
    LogTrace("got from server ", traceReader(upStreamResponse));
    
    // copy the headers
    auto hdrs = upStreamResponse.getHeaders();
    HttpHeaderFilterSetType dontCopyList{
        HttpHeader::Name::Host,
        HttpHeader::Name::ProxyConnection,
        HttpHeader::Name::Connection,
        HttpHeader::Name::TransferEncoding,
        HttpHeader::Name::ETag
    };
    
    HttpHeader::filterNotInList(hdrs, dontCopyList, [this]( HttpHeadersType& hdrs,
                                                        std::string k,
                                                         std::string v)
    {
        this->_resp->setHeader(k,v);
    });

    // set the uri and host header
    _resp->setStatus(upStreamResponse.status());
    _resp->setStatusCode(upStreamResponse.statusCode());
    // no keep alive
    _resp->setHeader("Connection", "close");
    // Http versions defaults to 1.1, so force it to the same as the request
    _resp->setHttpVersMinor(upStreamResponse.httpVersMinor());
    // now attach the body
    
    std::size_t len;
    if( (len = upStreamResponse.getBody().size()) > 0){
        _resp->setContent(upStreamResponse.getBody());
        _resp->setHeader("Content-length", std::to_string(len));
    }
}

template<class TCollector>
void ForwardingHandler<TCollector>::makeDownstreamErrorResponse(Marvin::ErrorType& err)
{
    LogDebug("");
    // bad gateway 502
    _resp->setStatus("Bad gateway");
    _resp->setStatusCode(501);
    std::string n("");
    _resp->setContent(n);
}
template<class TCollector>
void ForwardingHandler<TCollector>::onComplete(Marvin::ErrorType& err)
{
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
}
#pragma mark - bodies of utility functions

template<class TCollector>
ConnectAction ForwardingHandler<TCollector>::determineConnecAction(std::string host, int port)
{
    std::vector<std::regex>  regexs = this->_httpsHosts;
    std::vector<int>         ports  = this->_httpsPorts;
    /// !!! this needs to be upgraded
    return ConnectAction::TUNNEL;
}


template<class TCollector>
void ForwardingHandler<TCollector>::response403Forbidden(MessageWriter& writer)
{
    writer.setStatus("Forbidden");
    writer.setStatusCode(403);
    std::string n("");
    writer.setContent(n);
}
template<class TCollector>
void ForwardingHandler<TCollector>::response200OKConnected(MessageWriter& writer)
{
    writer.setStatus("OK");
    writer.setStatusCode(200);
    std::string n("");
    writer.setContent(n);
}
template<class TCollector>
void ForwardingHandler<TCollector>::response502Badgateway(MessageWriter& writer)
{
    writer.setStatus("BAD GATEWAY");
    writer.setStatusCode(503);
    std::string n("");
    writer.setContent(n);
}
