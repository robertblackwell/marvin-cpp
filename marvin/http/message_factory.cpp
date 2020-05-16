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
            msg->target(uri.absolutePath());
        else
            msg->target(uri.relativePath());
        msg->header(HeadersV2::Host, uri.host_and_port());
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
        msg.method(method);
        msg.target(uri.relativePath());
        msg.header(HeadersV2::Host, uri.host_and_port());
    }
    void makeProxyRequest(MessageBase& msg, HttpMethod method,  Uri& uri)
    {
        msg.method(method);
        msg.target(uri.absolutePath());
        msg.header(HeadersV2::Host, uri.host_and_port());
    }
    void makeProxyConnectRequest(MessageBase& msg, std::string server, std::string port)
    {
        using namespace boost;
        
        std::string host_port = trim_left_copy(trim_right_copy(server)) + ":" + trim_left_copy(trim_right_copy(port));
        msg.method(HttpMethod::CONNECT);
        msg.target(host_port);
        msg.header(HeadersV2::Host, host_port);
        msg.setContent(std::string(""));
    }

    void makeResponse403Forbidden(MessageBase& msg)
    {
        msg.reason("Forbidden");
        msg.status_code(403);
        std::string n("");
        msg.setContent(n);

    }
    void makeResponse200OKConnected(MessageBase& msg)
    {
        msg.reason("OK");
        msg.status_code(200);
        std::string n("");
        msg.setContent(n);

    }
    void makeResponse502Badgateway(MessageBase& msg)
    {
        msg.reason("BAD GATEWAY");
        msg.status_code(503);
        std::string n("");
        msg.setContent(n);

    }

} // namespace Marvin
