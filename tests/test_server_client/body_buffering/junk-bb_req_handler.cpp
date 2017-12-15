#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include "json.hpp"
#include "boost_stuff.hpp"

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "http_header.hpp"
#include "UriParser.hpp"
#include "url.hpp"
#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "uri_query.hpp"

#include "bb_req_handler.hpp"

using namespace body_buffering;

int RequestHandler::counter;
RequestHandler::RequestHandler(boost::asio::io_service& io):RequestHandlerBase(io), _timer(io), _uuid(boost::uuids::random_generator()())
{
    counter++;
}
RequestHandler::~RequestHandler()
{
    LogDebug("");
}

void RequestHandler::handleConnect(
    ServerContext&   server_context,
    MessageReaderSPtr           req,
    ConnectionInterfaceSPtr     connPtr,
    HandlerDoneCallbackType    done)
{
    LogDebug("");
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    auto er = Marvin::make_error_ok();
#pragma clang diagnostic pop

    boost::asio::streambuf b;
    std::ostream strm(&b);
    strm << "HTTP/1.1 200 OK\r\nContent-length:5\r\n\r\n12345" << std::endl;
    connPtr->asyncWrite(b, [this, done, &connPtr](Marvin::ErrorType& err, std::size_t bytes_transfered){
        auto er = Marvin::make_error_ok();
        LogDebug(" callback");
        done(er, true);
    });
}

void RequestHandler::pathHandler_A(
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
}
void RequestHandler::pathHandler_B(
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
}
void RequestHandler::pathHandler_C(
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
}

std::string RequestHandler::get_dispatcher(std::string p)
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

std::string RequestHandler::post_dispatcher(MessageReaderSPtr req)
{
    return "this was a post";
}
    
void RequestHandler::handleRequest(
    ServerContext&   server_context,
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

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
        bodyString = (req->get_body_chain()).to_string();
    }

    MessageBaseSPtr msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->setIsRequest(false);
    msg->setStatusCode(200);
    msg->setStatus("OK");
    msg->setHttpVersMajor(1);
    msg->setHttpVersMinor(1);

    BufferChainSPtr bchain_sptr = buffer_chain(bodyString);
    msg->setHeader(HttpHeader::Name::ContentLength, std::to_string(bodyString.length() ));
    
    /// correctly handle keep-alive/close
    bool keep_alive;
    if(req->getHeader(HttpHeader::Name::Connection) == HttpHeader::Value::ConnectionKeepAlive) {
        keep_alive = true;
        msg->setHeader(HttpHeader::Name::Connection, HttpHeader::Value::ConnectionKeepAlive);
    } else {
        keep_alive = false;
        msg->setHeader(HttpHeader::Name::Connection, HttpHeader::Value::ConnectionClose);
    }
#pragma clang diagnostic pop

    resp->asyncWrite(msg, bchain_sptr, [this, done, keep_alive](Marvin::ErrorType& err){;
        done(err, keep_alive);
    });
}
