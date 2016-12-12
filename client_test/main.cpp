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
#include "ConnectionManager.hpp"

void getOne(Connection* c){
    ReadSocketInterface* rs = c;
}

class UseOne{
public:
    UseOne(boost::asio::io_service& io): _io(io){}
    
    void getOneGiven(Connection* conn){
        rsock = conn;
    }
    void doSomething(){
        rsock = new Connection(_io, "", "", "");
    }
    ReadSocketInterface* rsock;
    boost::asio::io_service& _io;
};
class DoRequest
{
    public:
    DoRequest(boost::asio::io_service& io): _io(io), req(io)
    {
    }
    void do_request(){
    }
    
    Request req;
    boost::asio::io_service& _io;
    
};

Request* do_request1(std::string code, boost::asio::io_service& io)
{
    Request* req = new Request(io);
    std::cout << "Create request 1 : " << std::hex << req << std::endl;
    req->setMethod(HttpMethod::GET);
    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);
    std::string b("");
    req->setContent(b);
    req->go([req, code](Marvin::ErrorType& ec){
        MessageReader& resp = req->getResponse();
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << req << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << req << std::endl;
        
        std::string t = code+code+code+code+code+code+code+code+code + code;
        bool x = (t == resp.getBody());
        if( x ){ std::cout << "OK";} else {std::cout << "FAIL";};
        std::cout << std::endl;
        LogDebug("");
    });
    return req;
}


int main()
{
    std::vector<Request*> rt;
    boost::asio::io_service io_service;
    rt.push_back(do_request1("1", io_service));
    rt.push_back(do_request1("A", io_service));
    rt.push_back(do_request1("B", io_service));
    rt.push_back(do_request1("C", io_service));
    rt.push_back(do_request1("D", io_service));
    rt.push_back(do_request1("E", io_service));
    rt.push_back(do_request1("F", io_service));
    rt.push_back(do_request1("G", io_service));
    io_service.run();
    std::cout << "after io_service.run() " << std::endl;
    return 0;
}