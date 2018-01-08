#include <catch/catch.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

#include "boost_stuff.hpp"
#include "json.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "http_header.hpp"
#include "message.hpp"
#include "UriParser.hpp"
#include "url.hpp"
#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "uri_query.hpp"

#include "tsc_req_handler.hpp"
nlohmann::json jsonize_request(TscRequestHandler* req_handler, ServerContext& server_context, MessageReaderSPtr req)
{
    std::string bodyString = (req->getBody())->to_string();
    nlohmann::json j;
    j["req"] = {
        {"method", req->getMethodAsString()},
        {"uri", req->uri()},
        {"headers", req->getHeaders()},
        {"body",bodyString}
    };
    j["xtra"] = {
        {"connection_handler_uuid", server_context.connection_handler_ptr->uuid()},
        {"request_handler_uuid", boost::uuids::to_string(req_handler->_uuid)},
        {"fd", server_context.connection_ptr->nativeSocketFD()}
    };
    return j;
}
MessageBaseSPtr make_200_response(std::string body)
{
    MessageBaseSPtr msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->setIsRequest(false);
    msg->setStatusCode(200);
    msg->setStatus("OK");
    msg->setHttpVersMajor(1);
    msg->setHttpVersMinor(1);

    Marvin::BufferChainSPtr bchain_sptr = Marvin::BufferChain::makeSPtr(body);
    msg->setHeader(Marvin::Http::Headers::Name::ContentLength, std::to_string(body.length() ));
    return msg;
}
MessageBaseSPtr make_response(int status_code, std::string status, std::string body)
{
    MessageBaseSPtr msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->setIsRequest(false);
    msg->setStatusCode(status_code);
    msg->setStatus(status);
    msg->setHttpVersMajor(1);
    msg->setHttpVersMinor(1);

    Marvin::BufferChainSPtr bchain_sptr = Marvin::BufferChain::makeSPtr(body);
    msg->setHeader(Marvin::Http::Headers::Name::ContentLength, std::to_string(body.length() ));
    return msg;
}


bool apply_connection_close(MessageReaderSPtr req, MessageBaseSPtr response)
{
    response->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);
    return false;
}

bool apply_keepalive_rules(MessageReaderSPtr req, MessageBaseSPtr response)
{
    /// correctly handle keep-alive/close
    bool keep_alive;
    if(req->getHeader(Marvin::Http::Headers::Name::Connection) == Marvin::Http::Headers::Value::ConnectionKeepAlive) {
        keep_alive = true;
        response->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionKeepAlive);
    } else {
        keep_alive = false;
        response->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);
    }
    return keep_alive;
}

int TscRequestHandler::counter;
TscRequestHandler::TscRequestHandler(boost::asio::io_service& io):RequestHandlerBase(io), _timer(io), _uuid(boost::uuids::random_generator()())
{
    counter++;
}
TscRequestHandler::~TscRequestHandler()
{
    LogDebug("");
}

void TscRequestHandler::handleConnect(
    ServerContext&              server_context,
    MessageReaderSPtr           req,
    ISocketSPtr     connPtr,
    HandlerDoneCallbackType     done)
{
    LogDebug("");
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    auto er = Marvin::make_error_ok();
#pragma cland diagnostic pop
    boost::asio::streambuf b;
    std::ostream strm(&b);
    strm << "HTTP/1.1 200 OK\r\nContent-length:5\r\n\r\n12345" << std::endl;
    connPtr->asyncWrite(b, [this, done, &connPtr](Marvin::ErrorType& err, std::size_t bytes_transfered){
        auto er = Marvin::make_error_ok();
        LogDebug(" callback");
        done(er, true);
    });
}
void TscRequestHandler::prepare_send_response(
   MessageWriterSPtr resp,
   MessageBaseSPtr msg,
   std::string body,
   HandlerDoneCallbackType done,
   bool keep_alive
)
{
    _resp = resp;
    _msg = msg;
    _body = body;
    _keep_alive = keep_alive;
    _done = done;
}
void TscRequestHandler::send_response()
{
    _resp->asyncWrite(_msg, _body, [this](Marvin::ErrorType& err){;
        _done(err, keep_alive);
    });
}
void TscRequestHandler::handle_post_invalid(
    ServerContext&   server_context,
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
    std::string body = "";
    MessageBaseSPtr msg = make_response(501, "not implemented", body);
    bool keep_alive = apply_connection_close(req, msg);
    resp->asyncWrite(msg, body, [this, done, keep_alive](Marvin::ErrorType& err){;
        done(err, keep_alive);
    });
}

void TscRequestHandler::handle_post_timeout(
    ServerContext&   server_context,
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
//    nlohmann::json j = jsonize_request(this, server_context, req);
    auto chain = req->getBody();
    std::string bodyString = (req->getBody())->to_string();
    nlohmann::json j = nlohmann::json::parse(bodyString);
    long timeout_interval = j["timeout"].get<int>();

    std::cout << j.dump() << std::endl;

    MessageBaseSPtr msg = make_200_response(j.dump());
    bool keep_alive = apply_keepalive_rules(req, msg);
    std::string json_body = j.dump();
    Marvin::BufferChainSPtr bchain_sptr = Marvin::BufferChain::makeSPtr(json_body);
    prepare_send_response(resp, msg, json_body, done, keep_alive);
    _timer.expires_from_now(boost::posix_time::milliseconds(timeout_interval));
    _timer.async_wait([this](const boost::system::error_code& err) {
        _timer.expires_from_now(boost::posix_time::pos_infin);
        send_response();
    });
}
void TscRequestHandler::handle_post_echo(
    ServerContext&   server_context,
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
    nlohmann::json j = jsonize_request(this, server_context, req);
    MessageBaseSPtr msg = make_200_response(j.dump());
    bool keep_alive = apply_keepalive_rules(req, msg);
    std::string json_body = j.dump();
    Marvin::BufferChainSPtr bchain_sptr = Marvin::BufferChain::makeSPtr(json_body);
    resp->asyncWrite(msg, json_body, [this, done, keep_alive](Marvin::ErrorType& err){;
        done(err, keep_alive);
    });
}
void TscRequestHandler::handle_post(
    ServerContext&   server_context,
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
    std::string uri = req->uri();
    LogDebug("uri:", uri);
    if( uri ==  "/timeout" ) {
        handle_post_timeout(server_context, req, resp, done);
    } else if ((uri == "/echo") || (uri == "/")) {
        handle_post_echo(server_context, req, resp, done);
    } else {
        handle_post_invalid(server_context, req, resp, done);
    }
}
void TscRequestHandler::handle_get(
    ServerContext&   server_context,
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
    std::string uri = req->uri();
    LogDebug("uri:", uri);
    http::url parsed = http::ParseHttpUrl(uri);
    UriQuery query_test(parsed.search);
    std::map<std::string, std::string> parms = query_test.keyValues();
    std::string parm = parms["code"];
    auto f = [](std::string p)
    {
        std::string first = p.substr(0,1);
        std::string res = "";
        for(int i = 0; i < 9; i++) {
            res += first;
        }
        return res;
    };
    std::string body = f(parm);
    MessageBaseSPtr msg = make_200_response(body);
    msg->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);
    resp->asyncWrite(msg, body, [this, done](Marvin::ErrorType& err)
    {
        done(err, false);
    });

}
void TscRequestHandler::handleRequest(
    ServerContext&   server_context,
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    HandlerDoneCallbackType done
)
{
    MessageBaseSPtr msg;
    std::string body;
    bool keep_alive;
    /**
    * Let the sub handlers do their own response as we sometimes want those
    * sub handlers to do other async operations
    */
    switch( req->method() ){
        case HttpMethod::GET :
            handle_get(server_context, req, resp, done);
            break;
        case HttpMethod::POST :
            handle_post(server_context, req, resp, done);
            break;
        default:
            break;
    };
}
