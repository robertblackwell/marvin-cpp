#include <marvin/helpers/mitm.hpp>

#include <boost/algorithm/string/trim.hpp>

TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

namespace Marvin::Helpers {

typedef std::set<std::string> HttpHeaderFilterSetType;

std::string base64Encode(std::string& source)
{
    return "["+source+"]";
}

http::url decodeUri(MessageReaderSPtr requestSPtr)
{
    std::string tmp_url = requestSPtr->uri();
    http::url tmp_u = http::ParseHttpUrl(tmp_url);
    return tmp_u;
}
#if 0
void fillRequestFromUri(MessageBase& msg, std::string uri_in, bool absolute)
{
    /// \note do nothing with user name and password. A proxy would not expect to see a uri with those
    /// elements. Browser wold have already put them in the header
    std::string uri(uri_in);
    http::url us = http::ParseHttpUrl(uri);
    TROG_DEBUG(" uri:", uri);
    TROG_DEBUG(" scheme:", us.protocol);
    TROG_DEBUG(" host:", us.host);
    TROG_DEBUG(" port:", us.port);
    TROG_DEBUG(" path:", us.path);
    TROG_DEBUG(" search:", us.search);
    
    std::string host = us.host;
    std::string scheme = us.protocol;
    
    int port_int = us.port;
    if(us.port == 0) {
        if( us.protocol == "https")
            port_int = 443;
        else if( us.protocol == "http")
            port_int = 80;
        else
            assert(false);
    }
    assert(port_int != 0);
    std::string host_header = us.host+":"+std::to_string(port_int);
    std::string path_value;
    if( (us.path == "") && (us.search == "")) {
        path_value = "/";
    }else if( us.path == "") {
        path_value = "/?" + us.search;
    } else {
        path_value = us.path +"?"+ us.search;
    }
    if (absolute) {
        path_value = us.protocol+"://"+host_header+path_value;
    }
    msg.setHeader(Marvin::HeadersV2::Host, host_header);
    msg.setUri(path_value);
    std::string auth_header = "";
    if(us.user != "") {
        auth_header = us.user;
        if(us.password != "") {
            auth_header += ": " + us.password;
        }
    }
    if (auth_header != "")
        msg.setHeader(Marvin::HeadersV2::Authorization, base64Encode(auth_header));
}

void applyUri(MessageBaseSPtr msg, std::string uri)
{
    assert(false);
    http::url u = http::ParseHttpUrl(uri);
}
#endif
void applyUri(MessageBaseSPtr msg, Uri& uri, bool proxy)
{
    if(proxy)
        msg->setUri(uri.absolutePath());
    else
        msg->setUri(uri.relativePath());
    msg->setHeader(Marvin::HeadersV2::Host, uri.host());
}
void applyUriProxy(MessageBaseSPtr msgSPtr, Uri& uri)
{
    applyUri(msgSPtr, uri, true);
}
void applyUriNonProxy(MessageBaseSPtr msgSPtr, Uri& uri)
{
    applyUri(msgSPtr, uri, false);
}
void removeHopByHop(MessageBaseSPtr msgSPtr, std::string connectionValue)
{
    char_separator<char> sep(",");
    
    boost::tokenizer<boost::char_separator<char>> tokens(connectionValue, sep);
    for (const auto& t : tokens) {
        std::string str(t);
        boost::algorithm::trim (str);
        msgSPtr->removeHeader(str);
//        std::string s = std::string(t.c_str());
//        std::string s2 = t;
//        std::cout << t << "." << std::endl;
    }
}
void makeUpstreamRequest(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr)
{
    MessageReaderSPtr req = requestSPtr;
    MessageBaseSPtr result = upstreamRequest;
    
    Marvin::Uri tmp_uri(req->uri());
    
    applyUriNonProxy(upstreamRequest, tmp_uri);
//    helpers::fillRequestFromUri(*upstreamRequest, tmp_url);
    // filter out upgrade requests
    assert( ! req->hasHeader("Upgrade") );
    
    // set the method
    result->setMethod(req->method());
    // copy the headers
    // should also test for manditory Host header
    //
    auto hdrs = req->getHeaders();
    std::set<std::string> dontCopyList{
        Marvin::HeadersV2::Host,
        Marvin::HeadersV2::ProxyConnection,
        Marvin::HeadersV2::Connection,
        Marvin::HeadersV2::ETag,
        Marvin::HeadersV2::TransferEncoding
    };
    // copy all headers except those in dontCopyList
    HeadersV2::copyExcept(hdrs, result->getHeaders(),dontCopyList);
    //    Headers::filterNotInList(hdrs, dontCopyList, [result]( Marvin::Headers& hdrs, std::string k, std::string v) {
//        result->setHeader(k,v);
//    });
    if (req->hasHeader(Marvin::HeadersV2::Connection)) {
        std::string cv = req->getHeader(Marvin::HeadersV2::Connection);
        removeHopByHop(result, cv);
    }
    // set the uri and host header
    // no keep alive
    result->setHeader(Marvin::HeadersV2::Connection, Marvin::HeadersV2::ConnectionClose);
    result->setHeader(Marvin::HeadersV2::AcceptEncoding, "identity");
    result->setHeader(Marvin::HeadersV2::TE, "");
    // Http versions defaults to 1.1, so force it to the same as the request
    result->setHttpVersMinor(req->httpVersMinor());
    result->setContent(req->getContentBuffer());
//    result->setHeader(Marvin::HeadersV2::ContentLength, std::to_string(req->getBody()->size()));

}

void makeUpstreamHttpsRequest(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr)
{
    MessageReaderSPtr req = requestSPtr;
    MessageBaseSPtr result = upstreamRequest;
    
    result->setPath(req->getPath());
    result->setMethod(req->method());
    int nh = req->getHeaders().size();
    for(int i = 0; i < nh; i++ ) {
        auto y = req->getHeaders().atIndex(i);
        result->setHeader(y.first, y.second);
    }
    auto cb = req->getContentBuffer();
    result->setContentBuffer(cb);
    return;
    Marvin::Uri tmp_uri(req->uri());
    
    applyUriNonProxy(upstreamRequest, tmp_uri);
//    helpers::fillRequestFromUri(*upstreamRequest, tmp_url);
    // filter out upgrade requests
    assert( ! req->hasHeader("Upgrade") );
    
    // set the method
    result->setMethod(req->method());
    // copy the headers
    // should also test for manditory Host header
    //
    auto hdrs = req->getHeaders();
    std::set<std::string> dontCopyList{
        Marvin::HeadersV2::Host,
        Marvin::HeadersV2::ProxyConnection,
        Marvin::HeadersV2::Connection,
        Marvin::HeadersV2::ETag,
        Marvin::HeadersV2::TransferEncoding
    };
    // copy all headers except those in dontCopyList
    HeadersV2::copyExcept(hdrs, result->getHeaders(),dontCopyList);
    //    Headers::filterNotInList(hdrs, dontCopyList, [result]( Marvin::Headers& hdrs, std::string k, std::string v) {
//        result->setHeader(k,v);
//    });
    if (req->hasHeader(Marvin::HeadersV2::Connection)) {
        std::string cv = req->getHeader(Marvin::HeadersV2::Connection);
        removeHopByHop(result, cv);
    }
    // set the uri and host header
    // no keep alive
    result->setHeader(Marvin::HeadersV2::Connection, Marvin::HeadersV2::ConnectionClose);
    result->setHeader(Marvin::HeadersV2::AcceptEncoding, "identity");
    result->setHeader(Marvin::HeadersV2::TE, "");
    // Http versions defaults to 1.1, so force it to the same as the request
    result->setHttpVersMinor(req->httpVersMinor());
    result->setContent(req->getContentBuffer());
//    result->setHeader(Marvin::HeadersV2::ContentLength, std::to_string(req->getBody()->size()));

}


void makeDownstreamGoodResponse(MessageBaseSPtr downstream, MessageReaderSPtr responseSPtr )
{
    TROG_INFO("");
    MessageReaderSPtr resp = responseSPtr;
    MessageBaseSPtr result = downstream;
        // copy the headers
    auto hdrs = resp->getHeaders();
    HttpHeaderFilterSetType dontCopyList{
        Marvin::HeadersV2::Host,
        Marvin::HeadersV2::ProxyConnection,
        Marvin::HeadersV2::Connection,
        Marvin::HeadersV2::TransferEncoding,
        Marvin::HeadersV2::ETag
    };
    HeadersV2::copyExcept(hdrs, result->getHeaders(), dontCopyList);
//    Headers::filterNotInList(hdrs, dontCopyList, [result]( Marvin::Headers& hdrs, std::string k, std::string v)
//    {
//        result->setHeader(k,v);
//    });

    // set the uri and host header
    result->setStatus(resp->status());
    result->setStatusCode(resp->statusCode());
    // no keep alive
    result->setHeader(Marvin::HeadersV2::Connection,  Marvin::HeadersV2::ConnectionClose);
    // Http versions defaults to 1.1, so force it to the same as the request
    result->setHttpVersMinor(resp->httpVersMinor());
    // now attach the body
    std::size_t len;
    if( (len = responseSPtr->getContentBuffer()->size()) > 0){
        result->setContent(responseSPtr->getContent());
//        resp->setHeader(Marvin::HeadersV2::ContentLength, std::to_string(len));
    }

}

void makeDownstreamErrorResponse(MessageBaseSPtr msg, MessageReaderSPtr resp, Marvin::ErrorType& err)
{
    TROG_DEBUG("");
    // bad gateway 502
    msg->setStatus("Bad gateway");
    msg->setStatusCode(501);
    msg->setHeader(Marvin::HeadersV2::ContentLength, std::to_string(0));
    std::string n("");
}
void makeDownstreamResponse(MessageBaseSPtr msg_sptr, MessageReaderSPtr resp, Marvin::ErrorType& err)
{
    if( err ){
        makeDownstreamErrorResponse(msg_sptr, resp, err);
       TROG_TRACE3(Marvin::make_error_description(err));
    }else{
        makeDownstreamGoodResponse(msg_sptr, resp);
//        makeDownstreamResponse();
    }
}

bool apply_connection_close(MessageReaderSPtr req, MessageBaseSPtr response)
{
    response->setHeader(Marvin::HeadersV2::Connection, Marvin::HeadersV2::ConnectionClose);
    return false;
}

bool apply_keepalive_rules(MessageReaderSPtr req, MessageBaseSPtr response)
{
    /// correctly handle keep-alive/close
    bool keep_alive;
    if(req->getHeader(Marvin::HeadersV2::Connection) == Marvin::HeadersV2::ConnectionKeepAlive) {
        keep_alive = true;
        response->setHeader(Marvin::HeadersV2::Connection, Marvin::HeadersV2::ConnectionKeepAlive);
    } else {
        keep_alive = false;
        response->setHeader(Marvin::HeadersV2::Connection, Marvin::HeadersV2::ConnectionClose);
    }
    return keep_alive;
}

void response403Forbidden(MessageBaseSPtr msg)
{
    msg->setStatus("Forbidden");
    msg->setStatusCode(403);
    msg->setContent("");
}
void response200OKConnected(MessageBaseSPtr msg)
{
    msg->setStatus("OK");
    msg->setStatusCode(200);
    msg->setContent("");
}
void response502Badgateway(MessageBaseSPtr msg)
{
    msg->setStatus("BAD GATEWAY");
    msg->setStatusCode(503);
    msg->setContent("");
}
} // namespace
