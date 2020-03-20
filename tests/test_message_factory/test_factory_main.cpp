//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/connection/tcp_connection.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/client/client.hpp>
#include <marvin/forwarding//forward_helpers.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include <marvin/http/uri.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/message_factory.hpp>

using namespace Marvin;
using namespace Marvin::Http;

TEST_CASE("OK")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    makeResponse200OKConnected(*msg);
    std::string s = msg->str();
    REQUIRE(s == std::string("HTTP/1.1 200 OK\r\nCONTENT-LENGTH: 0\r\n\r\n"));
}
TEST_CASE("403")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    makeResponse403Forbidden(*msg);
    std::string s = msg->str();
    REQUIRE(s == std::string("HTTP/1.1 403 Forbidden\r\nCONTENT-LENGTH: 0\r\n\r\n"));
}
TEST_CASE("502")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    makeResponse502Badgateway(*msg);
    std::string s = msg->str();
    REQUIRE(s == std::string("HTTP/1.1 503 BAD GATEWAY\r\nCONTENT-LENGTH: 0\r\n\r\n"));
}

TEST_CASE("GetRequest")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    Uri uri("http://www.somewhere:77?a=1111&b=2222");
    makeRequest(*msg, HttpMethod::GET, uri);
    std::string s = msg->str();
    REQUIRE(s == std::string("GET /?a=1111&b=2222 HTTP/1.1\r\nHOST: www.somewhere:77\r\n\r\n"));
}
TEST_CASE("GetRequest:80")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    Uri uri("http://www.somewhere?a=1111&b=2222");
    makeRequest(*msg, HttpMethod::GET, uri);
    std::string s = msg->str();
    REQUIRE(s == std::string("GET /?a=1111&b=2222 HTTP/1.1\r\nHOST: www.somewhere\r\n\r\n"));
}
TEST_CASE("GetRequest:443")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    Uri uri("https://www.somewhere?a=1111&b=2222");
    makeRequest(*msg, HttpMethod::GET, uri);
    std::string s = msg->str();
    REQUIRE(s == std::string("GET /?a=1111&b=2222 HTTP/1.1\r\nHOST: www.somewhere\r\n\r\n"));
}

TEST_CASE("ProxyGetRequest")
{
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    Uri uri("http://www.somewhere:77?a=1111&b=2222");
    makeProxyRequest(*msg, HttpMethod::GET, uri);
    std::string s = msg->str();
    REQUIRE(s == std::string("GET http://www.somewhere:77/?a=1111&b=2222 HTTP/1.1\r\nHOST: www.somewhere:77\r\n\r\n"));
}
