//
//  forward_helpers.cpp
//  marvin
//
//  Created by ROBERT BLACKWELL on 12/29/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//
#include <boost/algorithm/string/trim.hpp>
#include <marvin/forwarding/forward_helpers.hpp>

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

using namespace Marvin;
using namespace Marvin::Http;

std::string helpers::base64Encode(std::string& source)
{
    return "["+source+"]";
}

http::url helpers::decodeUri(MessageReaderSPtr requestSPtr)
{
    std::string tmp_url = requestSPtr->uri();
    http::url tmp_u = http::ParseHttpUrl(tmp_url);
    return tmp_u;
}
#if 0
void helpers::fillRequestFromUri(MessageBase& msg, std::string uri_in, bool absolute)
{
    /// \note do nothing with user name and password. A proxy would not expect to see a uri with those
    /// elements. Browser wold have already put them in the header
    std::string uri(uri_in);
    http::url us = http::ParseHttpUrl(uri);
    LogDebug(" uri:", uri);
    LogDebug(" scheme:", us.protocol);
    LogDebug(" host:", us.host);
    LogDebug(" port:", us.port);
    LogDebug(" path:", us.path);
    LogDebug(" search:", us.search);
    
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
    msg.setHeader(Marvin::Http::Headers::Name::Host, host_header);
    msg.setUri(path_value);
    std::string auth_header = "";
    if(us.user != "") {
        auth_header = us.user;
        if(us.password != "") {
            auth_header += ": " + us.password;
        }
    }
    if (auth_header != "")
        msg.setHeader(Marvin::Http::Headers::Name::Authorization, base64Encode(auth_header));
}

void helpers::applyUri(MessageBaseSPtr msg, std::string uri)
{
    assert(false);
    http::url u = http::ParseHttpUrl(uri);
}
#endif
void applyUri(MessageBaseSPtr msg, Marvin::Uri& uri, bool proxy)
{
    if(proxy)
        msg->setUri(uri.absolutePath());
    else
        msg->setUri(uri.relativePath());
    msg->setHeader(Marvin::Http::Headers::Name::Host, uri.host());
}
void helpers::applyUriProxy(MessageBaseSPtr msgSPtr, Marvin::Uri& uri)
{
    applyUri(msgSPtr, uri, true);
}
void helpers::applyUriNonProxy(MessageBaseSPtr msgSPtr, Marvin::Uri& uri)
{
    applyUri(msgSPtr, uri, false);
}
void helpers::removeHopByHop(MessageBaseSPtr msgSPtr, std::string connectionValue)
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
void helpers::makeUpstreamRequest(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr)
{
    MessageReaderSPtr req = requestSPtr;
    MessageBaseSPtr result = upstreamRequest;
    
    Marvin::Uri tmp_uri(req->uri());
    
    helpers::applyUriNonProxy(upstreamRequest, tmp_uri);
//    helpers::fillRequestFromUri(*upstreamRequest, tmp_url);
    // filter out upgrade requests
    assert( ! req->hasHeader("Upgrade") );
    
    // set the method
    result->setMethod(req->method());
    // copy the headers
    // should also test for manditory Host header
    //
    auto hdrs = req->getHeaders();
    HttpHeaderFilterSetType dontCopyList{
        Marvin::Http::Headers::Name::Host,
        Marvin::Http::Headers::Name::ProxyConnection,
        Marvin::Http::Headers::Name::Connection,
        Marvin::Http::Headers::Name::ETag,
        Marvin::Http::Headers::Name::TransferEncoding
    };
    // copy all headers except those in dontCopyList
    Headers::copyExcept(hdrs, result->getHeaders(),dontCopyList);
    //    Headers::filterNotInList(hdrs, dontCopyList, [result]( Marvin::Http::Headers& hdrs, std::string k, std::string v) {
//        result->setHeader(k,v);
//    });
    if (req->hasHeader(Marvin::Http::Headers::Name::Connection)) {
        std::string cv = req->getHeader(Marvin::Http::Headers::Name::Connection);
        helpers::removeHopByHop(result, cv);
    }
    // set the uri and host header
    // no keep alive
    result->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);
    result->setHeader(Marvin::Http::Headers::Name::AcceptEncoding, "identity");
    result->setHeader(Marvin::Http::Headers::Name::TE, "");
    // Http versions defaults to 1.1, so force it to the same as the request
    result->setHttpVersMinor(req->httpVersMinor());
    result->setContent(req->getContentBuffer());
//    result->setHeader(Marvin::Http::Headers::Name::ContentLength, std::to_string(req->getBody()->size()));

}

void helpers::makeDownstreamGoodResponse(MessageBaseSPtr downstream, MessageReaderSPtr responseSPtr )
{
    LogInfo("");
    MessageReaderSPtr resp = responseSPtr;
    MessageBaseSPtr result = downstream;
        // copy the headers
    auto hdrs = resp->getHeaders();
    HttpHeaderFilterSetType dontCopyList{
        Marvin::Http::Headers::Name::Host,
        Marvin::Http::Headers::Name::ProxyConnection,
        Marvin::Http::Headers::Name::Connection,
        Marvin::Http::Headers::Name::TransferEncoding,
        Marvin::Http::Headers::Name::ETag
    };
    Headers::copyExcept(hdrs, result->getHeaders(), dontCopyList);
//    Headers::filterNotInList(hdrs, dontCopyList, [result]( Marvin::Http::Headers& hdrs, std::string k, std::string v)
//    {
//        result->setHeader(k,v);
//    });

    // set the uri and host header
    result->setStatus(resp->status());
    result->setStatusCode(resp->statusCode());
    // no keep alive
    result->setHeader(Marvin::Http::Headers::Name::Connection,  Marvin::Http::Headers::Value::ConnectionClose);
    // Http versions defaults to 1.1, so force it to the same as the request
    result->setHttpVersMinor(resp->httpVersMinor());
    // now attach the body
    std::size_t len;
    if( (len = responseSPtr->getContentBuffer()->size()) > 0){
        result->setContent(responseSPtr->getContent());
//        resp->setHeader(Marvin::Http::Headers::Name::ContentLength, std::to_string(len));
    }

}

void helpers::makeDownstreamErrorResponse(MessageBaseSPtr msg, MessageReaderSPtr resp, Marvin::ErrorType& err)
{
    LogDebug("");
    // bad gateway 502
    msg->setStatus("Bad gateway");
    msg->setStatusCode(501);
    msg->setHeader(Marvin::Http::Headers::Name::ContentLength, std::to_string(0));
    std::string n("");
//    _resp->setContent(n);
}
void helpers::makeDownstreamResponse(MessageBaseSPtr msg_sptr, MessageReaderSPtr resp, Marvin::ErrorType& err)
{
    if( err ){
        helpers::makeDownstreamErrorResponse(msg_sptr, resp, err);
        LogTrace(Marvin::make_error_description(err));
    }else{
        helpers::makeDownstreamGoodResponse(msg_sptr, resp);
//        makeDownstreamResponse();
    }
}

bool helpers::apply_connection_close(MessageReaderSPtr req, MessageBaseSPtr response)
{
    response->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);
    return false;
}

bool helpers::apply_keepalive_rules(MessageReaderSPtr req, MessageBaseSPtr response)
{
    /// correctly handle keep-alive/close
    bool keep_alive;
    if(req->getHeader(Marvin::Http::Headers::Name::Connection) == Marvin::Http::Headers::Value::ConnectionKeepAlive) {
        keep_alive = true;
        response->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionKeepAlive);
    } else {
        keep_alive = false;
        response->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);
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

