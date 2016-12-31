

void response403Forbidden(boost::asio::streambuf& sbuf);
void response200OKConnected(boost::asio::streambuf& sbuf);
void response502Badgateway(boost::asio::streambuf& sbuf);

template<class TCollector>
ForwardingHandler<TCollector>::ForwardingHandler(
    boost::asio::io_service& io
): RequestHandlerBase(io)
{}

template<class TCollector>
ForwardingHandler<TCollector>::~ForwardingHandler(){}

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
            HandlerDoneCallbackType done);
){
    _collector = TCollector::getInstance(_io);

    LogInfo("");
};

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
    // set up the callback on response received and GO
    auto hf = std::bind(&ForwardingHandler<TCollector>::handleUpstreamResponseReceived, this, std::placeholders::_1);
    _upStreamRequestUPtr->go(hf);
    
};

template<class TCollector>
void ForwardingHandler<TCollector>::handleUpstreamResponseReceived(Marvin::ErrorType& err)
{
    LogInfo("");
    LogInfo("",traceRequest(*_upStreamRequestUPtr));

    if( err ){
        LogWarn("");
        // this means we got an error NOT a response wit an error status code
        // so we have to consttruct a response
        LogTrace(Marvin::make_error_description(err));
    }else{
        // Got a response from the upstream server
        // use it to create the downstream response
        makeDownstreamResponse();
    }
    _upStreamRequestUPtr->end();
    LogTrace("send to client", traceWriter(*_resp));
    auto hf = std::bind(&ForwardingHandler<TCollector>::onComplete, this, std::placeholders::_1);
    _collector->collect(_scheme, _host, _req, _resp);
    _resp->asyncWrite(hf);
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

template<class TCollector>
void response403Forbidden(boost::asio::streambuf& sbuf)
{
    std::ostream   os(&sbuf);
    os << "Http/1.1 401 Forbidden\r\n\r\b";
}
template<class TCollector>
void response200OKConnected(boost::asio::streambuf& sbuf)
{
    std::ostream   os(&sbuf);
    os << "Http/1.1 200 OK\r\n\r\b";
}
template<class TCollector>
void response502Badgateway(boost::asio::streambuf& sbuf)
{
    std::ostream   os(&sbuf);
    os << "Http/1.1 502 Bad Gateway\r\n\r\b";
}
