#ifndef marvin_test_server_v3_app_handler_hpp
#define marvin_test_server_v3_app_handler_hpp

#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v3/http_server.hpp>
#include <marvin/server_v3/server_context.hpp>
#include <marvin/server_v3/request_handler_base.hpp>
#include "handler.hpp"
#include "timer.hpp"
class AppHandler : public Handler
{
public:
    AppHandler(boost::asio::io_service& io);
    ~AppHandler();
    
    void handleRequest(
        // ISocketSPtr         connPtr,
        // MessageWriterSPtr   m_wrtr,
        // MessageReaderSPtr   m_rdr
    );
    void p_internal_handle();
    void p_handle_echo();
    void p_handle_smart_echo();
    void p_handle_delay(std::vector<std::string>& bits);
    void p_invalid_request();
    void p_non_specific_response();
    void p_echo_response();

};
#endif /* test_server_h */
