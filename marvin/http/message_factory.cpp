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

    void make_request(MessageBase& msg, HttpMethod method, Uri& uri)
    {
        msg.method(method);
        msg.target(uri.relativePath());
        msg.header(HeadersV2::Host, uri.host_and_port());
    }
    void make_proxy_request(MessageBase& msg, HttpMethod method, Uri& uri)
    {
        msg.method(method);
        msg.target(uri.absolutePath());
        msg.header(HeadersV2::Host, uri.host_and_port());
    }
    void make_proxy_connect_request(MessageBase& msg, std::string server, std::string port)
    {
        using namespace boost;
        
        std::string host_port = trim_left_copy(trim_right_copy(server)) + ":" + trim_left_copy(trim_right_copy(port));
        msg.method(HttpMethod::CONNECT);
        msg.target(host_port);
        msg.header(HeadersV2::Host, host_port);
        msg.set_body(std::string(""));
    }

    void make_response_403_forbidden(MessageBase& msg)
    {
        msg.reason("Forbidden");
        msg.status_code(403);
        std::string n("");
        msg.set_body(n);

    }
    void make_response_200_OK_connected(MessageBase& msg)
    {
        msg.reason("OK");
        msg.status_code(200);
        std::string n("");
        msg.set_body(n);

    }
    void make_response_502_badgateway(MessageBase& msg)
    {
        msg.reason("BAD GATEWAY");
        msg.status_code(503);
        std::string n("");
        msg.set_body(n);

    }

} // namespace Marvin
