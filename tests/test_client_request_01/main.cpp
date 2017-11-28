//
// This file makes multiple parallel requests to the same host and checks that
// each reply corresponds to the request. That is ensures no "mixing up".
//
// Note the requests go to a local private host that has been programmed
// specifically for this test
//
// Primarily this is a test of the Request object and uses Request in it's
// simplest for - that is sending an entire http message in one hit and with
// no body.
//
// Consider running this test with different Connection management strategies
// to see whether we can save on "async_connect" calls and maybe even get some
// pipe-lining
//
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "boost_stuff.hpp"

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "http_header.hpp"
#include "request.hpp"
#include "client.hpp"

#undef VERBOSE
/*
* Now include test cases
*/
#include "pipeline.cpp"
#include "multiple.cpp"
#include "roundtrip.cpp"


std::shared_ptr<Client> do_client_rt_fragbuffer(std::string code, boost::asio::io_service& io)

{
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, "http://whiteacorn.com/" ));
    
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    
    msg->setIsRequest(true);
    msg->setHttpVersMajor(1);
    msg->setHttpVersMinor(1);
    msg->setUri("/posts/rtw");
    msg->setMethod(HttpMethod::GET);
    
    msg->setHeader(HttpHeader::Name::Host, "whiteacorn.com");
    msg->setHeader(HttpHeader::Name::Connection, "Close");
    msg->setHeader(HttpHeader::Name::ContentLength, "0");

    
    auto f = [client, msg, code](Marvin::ErrorType& ec, MessageReaderV2SPtr rdr) {
#if 1 //VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
        MessageReaderV2SPtr b = client->getResponse();
        std::string bdy = b->getBody();
        std::cout << bdy << std::endl;
        
#endif
    };
    client->asyncWrite(msg, f);

//    req->setMethod(HttpMethod::GET);
//    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);
//
//    std::string b("");
//    req->setContent(b);
//    req->go([ req, code](Marvin::ErrorType& ec){
    return client;

}


int main(int argc, char* argv[]){
    RBLogging::setEnabled(false);
//    testcase_pipeline();

    char* _argv[2] = {argv[0], (char*)"--gtest_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    return RUN_ALL_TESTS();
    
    return 0;
}
