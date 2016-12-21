//
//  test_server.c
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/13/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include "test_server.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include <pthread.h>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "UriParser.hpp"
#include "url.hpp"
#include "server.hpp"
#include "request_handler_interface.hpp"
#include "request.hpp"
#include "uri_query.hpp"

class MyHandler : public RequestHandlerInterface
{
public:
    static int counter;
    MyHandler()
    {
        counter++;
    }
    ~MyHandler()
    {
        LogDebug("");
    }
    
    void handleConnect(
        boost::asio::io_service& io,
        MessageReader& req,
        ConnectionPtr  connPtr,
        HandlerDoneCallbackType hijack_connection)
    {
        LogDebug("");
        hijack_connection(true);
        
        boost::asio::streambuf b;
        std::ostream strm(&b);
        strm << "HTTP/1.1 200 OK\r\nContent-length:5\r\n\r\n12345" << std::endl;
        connPtr->asyncWriteStreamBuf(b, [this, connPtr](Marvin::ErrorType& err, std::size_t bytes_transfered){
            LogDebug(" callback");
        });
    }

    void pathHandler_A(
        boost::asio::io_service& io,
        MessageReader& req,
        MessageWriter& resp,
        HandlerDoneCallbackType done
    ){
    }
    void pathHandler_B(
        boost::asio::io_service& io,
        MessageReader& req,
        MessageWriter& resp,
        HandlerDoneCallbackType done
    ){
    }
    void pathHandler_C(
        boost::asio::io_service& io,
        MessageReader& req,
        MessageWriter& resp,
        HandlerDoneCallbackType done
    ){
    }
    std::string dispatcher(std::string p)
    {
        if( p == "A"){
            return "AAAAAAAAA";
        }else if( p == "B" ){
            return "BBBBBBBBBB";
        }else if( p == "C" ){
            return "CCCCCCCCCC";
        }else if( p == "D" ){
            return "DDDDDDDDDD";
        }else if( p == "E" ){
            return "EEEEEEEEEE";
        }else if( p == "F" ){
            return "FFFFFFFFFF";
        }else if( p == "G" ){
            return "GGGGGGGGGG";
        }else if( p == "1" ){
            return "1111111111";
        }else{
            return "ERROR WRONG REQUEST";
        }
    }
    void handleRequest(
        boost::asio::io_service& io,
        MessageReader& req,
        MessageWriter& resp,
        HandlerDoneCallbackType done
    ){
        int local_counter = counter++;
        std::cout << "got to a new handler counter: " << local_counter << std::endl;
        std::ostringstream os;
        std::string uri = req.uri();
        LogDebug("uri:", uri);
        http::url parsed = http::ParseHttpUrl(uri);
        UriQuery query_test(parsed.search);
        std::map<std::string, std::string> parms = query_test.keyValues();
        std::string parm = parms["code"];
        std::string bdy = dispatcher(parm);
        
        LogDebug(
            "protoco:", parsed.protocol,
            "host:",parsed.host,
            "port:",parsed.port,
            "path:",parsed.path,
            "search:",parsed.search
        );
        
        std::string bodyString("");
        if( false) {
        
            os << "<!DOCTYPE html>";
            os << "<html><head></head><body>";
            os << "<p>Method: "     << req.getMethodAsString() << "</p>";
            os << "<p>Vers Maj: "   << req.httpVersMajor() << "</p>";
            os << "<p>Vers Minor: " << req.httpVersMinor() << "</p>";
            os << "<p>uri: "        << req.uri() << "</p>";
            req.dumpHeaders(os);
            for(int i = 0; i < 10; i++ ){
                os << "<p>This is simple a line to fill out the transmission</p>";
            }
            os << "</body>";
        }else{
            os << bdy;
        }
        resp.setStatusCode(200);
        resp.setStatus("OK");
        resp.setHttpVersMajor(1);
        resp.setHttpVersMinor(1);
        bodyString = os.str();
        resp.setContent(bodyString);
        resp.setHeader("Content-length", std::to_string(bodyString.length() ));
        resp.setHeader("Connection","close");
        resp.asyncWrite([done, local_counter](Marvin::ErrorType& err){;
            std::cout << "handler done local_counter : " << local_counter << std::endl;
            done(true);
        });
    }
};
int
MyHandler::counter;

void
runTestServer()
{
    try
    {
        Server<MyHandler> server;
        server.listen();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }
}

