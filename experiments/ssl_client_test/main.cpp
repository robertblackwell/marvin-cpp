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
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "request.hpp"


//
// This is the type callback is OK the best when the continued existence of Request is
// guarenteed by something other than the bind to the callback
//
std::shared_ptr<Request> do_request1(std::string url_in, boost::asio::io_service& io)
{
    std::shared_ptr<Request> req = std::shared_ptr<Request>(new Request(io));
    Request& reqRef = *req;
    
    std::cout << "Create request 1 : " << std::hex << req.get() << std::dec << std::endl;
    req->setMethod(HttpMethod::GET);
    req->setUrl(url_in);
    std::string b("");
    
    reqRef.setContent(b);
    req->go([ &reqRef](Marvin::ErrorType& ec){
        MessageReader& resp = reqRef.getResponse();
        std::cout << std::dec << "request Error" << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request status: " << resp.status() << " statusCode: " << resp.statusCode()  << std::endl;
        resp.dumpHeaders(std::cout);
        std::cout << std::endl;
        std::cout << "request body:" << resp.getBody() << std::endl;
        std::cout << "Outcome: " << ((resp.statusCode() == 200 )? "SUCCESS" : "FAIL") << std::endl;
        std::cout << std::endl;
        LogDebug("");
    });
    std::cout << "exit use: " << req.use_count() << std::endl;;
    return std::move(req);
}


void testType1CallbackBinding()
{
    boost::asio::io_service io_service;
    {
        std::vector<std::shared_ptr<Request>> rt;

        rt.push_back(do_request1("https://httpbin.org/", io_service));
        rt.push_back(do_request1("https://httpbin.org/ip", io_service));
        rt.push_back(do_request1("https://httpbin.org/user-agent", io_service));
        rt.push_back(do_request1("https://httpbin.org/headers", io_service));
        rt.push_back(do_request1("https://httpbin.org/image", io_service));

        io_service.run();
        rt.clear();
    }
    std::cout << "after io_service.run() " << std::endl;
}



int main(){
    testType1CallbackBinding();
    return 0;
}