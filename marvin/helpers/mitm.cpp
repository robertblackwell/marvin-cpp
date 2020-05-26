#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/helpers/mitm.hpp>
#include <boost/algorithm/string/trim.hpp>

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
    msg->header(Marvin::HeaderFields::Host, uri.host_and_port());
}
void apply_uri_proxy(MessageBaseSPtr msgSPtr, Uri& uri)
{
    applyUri(msgSPtr, uri, true);
}
void apply_uri_non_proxy(MessageBaseSPtr msgSPtr, Uri& uri)
{
    applyUri(msgSPtr, uri, false);
}

void remove_hop_by_hop(MessageBaseSPtr msgSPtr, std::string connectionValue)
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
//
// Elements that are common between http and https in the transforming of a client
// request into a form to send upstream to the end server.
//
void request_transform_common(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr)
{
    MessageReaderSPtr req = requestSPtr;
    MessageBaseSPtr result = upstreamRequest;

    // copy the headers
    auto hdrs = req->headers();
    std::set<std::string> dontCopyList{
        Marvin::HeaderFields::Host,
        Marvin::HeaderFields::ProxyConnection,
        Marvin::HeaderFields::ProxyAuthorization,
        Marvin::HeaderFields::ProxyAuthentication,
        Marvin::HeaderFields::Connection,
        Marvin::HeaderFields::ETag,
        Marvin::HeaderFields::TransferEncoding,
        Marvin::HeaderFields::TE,
        Marvin::HeaderFields::Trailer,
        Marvin::HeaderFields::Upgrade
    };
    // copy all headers except those in dontCopyList
    HeaderFields::copy_except(hdrs, result->headers(), dontCopyList);

    if (auto hdropt = req->header(Marvin::HeaderFields::Connection)) {
        std::string cv = hdropt.get();
        remove_hop_by_hop(result, cv);
    }

    // no keep alive
    result->header(Marvin::HeaderFields::Connection, Marvin::HeaderFields::ConnectionClose);
    // no compression
    result->header(Marvin::HeaderFields::AcceptEncoding, "identity");
    result->header(Marvin::HeaderFields::TE, "");
    // Http versions defaults to 1.1, so force it to the same as the request
    result->version_minor(req->version_minor());
    result->set_body(req->get_body_buffer_chain());
//    result->header(Marvin::HeaderFields::ContentLength, std::to_string(req->getBody()->size()));

}
void make_upstream_request(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr)
{
    MessageReaderSPtr req = requestSPtr;
    MessageBaseSPtr result = upstreamRequest;
    
    Marvin::Uri tmp_uri(req->target());
    
    /// this call sets the target and host field in the upstream request
    apply_uri_non_proxy(upstreamRequest, tmp_uri);

    // filter out upgrade requests
    assert( ! req->header("Upgrade") );
    
    // set the method
    result->method(req->method());

    request_transform_common(result, req);
}
// https is a little different because when in mitm mode the client thinks
// they have a direct connection rather than through proxy and hence the
// request will be different to that of a http request
// the target field will be a relative path not an absolue path
void make_upstream_https_request(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr)
{
    MessageReaderSPtr req = requestSPtr;
    MessageBaseSPtr result = upstreamRequest;
    
    result->target(req->target());
    result->method(req->method());
    auto hdropt = req->header(HeaderFields::Host);
    if (!hdropt) {
        TROG_ERROR("no host header in request");
    }
    result->header(HeaderFields::Host, req->header(HeaderFields::Host).get());
    request_transform_common(result, req);
}

void make_downstream_good_response(MessageBaseSPtr downstream, MessageReaderSPtr responseSPtr )
{
    TROG_INFO("");
    MessageReaderSPtr resp = responseSPtr;
    MessageBaseSPtr result = downstream;
        // copy the headers
    auto hdrs = resp->headers();
    HttpHeaderFilterSetType dontCopyList{
        Marvin::HeaderFields::Host,
        Marvin::HeaderFields::ProxyConnection,
        Marvin::HeaderFields::Connection,
        Marvin::HeaderFields::TransferEncoding,
        Marvin::HeaderFields::ETag
    };
    HeaderFields::copy_except(hdrs, result->headers(), dontCopyList);
//    Headers::filterNotInList(hdrs, dontCopyList, [result]( Marvin::Headers& hdrs, std::string k, std::string v)
//    {
//        result->header(k,v);
//    });

    // set the uri and host header
    result->reason(resp->reason());
    result->status_code(resp->status_code());
    // no keep alive
    result->header(Marvin::HeaderFields::Connection, Marvin::HeaderFields::ConnectionClose);
    // Http versions defaults to 1.1, so force it to the same as the request
    result->version_minor(resp->version_minor());
    // now attach the body
    std::size_t len;
    if((len = responseSPtr->get_body_buffer_chain()->size()) > 0){
        result->set_body(responseSPtr->get_body());
//        resp->header(Marvin::HeaderFields::ContentLength, std::to_string(len));
    }

}

void make_downstream_error_response(MessageBaseSPtr msg, MessageReaderSPtr resp, Marvin::ErrorType& err)
{
    TROG_DEBUG("");
    // bad gateway 502
    msg->reason("Bad gateway");
    msg->status_code(501);
    msg->header(Marvin::HeaderFields::ContentLength, std::to_string(0));
    std::string n("");
}
void make_downstream_response(MessageBaseSPtr msg_sptr, MessageReaderSPtr resp, Marvin::ErrorType& err)
{
    if( err ){
        make_downstream_error_response(msg_sptr, resp, err);
       TROG_TRACE3(Marvin::make_error_description(err));
    }else{
        make_downstream_good_response(msg_sptr, resp);
//        make_downstream_response();
    }
}

bool apply_connection_close(MessageReaderSPtr req, MessageBaseSPtr response)
{
    response->header(Marvin::HeaderFields::Connection, Marvin::HeaderFields::ConnectionClose);
    return false;
}

bool apply_keepalive_rules(MessageReaderSPtr req, MessageBaseSPtr response)
{
    /// correctly handle keep-alive/close
    bool keep_alive;
    boost::optional<std::string> hopt;
    
    if((hopt = req->header(Marvin::HeaderFields::Connection))
        && (hopt.get() == Marvin::HeaderFields::ConnectionKeepAlive)) {
        keep_alive = true;
        response->header(Marvin::HeaderFields::Connection, Marvin::HeaderFields::ConnectionKeepAlive);
    } else {
        keep_alive = false;
        response->header(Marvin::HeaderFields::Connection, Marvin::HeaderFields::ConnectionClose);
    }
    return keep_alive;
}

void response403Forbidden(MessageBaseSPtr msg)
{
    msg->reason("Forbidden");
    msg->status_code(403);
    msg->set_body("");
}
void response200OKConnected(MessageBaseSPtr msg)
{
    msg->reason("OK");
    msg->status_code(200);
    msg->set_body("");
}
void response502Badgateway(MessageBaseSPtr msg)
{
    msg->reason("BAD GATEWAY");
    msg->status_code(503);
    msg->set_body("");
}
} // namespace
