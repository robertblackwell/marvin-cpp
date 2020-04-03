
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

#include <marvin/boost_stuff.hpp>
#include <json/json.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include <marvin/http/headers_v2.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/external_src/uri-parser/UriParser.hpp>
#include <marvin/external_src/CxxUrl/url.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v3/http_server.hpp>
#include <marvin/server_v3/request_handler_base.hpp>

#include "handler.hpp"
#include "handle_app.hpp"

using namespace Marvin;
using namespace Http;
namespace {

bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

} // namespace anon

MessageBaseSPtr make_200_response(std::string body)
{
    MessageBaseSPtr msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->setIsRequest(false);
    msg->setStatusCode(200);
    msg->setStatus("OK");
    msg->setHttpVersMajor(1);
    msg->setHttpVersMinor(1);

    // Marvin::BufferChainSPtr bchain_sptr = Marvin::BufferChain::makeSPtr(body);
    // msg->setHeader(Marvin::Http::HeadersV2::ContentLength, std::to_string(body.length() ));
    msg->setHeader(Marvin::Http::HeadersV2::ContentType, std::string("plain/text"));
    msg->setContent(body);
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
    msg->setHeader(Marvin::Http::HeadersV2::ContentLength, std::to_string(body.length() ));
    return msg;
}
AppHandler::AppHandler(boost::asio::io_service& io): Handler(io)
{

}
AppHandler::~AppHandler()
{
}
void AppHandler::handleRequest()
{
    p_internal_handle();
}
void AppHandler::p_invalid_request()
{
    std::string body = "INVALID REQUEST";
    MessageBaseSPtr response_msg = make_200_response(body);
    auto s = response_msg->str();
    m_wrtr->asyncWrite(response_msg, body, [this](Marvin::ErrorType& err) 
    {
        if (err) {
            p_on_write_error(err);
        } else {
            p_req_resp_cycle_complete();
        }
    });    
}
void AppHandler::p_handle_echo()
{
    std::string body = "THIS IS A RESPONSE BODY";
    MessageBaseSPtr response_msg = make_200_response(body);
    auto s = response_msg->str();
    m_wrtr->asyncWrite(response_msg, body, [this](Marvin::ErrorType& err) 
    {
        if (err) {
            p_on_write_error(err);
        } else {
            p_req_resp_cycle_complete();
        }
    });    
}
void AppHandler::p_handle_smart_echo()
{
    std::string body = "INVALID REQUEST";
    MessageBaseSPtr response_msg = make_200_response(body);
    auto s = response_msg->str();
    m_wrtr->asyncWrite(response_msg, body, [this](Marvin::ErrorType& err) 
    {
        if (err) {
            p_on_write_error(err);
        } else {
            p_req_resp_cycle_complete();
        }
    });    
}
void AppHandler::p_non_specific_response()
{
    std::string body = "THIS IS A RESPONSE BODY";
    MessageBaseSPtr response_msg = make_200_response(body);
    auto s = response_msg->str();
    m_wrtr->asyncWrite(response_msg, body, [this](Marvin::ErrorType& err) 
    {
        if (err) {
            p_on_write_error(err);
        } else {
            p_req_resp_cycle_complete();
        }
    });    
}
void  AppHandler::p_handle_delay(std::vector<std::string>& bits)
{
    int delay;
    if ((bits.size() == 3) && (is_number(bits[2]))) {
        delay = std::stoi(bits[2]);
        m_timer_sptr = std::make_shared<ATimer>(m_io, "Handle Delay");
        m_timer_sptr->arm(delay, [this]()
        {
            p_handle_echo();
        });

    } else {
        p_invalid_request();
    }
}
void AppHandler::p_internal_handle()
{
    m_rdr->readMessage([this](Marvin::ErrorType err)
    {
        if (err) {
            p_on_read_error(err);
        } else {
            std::string path = m_rdr->getPath();
            std::vector<std::string> bits;
            boost::split(bits, path, [](char c){return c == '/';});
            if (bits.size() < 2) {
                bits[1] = "";
            }
            std::string path_01 = bits[1];

            if (path_01 == "echo") {
                p_handle_echo();
            } else if (path_01 == "echosmart") {
                p_handle_smart_echo();
            } else if (path_01 == "delay") {
                p_handle_delay(bits);
            } else {
                p_invalid_request();
            }
        }
    });
}
