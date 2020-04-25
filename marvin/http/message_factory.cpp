#include <marvin/http/message_factory.hpp>

#include <boost/algorithm/string.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/http/message_factory.hpp>

namespace Marvin {

namespace {
    void applyUri(MessageBaseSPtr msg, Uri& uri, bool proxy)
    {
        if(proxy)
            msg->setUri(uri.absolutePath());
        else
            msg->setUri(uri.relativePath());
        msg->setHeader(HeadersV2::Host, uri.host());
    }
    void applyUriProxy(MessageBaseSPtr msgSPtr, Uri& uri)
    {
        applyUri(msgSPtr, uri, true);
    }
    void applyUriNonProxy(MessageBaseSPtr msgSPtr, Uri& uri)
    {
        applyUri(msgSPtr, uri, false);
    }
} // namespace anonymous

    void makeRequest(MessageBase& msg, HttpMethod method, Uri& uri)
    {
        msg.setMethod(method);
        msg.setUri(uri.relativePath());
        msg.setHeader(HeadersV2::Host, uri.host());
    }
    void makeProxyRequest(MessageBase& msg, HttpMethod method,  Uri& uri)
    {
        msg.setMethod(method);
        msg.setUri(uri.absolutePath());
        msg.setHeader(HeadersV2::Host, uri.host());
    }
    void makeProxyConnectRequest(MessageBase& msg, std::string server, std::string port)
    {
        using namespace boost;
        
        std::string host_port = trim_left_copy(trim_right_copy(server)) + ":" + trim_left_copy(trim_right_copy(port));
        msg.setMethod(HttpMethod::CONNECT);
        msg.setUri(host_port);
        msg.setHeader(HeadersV2::Host, host_port);
        msg.setContent(std::string(""));
    }

    void makeResponse403Forbidden(MessageBase& msg)
    {
        msg.setStatus("Forbidden");
        msg.setStatusCode(403);
        std::string n("");
        msg.setContent(n);

    }
    void makeResponse200OKConnected(MessageBase& msg)
    {
        msg.setStatus("OK");
        msg.setStatusCode(200);
        std::string n("");
        msg.setContent(n);

    }
    void makeResponse502Badgateway(MessageBase& msg)
    {
        msg.setStatus("BAD GATEWAY");
        msg.setStatusCode(503);
        std::string n("");
        msg.setContent(n);

    }

} // namespace Marvin
