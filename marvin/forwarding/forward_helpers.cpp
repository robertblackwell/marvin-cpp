#include <marvin/forwarding/forward_helpers.hpp>

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
    std::string tmp_url = requestSPtr->target();
    http::url tmp_u = http::ParseHttpUrl(tmp_url);
    return tmp_u;
}

void applyUri(MessageBaseSPtr msg, Uri& uri, bool proxy)
{
    if(proxy)
        msg->target(uri.absolutePath());
    else
        msg->target(uri.relativePath());
    msg->header(Marvin::HeadersV2::Host, uri.host_and_port());
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
        msgSPtr->remove_header(str);
//        std::string s = std::string(t.c_str());
//        std::string s2 = t;
//        std::cout << t << "." << std::endl;
    }
}
void makeUpstreamRequest(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr)
{
    MessageReaderSPtr req = requestSPtr;
    MessageBaseSPtr result = upstreamRequest;
    
    Marvin::Uri tmp_uri(req->target());
    
    applyUriNonProxy(upstreamRequest, tmp_uri);
//    helpers::fillRequestFromUri(*upstreamRequest, tmp_url);
    // filter out upgrade requests
    assert( ! req->header("Upgrade") );
    
    // set the method
    result->method(req->method());
    // copy the headers
    // should also test for manditory Host header
    //
    auto hdrs = req->headers();
    std::set<std::string> dontCopyList{
        Marvin::HeadersV2::Host,
        Marvin::HeadersV2::ProxyConnection,
        Marvin::HeadersV2::Connection,
        Marvin::HeadersV2::ETag,
        Marvin::HeadersV2::TransferEncoding
    };
    // copy all headers except those in dontCopyList
    HeadersV2::copyExcept(hdrs, result->headers(),dontCopyList);
    //    Headers::filterNotInList(hdrs, dontCopyList, [result]( Marvin::Headers& hdrs, std::string k, std::string v) {
//        result->header(k,v);
//    });
    if (auto hopt = req->header(Marvin::HeadersV2::Connection)) {
        std::string cv = hopt.get();
        removeHopByHop(result, cv);
    }
    // set the uri and host header
    // no keep alive
    result->header(Marvin::HeadersV2::Connection, Marvin::HeadersV2::ConnectionClose);
    result->header(Marvin::HeadersV2::AcceptEncoding, "identity");
    result->header(Marvin::HeadersV2::TE, "");
    // Http versions defaults to 1.1, so force it to the same as the request
    result->version_minor(req->version_minor());
    result->setContent(req->getContentBuffer());
//    result->header(Marvin::HeadersV2::ContentLength, std::to_string(req->getBody()->size()));

}

void makeDownstreamGoodResponse(MessageBaseSPtr downstream, MessageReaderSPtr responseSPtr )
{
    TROG_INFO("");
    MessageReaderSPtr resp = responseSPtr;
    MessageBaseSPtr result = downstream;
        // copy the headers
    auto hdrs = resp->headers();
    HttpHeaderFilterSetType dontCopyList{
        Marvin::HeadersV2::Host,
        Marvin::HeadersV2::ProxyConnection,
        Marvin::HeadersV2::Connection,
        Marvin::HeadersV2::TransferEncoding,
        Marvin::HeadersV2::ETag
    };
    HeadersV2::copyExcept(hdrs, result->headers(), dontCopyList);
//    Headers::filterNotInList(hdrs, dontCopyList, [result]( Marvin::Headers& hdrs, std::string k, std::string v)
//    {
//        result->header(k,v);
//    });

    // set the uri and host header
    result->reason(resp->status());
    result->status_code(resp->status_code());
    // no keep alive
    result->header(Marvin::HeadersV2::Connection,  Marvin::HeadersV2::ConnectionClose);
    // Http versions defaults to 1.1, so force it to the same as the request
    result->version_minor(resp->version_minor());
    // now attach the body
    std::size_t len;
    if( (len = responseSPtr->getContentBuffer()->size()) > 0){
        result->setContent(responseSPtr->getContent());
//        resp->header(Marvin::HeadersV2::ContentLength, std::to_string(len));
    }

}

void makeDownstreamErrorResponse(MessageBaseSPtr msg, MessageReaderSPtr resp, Marvin::ErrorType& err)
{
    TROG_DEBUG("");
    // bad gateway 502
    msg->reason("Bad gateway");
    msg->status_code(501);
    msg->header(Marvin::HeadersV2::ContentLength, std::to_string(0));
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
    response->header(Marvin::HeadersV2::Connection, Marvin::HeadersV2::ConnectionClose);
    return false;
}

bool apply_keepalive_rules(MessageReaderSPtr req, MessageBaseSPtr response)
{
    /// correctly handle keep-alive/close
    bool keep_alive;
    if((auto hopt = req->header(Marvin::HeadersV2::Connection))
    && (hopt.get() == Marvin::HeadersV2::ConnectionKeepAlive)) {
        keep_alive = true;
        response->header(Marvin::HeadersV2::Connection, Marvin::HeadersV2::ConnectionKeepAlive);
    } else {
        keep_alive = false;
        response->header(Marvin::HeadersV2::Connection, Marvin::HeadersV2::ConnectionClose);
    }
    return keep_alive;
}

void response403Forbidden(MessageBaseSPtr msg)
{
    msg->reason("Forbidden");
    msg->status_code(403);
    msg->setContent("");
}
void response200OKConnected(MessageBaseSPtr msg)
{
    msg->reason("OK");
    msg->status_code(200);
    msg->setContent("");
}
void response502Badgateway(MessageBaseSPtr msg)
{
    msg->reason("BAD GATEWAY");
    msg->status_code(503);
    msg->setContent("");
}
} // namespace
