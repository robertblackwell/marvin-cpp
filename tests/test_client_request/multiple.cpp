#include <marvin/forwarding/forward_helpers.hpp>
#include <doctest/doctest.h>
#include "multiple.hpp"
Trog_SETLEVEL(LOG_LEVEL_WARN)

using namespace Marvin;
using namespace Marvin::Http;

/**
* This file tests mutiple simultanious requests to by different client objects
* to ensure isloation between instances
*/


/**
* Perform a single get request
*/
std::shared_ptr<Client> do_get_request(std::string code, boost::asio::io_service& io)
{
    Marvin::Uri uri("http://whiteacorn.com/posts/rtw");
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, uri));
    
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    
    msg->setMethod(HttpMethod::GET);
    helpers::applyUriNonProxy(msg, uri);
    msg->setHeader(Marvin::Http::HeadersV2::Connection, Marvin::Http::HeadersV2::ConnectionClose);
    msg->setContent("");

    std::function<void(Marvin::ErrorType& er, MessageReaderSPtr rdr)> f = [client, msg, code](Marvin::ErrorType& ec, MessageReaderSPtr rdr) {
#ifdef VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
        
#endif
        MessageReaderSPtr b = client->getResponse();
        std::string bdy = b->getContent()->to_string();
        auto st = b->statusCode();
        REQUIRE(b->statusCode() == 200);
    };
    client->asyncWrite(msg, f);
    return client;
}
#if 1
TEST_CASE("Request_MultipleConsecutiveTimes")
{
    boost::asio::io_service io_service;
    {
        std::vector<std::shared_ptr<Client>> rt;
        rt.push_back(do_get_request("1", io_service));
        rt.push_back(do_get_request("A", io_service));
        rt.push_back(do_get_request("B", io_service));
        rt.push_back(do_get_request("C", io_service));
        rt.push_back(do_get_request("D", io_service));
        rt.push_back(do_get_request("E", io_service));
        rt.push_back(do_get_request("F", io_service));
        rt.push_back(do_get_request("G", io_service));
        io_service.run();
        rt.clear();
    }
}
#endif
