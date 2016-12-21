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
void handleFinished1(Request& req, std::string code, Marvin::ErrorType& ec)
{
        MessageReader& resp = req.getResponse();
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << &req << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << &req << std::endl;

        std::string t = code+code+code+code+code+code+code+code+code + code;
        bool x = (t == resp.getBody());
        if( x ){ std::cout << "OK";} else {std::cout << "FAIL";};
        std::cout << std::endl;
        LogDebug("");
}
//
// This is the type callback is OK the best when the continued existence of Request is
// guarenteed by something other than the bind to the callback
//
std::shared_ptr<Request> do_request1(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Request> req = std::shared_ptr<Request>(new Request(io));
    Request& reqRef = *req;
    
    std::cout << "Create request 1 : " << std::hex << req.get() << std::endl;
    req->setMethod(HttpMethod::GET);
    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);

    std::string b("");
    reqRef.setContent(b);
    req->go([ &reqRef, code](Marvin::ErrorType& ec){
        MessageReader& resp = reqRef.getResponse();
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << &reqRef << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << &reqRef << std::endl;

        std::string t = code+code+code+code+code+code+code+code+code + code;
        bool x = (t == resp.getBody());
        if( x ){ std::cout << "OK";} else {std::cout << "FAIL";};
        std::cout << std::endl;
        LogDebug("");
    });
    std::cout << "exit use: " << req.use_count() << std::endl;;
    return std::move(req);
}


std::shared_ptr<Request> do_request2(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Request> req = std::shared_ptr<Request>(new Request(io));
    Request& reqRef = *req;
    
    std::cout << "Create request 1 : " << std::hex << req.get() << std::endl;
    req->setMethod(HttpMethod::GET);
    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);

    std::string b("");
    reqRef.setContent(b);
    req->go([ req, code](Marvin::ErrorType& ec){
        MessageReader& resp = req->getResponse();
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;

        std::string t = code+code+code+code+code+code+code+code+code + code;
        bool x = (t == resp.getBody());
        if( x ){ std::cout << "OK";} else {std::cout << "FAIL";};
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
//        std::shared_ptr<Request> p1 = do_request1("1", io_service);

        rt.push_back(do_request1("1", io_service));
        rt.push_back(do_request1("A", io_service));
        rt.push_back(do_request1("B", io_service));
        rt.push_back(do_request1("C", io_service));
        rt.push_back(do_request1("D", io_service));
        rt.push_back(do_request1("E", io_service));
        rt.push_back(do_request1("F", io_service));
        rt.push_back(do_request1("G", io_service));
        io_service.run();
        rt.clear();
    }
    std::cout << "after io_service.run() " << std::endl;
}

//
// This one fails
//
void testType2CallbackBinding()
{
    boost::asio::io_service io_service;
    {
        std::vector<std::shared_ptr<Request>> rt;
//        std::shared_ptr<Request> p1 = do_request1("1", io_service);

        do_request1("1", io_service);
        do_request1("A", io_service);
        do_request1("B", io_service);
        do_request1("C", io_service);
        do_request1("D", io_service);
        do_request1("E", io_service);
        do_request1("F", io_service);
        
        io_service.run();
        rt.clear();
    }
    std::cout << "after io_service.run() " << std::endl;
}



int main(){
    testType1CallbackBinding();
    return 0;
}