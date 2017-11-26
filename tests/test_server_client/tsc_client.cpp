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
#include "request.hpp"

//
// Issue a bunch of simultanious get requests to the test server
// and check that the request gets the expected response
//
std::shared_ptr<Request> connect_request(boost::asio::io_service& io)
{
    LogDebug("");
    std::shared_ptr<Request> req = std::shared_ptr<Request>(new Request(io));
#ifdef VERBOSE
    std::cout << "Create request 1 : " << std::hex << req.get() << std::endl;
#endif
    req->setMethod(HttpMethod::CONNECT);
    req->setUrl("http://localhost:9991");

    std::string b("");
    req->setContent(b);
    req->go([ req ](Marvin::ErrorType& ec){
        LogDebug("");
        MessageReader& resp = req->getResponse();
#ifdef VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
#endif
        LogDebug("");
        //
        // what response should we expect ??
        //
    });

#ifdef VERBOSE
    std::cout << "exit use: " << req.use_count() << std::endl;;
#endif

    return req;
}


//
// Issue a bunch of simultanious get requests to the test server
// and check that the request gets the expected response
//
std::shared_ptr<Request> do_get_request(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Request> req = std::shared_ptr<Request>(new Request(io));

#ifdef VERBOSE
    std::cout << "Create request 1 : " << std::hex << req.get() << std::endl;
#endif

    req->setMethod(HttpMethod::GET);
    req->setUrl("http://localhost:9991?code="+code);

    std::string b("");
    req->setContent(b);
    req->go([ req, code](Marvin::ErrorType& ec){
        MessageReader& resp = req->getResponse();
#ifdef VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
#endif
        std::string t = code+code+code+code+code+code+code+code+code + code;
        bool x = (t == resp.getBody());
        if( x ){ std::cout << "OK";} else {std::cout << "FAIL";};
        if( !x ){
            std::cout << "expecting " << t << std::endl;
            std::cout << "got " << resp.getBody() << std::endl;
        }
        std::cout << std::endl;
        LogDebug("");
    });
#ifdef VERBOSE
    std::cout << "exit use: " << req.use_count() << std::endl;;
#endif
    return req;
}

//
// Issue a bunch of simultanious get requests to the test server
// and check that the request gets the expected response
//
std::shared_ptr<Request> do_post_with_body_request(std::string bodySeed, boost::asio::io_service& io)
{
    std::shared_ptr<Request> req = std::shared_ptr<Request>(new Request(io));

#ifdef VERBOSE
    std::cout << "Create request 1 : " << std::hex << req.get() << std::endl;
#endif

    req->setMethod(HttpMethod::POST);
    req->setUrl("http://localhost:9991");
    
    std::string b = bodySeed;
    for(int i = 0; i < 15; i++) {
        b += bodySeed;
    }
    req->setContent(b);
    req->go([ req, b](Marvin::ErrorType& ec){
        MessageReader& resp = req->getResponse();
#ifdef VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
#endif
        std::string t = b;
        bool x = (t == resp.getBody());
        if( x ){ std::cout << "OK";} else {std::cout << "FAIL";};
        if( !x ){
            std::cout << "expecting " << t << std::endl;
            std::cout << "got " << resp.getBody() << std::endl;
        }
        std::cout << std::endl;
        LogDebug("");
    });
#ifdef VERBOSE
    std::cout << "exit use: " << req.use_count() << std::endl;;
#endif
    return req;
}


void runTestClient()
{
    boost::asio::io_service io_service;
    {
        std::vector<std::shared_ptr<Request>> rt;
//        std::shared_ptr<Request> p1 = do_get_request("1", io_service);

        rt.push_back(connect_request(io_service));
        rt.push_back(do_post_with_body_request("thebodyseed", io_service));
        rt.push_back(do_get_request("1", io_service));
        rt.push_back(do_get_request("A", io_service));
        rt.push_back(do_get_request("B", io_service));
        rt.push_back(do_get_request("C", io_service));
        #if 0
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
}

