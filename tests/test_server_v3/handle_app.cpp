#include "handle_app.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

#include <json/json.hpp>
#include <marvin/boost_stuff.hpp>


#include <marvin/http/headers_v2.hpp>
#include <marvin/http/message_base.hpp>
#include<uri-parser/UriParser.hpp>
#include<CxxUrl/url.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/request_handler_interface.hpp>

using namespace Marvin;

namespace {

bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

} // namespace anon

MessageBaseSPtr make_200_response(std::string body)
{
    MessageBaseSPtr msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->set_is_request(false);
    msg->status_code(200);
    msg->reason("OK");
    msg->version(1, 1);

    // BufferChain::SPtr bchain_sptr = makeBufferChainSPtr(body);
    // msg->header(HeaderFields::ContentLength, std::to_string(body.length() ));
    msg->header(HeaderFields::ContentType, std::string("plain/text"));
    msg->set_body(body);
    return msg;
}
MessageBaseSPtr make_response(int status_code, std::string status, std::string body)
{
    MessageBaseSPtr msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->set_is_request(false);
    msg->status_code(status_code);
    msg->reason(status);
    msg->version(1, 1);

    BufferChain::SPtr bchain_sptr = makeBufferChainSPtr(body);
    msg->header(HeaderFields::ContentLength, std::to_string(body.length() ));
    return msg;
}
AppHandler::AppHandler(boost::asio::io_service& io): m_io(io)
{

}
AppHandler::~AppHandler()
{
}

void AppHandler::handle(
    ServerContext&          server_context,
    ISocketSPtr                     socket_sptr,
    HandlerDoneCallbackType done
)
{
    m_socket_sptr = socket_sptr;
    m_rdr = std::make_shared<MessageReader>(socket_sptr);
    m_wrtr = std::make_shared<MessageWriter>(socket_sptr);
    m_done_callback = done;
    // Adapterequest(m_socket_sptr, m_wrtr, m_rdr);
    p_internal_handle();
}
void AppHandler::p_internal_handle()
{
    m_rdr->async_read_message([this](ErrorType err)
                              {
                                  if (err) {
                                      p_on_read_error(err);
                                  } else {
                                      std::string path = m_rdr->target();
                                      std::vector<std::string> bits;
                                      boost::split(bits, path, [](char c) { return c == '/'; });
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

void AppHandler::p_on_completed()
{
    p_req_resp_cycle_complete();
}
/// determine whether to callback to the server or start another read/write cycle
void AppHandler::p_req_resp_cycle_complete()
{
    bool keep_alive = false;
    /// @TODO - this is a hack
    auto hopt = m_rdr->header(HeaderFields::Connection);
    if (hopt) {
        std::string conhdr = hopt.get();
        keep_alive = (conhdr == "Keep-Alive");
    }
    if (keep_alive) {
        // Adapterequest(m_socket_sptr, m_wrtr, m_rdr);
        p_internal_handle();
    } else {
        m_socket_sptr->shutdown(ISocket::ShutdownSend); // remember this is actually shutdown send side
        m_done_callback();
    }
    // m_done_callback();
}
void AppHandler::p_on_read_error(ErrorType err)
{
    // m_socket_sptr->close();
    m_done_callback();
}
void AppHandler::p_on_write_error(ErrorType err)
{
    // m_socket_sptr->close();
    m_done_callback();
}



void AppHandler::p_invalid_request()
{
    std::string body = "INVALID REQUEST";
    MessageBaseSPtr response_msg = make_200_response(body);
    auto s = response_msg->to_string();
    m_wrtr->async_write(response_msg, body, [this](ErrorType& err) 
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
    auto s = response_msg->to_string();
    m_wrtr->async_write(response_msg, body, [this](ErrorType& err) 
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
    auto s = response_msg->to_string();
    m_wrtr->async_write(response_msg, body, [this](ErrorType& err) 
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
    auto s = response_msg->to_string();
    m_wrtr->async_write(response_msg, body, [this](ErrorType& err) 
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
