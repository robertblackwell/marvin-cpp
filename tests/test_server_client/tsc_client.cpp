//
//  test_client.c
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/13/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include "tsc_client.hpp"
//
//  main.cpp
//  asio-mitm
//
//  Created by ROBERT BLACKWELL on 11/18/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include "bufferV2.hpp"
#include "client.hpp"
#include "tsc_testcase.hpp"
#include "tsc_post.hpp"
#include "tsc_get.hpp"

EchoTestcase  tcase1(
    EchoTestType::WITH_STRING,
    "With string - 2 buffers",
    {
        "1234567890",
        "HGYTRESAWQ"
    }
);
EchoTestcase  tcase2(
    EchoTestType::WITH_MBUFFER,
    "With mbuffer - 2 buffers",
    {
        "1234567890",
        "HGYTRESAWQ"
    }
);
EchoTestcase  tcase3(
    EchoTestType::WITH_BUFFER_CHAIN,
    "With buffer chain - 2 buffers",
    {
        "1234567890",
        "HGYTRESAWQ"
    }
);

#define VERBOSE
#if 0
//
// Issue a bunch of simultanious get requests to the test server
// and check that the request gets the expected response
//
/**
* Perform a single get request
*/
std::shared_ptr<Client> get_testcase(std::string code, boost::asio::io_service& io)
{
     std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, "http://localhost:9991" ));
    
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());

    msg->setMethod(HttpMethod::GET);
    
    std::function<void(Marvin::ErrorType& er, MessageReaderV2SPtr rdr)> f = [client, msg, code](Marvin::ErrorType& ec, MessageReaderV2SPtr rdr) {
#ifdef VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
        MessageReaderV2SPtr b = client->getResponse();
        BufferChain bdy_chain = b->get_body_chain();
        std::string body_as_string = bdy_chain.to_string();
        std::string bdy = body_as_string;
        auto st = b->statusCode();
        assert(b->statusCode() == 200);
        std::cout << "get request code = " << code << " success " << std::endl;
        
#endif
    };
    client->asyncWrite(msg, f);
    return client;
}
#endif

void runTestClient()
{
    boost::asio::io_service io_service;
    {
        RBLogging::setEnabled(false);
        std::vector<std::shared_ptr<Client>> rt;

        rt.push_back(post_body_testcase(tcase1, io_service));
        rt.push_back(post_body_testcase(tcase2, io_service));
        rt.push_back(post_body_testcase(tcase3, io_service));
        rt.push_back(get_testcase("1", io_service));
        #if 0
        rt.push_back(do_get_request("1", io_service));
        rt.push_back(do_get_request("A", io_service));
        rt.push_back(do_get_request("B", io_service));
        rt.push_back(do_get_request("C", io_service));

        rt.push_back(do_get_request("D", io_service));
        rt.push_back(do_get_request("E", io_service));
        rt.push_back(do_get_request("F", io_service));
        rt.push_back(do_get_request("G", io_service));
        rt.push_back(do_get_request("A", io_service));
        rt.push_back(do_get_request("B", io_service));
        rt.push_back(do_get_request("C", io_service));
        rt.push_back(do_get_request("D", io_service));
        rt.push_back(do_get_request("E", io_service));
        rt.push_back(do_get_request("F", io_service));
        rt.push_back(do_get_request("G", io_service));
        #endif
        io_service.run();
        rt.clear();
    }
    std::cout << "after io_service.run() " << std::endl;
    pthread_exit(NULL);
}

