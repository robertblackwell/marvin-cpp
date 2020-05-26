//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <string>
#include <thread>
#include <doctest/doctest.h>

#include <marvin/http/message_base.hpp>


#include <marvin/http/uri.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/message_factory.hpp>

using namespace Marvin;

TEST_CASE("OK")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    make_response_200_OK_connected(*msg);
    std::string s = msg->to_string();
    CHECK(s == std::string("HTTP/1.1 200 OK\r\nCONTENT-LENGTH: 0\r\n\r\n"));
}
TEST_CASE("403")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    make_response_403_forbidden(*msg);
    std::string s = msg->to_string();
    CHECK(s == std::string("HTTP/1.1 403 Forbidden\r\nCONTENT-LENGTH: 0\r\n\r\n"));
}
TEST_CASE("502")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    make_response_502_badgateway(*msg);
    std::string s = msg->to_string();
    CHECK(s == std::string("HTTP/1.1 503 BAD GATEWAY\r\nCONTENT-LENGTH: 0\r\n\r\n"));
}

TEST_CASE("GetRequest")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    Uri uri("http://www.somewhere:77?a=1111&b=2222");
    make_request(*msg, HttpMethod::GET, uri);
    std::string s = msg->to_string();
    CHECK(s == std::string("GET /?a=1111&b=2222 HTTP/1.1\r\nHOST: www.somewhere:77\r\n\r\n"));
}
TEST_CASE("GetRequest:80")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    Uri uri("http://www.somewhere?a=1111&b=2222");
    make_request(*msg, HttpMethod::GET, uri);
    std::string s = msg->to_string();
    std::string expected = std::string("GET /?a=1111&b=2222 HTTP/1.1\r\nHOST: www.somewhere:80\r\n\r\n");
    bool t = (s == expected); 
    CHECK(s == expected);

}
TEST_CASE("GetRequest:443")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    MessageBaseSPtr msgv2 = std::make_shared<MessageBase>();
    Uri uri("https://www.somewhere?a=1111&b=2222");
    make_request(*msg, HttpMethod::GET, uri);

    std::string s = msg->to_string();
    std::string expected = std::string("GET /?a=1111&b=2222 HTTP/1.1\r\nHOST: www.somewhere:443\r\n\r\n");
    bool t = (s == expected);
    CHECK(t);
    // CHECK(s == std::string("GET /?a=1111&b=2222 HTTP/1.1\r\nHOST: www.somewhere:443\r\n\r\n"));
}

TEST_CASE("ProxyGetRequest")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    Uri uri("http://www.somewhere:77?a=1111&b=2222");
    make_proxy_request(*msg, HttpMethod::GET, uri);
    std::string s = msg->to_string();
    CHECK(s == std::string("GET http://www.somewhere:77/?a=1111&b=2222 HTTP/1.1\r\nHOST: www.somewhere:77\r\n\r\n"));
}
