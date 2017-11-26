//
//  test_server.c
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/13/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include "tsc_server.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

#include "boost_stuff.hpp"

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "UriParser.hpp"
#include "url.hpp"
#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "uri_query.hpp"

class MyRequestHandler : public RequestHandlerBase
{
public:
    static int counter;
    MyRequestHandler(boost::asio::io_service& io):RequestHandlerBase(io)
    {
        counter++;
    }
    ~MyRequestHandler()
    {
        LogDebug("");
    }
    
    void handleConnect(
        MessageReaderSPtr           req,
        ConnectionInterfaceSPtr     connPtr,
        HandlerDoneCallbackType    done)
    {
        LogDebug("");
        auto er = Marvin::make_error_ok();
        
        boost::asio::streambuf b;
        std::ostream strm(&b);
        strm << "HTTP/1.1 200 OK\r\nContent-length:5\r\n\r\n12345" << std::endl;
        connPtr->asyncWriteStreamBuf(b, [this, done, &connPtr](Marvin::ErrorType& err, std::size_t bytes_transfered){
            auto er = Marvin::make_error_ok();
            LogDebug(" callback");
            done(er, true);
        });
    }

    void pathHandler_A(
        MessageReaderSPtr req,
        MessageWriterSPtr resp,
        HandlerDoneCallbackType done
    ){
    }
    void pathHandler_B(
        MessageReaderSPtr req,
        MessageWriterSPtr resp,
        HandlerDoneCallbackType done
    ){
    }
    void pathHandler_C(
        MessageReaderSPtr req,
        MessageWriterSPtr resp,
        HandlerDoneCallbackType done
    ){
    }
    std::string get_dispatcher(std::string p)
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
    std::string post_dispatcher(MessageReaderSPtr req) {
        return "this was a post";
    }
    
    void handleRequest(
        MessageReaderSPtr req,
        MessageWriterSPtr resp,
        HandlerDoneCallbackType done
    ){
        int local_counter = counter++;
        std::cout << "got to a new handler counter: " << local_counter << std::endl;
        std::ostringstream os;
        std::string uri = req->uri();
        LogDebug("uri:", uri);
        http::url parsed = http::ParseHttpUrl(uri);
        UriQuery query_test(parsed.search);
        std::map<std::string, std::string> parms = query_test.keyValues();
        std::string parm = parms["code"];
        auto meth = req->method();
        auto mm = req->getMethodAsString();
        std::string bodyString("");
        if( mm == "GET") {
            std::string bdy = get_dispatcher(parm);
        } else if(mm == "POST") {
            std::string bdy = post_dispatcher(req);
            bodyString = bdy;
        }
        LogDebug(
            "protoco:", parsed.protocol,
            "host:",parsed.host,
            "port:",parsed.port,
            "path:",parsed.path,
            "search:",parsed.search
        );
        
        if( false) {
        
            os << "<!DOCTYPE html>";
            os << "<html><head></head><body>";
            os << "<p>Method: "     << req->getMethodAsString() << "</p>";
            os << "<p>Vers Maj: "   << req->httpVersMajor() << "</p>";
            os << "<p>Vers Minor: " << req->httpVersMinor() << "</p>";
            os << "<p>uri: "        << req->uri() << "</p>";
            req->dumpHeaders(os);
            for(int i = 0; i < 10; i++ ){
                os << "<p>This is simple a line to fill out the transmission</p>";
            }
            os << "</body>";
        }else{
            os << bodyString;
        }
        resp->setStatusCode(200);
        resp->setStatus("OK");
        resp->setHttpVersMajor(1);
        resp->setHttpVersMinor(1);
        bodyString = os.str();
        resp->setContent(bodyString);
        resp->setHeader("Content-length", std::to_string(bodyString.length() ));
        resp->setHeader("Connection","close");
        resp->asyncWrite([done, local_counter](Marvin::ErrorType& err){;
            std::cout << "handler done local_counter : " << local_counter << std::endl;
            done(err, true);
        });
    }
};
int
MyRequestHandler::counter;

void
runTestServer()
{
    try
    {
        HTTPServer<MyRequestHandler> server;
        server.listen();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }
}

