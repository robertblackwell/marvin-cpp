#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/forwarding/forward_helpers.hpp>
#include <doctest/doctest.h>
#include "roundtrip.hpp"

using namespace Marvin;
using namespace Marvin::Http;
#if 1
std::shared_ptr<Client> do_client_connect(std::string code, boost::asio::io_service& io)
{
    Marvin::Uri uri("http://whiteacorn.com/utests/echo/test.php");
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, uri));
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    helpers::applyUriProxy(msg, uri);
    client->asyncConnect([client, msg, code](Marvin::ErrorType& ec){
#if 1
        REQUIRE(!ec);
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
#endif
    });
    return client;

}

std::shared_ptr<Client> one_roundtrip(std::string code, boost::asio::io_service& io)
{
    Marvin::Uri uri("http://whiteacorn.com/utests/echo/");
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, uri ));
    
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    
    msg->method(HttpMethod::GET);
    helpers::applyUriProxy(msg, uri);
    msg->header(Marvin::Http::HeadersV2::Connection, Marvin::Http::HeadersV2::ConnectionClose);
    msg->header(Marvin::Http::HeadersV2::AcceptEncoding, "identity");
    msg->header(Marvin::Http::HeadersV2::TE, "");
    msg->setContent("");

    std::function<void(Marvin::ErrorType& er, MessageReaderSPtr rdr)> f = [client, msg, code](Marvin::ErrorType& ec, MessageReaderSPtr rdr) {
#if 1
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
#endif
        if (ec) {
            REQUIRE(false);
        } else {
            MessageReaderSPtr b = client->getResponse();
            auto st = b->status_code();
            REQUIRE(b->status_code() == 200);
            // maybe CHECK some other stuff here ?
        }
       
    };
    client->async_write(msg, f);
    return client;
}
/// makes a GET request to the url provided and expects status=200/302 and a non empty content
std::shared_ptr<Client> general_roundtrip(boost::asio::io_service& io, std::string request_url)
{
    Marvin::Uri uri(request_url);
    std::string sch = uri.scheme();
    boost::algorithm::to_lower(sch);
    if (sch != "http") {
        throw "Currently can only handle non secure requests";
    }
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, uri ));
    
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    
    msg->method(HttpMethod::GET);
    helpers::applyUriProxy(msg, uri);
    msg->header(Marvin::Http::HeadersV2::Connection, Marvin::Http::HeadersV2::ConnectionClose);
    msg->header(Marvin::Http::HeadersV2::AcceptEncoding, "identity");
    msg->header(Marvin::Http::HeadersV2::TE, "");
    // Http versions defaults to 1.1, so force it to the same as the request
    msg->setContent("");

    std::function<void(Marvin::ErrorType& er, MessageReaderSPtr rdr)> f = [client, msg, request_url, &io](Marvin::ErrorType& ec, MessageReaderSPtr rdr) {
        MessageReaderSPtr b = client->getResponse();
        std::string bdy = (b->getContent())->to_string();
        if (b->status_code() != 200) {
            std::cout << "Request URL : " << request_url << " StatusCode: " << b->status_code() << std::endl;
        }
        CHECK((b->status_code() == 200 || b->status_code() == 302));
        CHECK( bdy.size() > 0);
        io.stop();
        
    };
    client->async_write(msg, f);
    return client;
}
#ifdef WHEN_WE_CAN_HANDLE_HTTPS_TRAFFIC
TEST_CASE("ssl_ssllabs")
{
    boost::asio::io_service io_service;
    std::vector<std::shared_ptr<Client>> rt;
    auto c = general_roundtrip(io_service, "https://www.ssllabs.com"); /*//https://www.ssllabs.com/");*/
    io_service.run();
    std::cout << "Return from io " << std::endl;
    rt.clear();
}
TEST_CASE("ssl_ssltest")
{
    boost::asio::io_service io_service;
    std::vector<std::shared_ptr<Client>> rt;
    auto c = general_roundtrip(io_service, "https://paypal.com/");
    io_service.run();
    rt.clear();

}
#endif
TEST_CASE("ClientRoundTrip-SixTimes")
{
    boost::asio::io_service io_service;
    std::vector<std::shared_ptr<Client>> rt;
    rt.push_back(one_roundtrip("1", io_service));
#if 0
    rt.push_back(one_roundtrip("A", io_service));
    rt.push_back(one_roundtrip("B", io_service));
    rt.push_back(one_roundtrip("C", io_service));
    rt.push_back(one_roundtrip("D", io_service));
    rt.push_back(one_roundtrip("E", io_service));
#endif
    io_service.run();
    rt.clear();
}
#endif

