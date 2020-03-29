#ifndef marvin_test_server_v2_handler_hpp
#define marvin_test_server_v2_handler_hpp

#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <marvin/http/http_header.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v2/http_server_v2.hpp>
#include <marvin/server_v2/server_context_v2.hpp>
#include <marvin/server_v2/request_handler_base_v2.hpp>

/**
* Class used to handle requests in the test server for both the bb abd bf
* set of tests. This handler:
*   -   json encodes the full request (headers and body)
*   -   json encodes the uuid for the connection handler and the request handler
*
* and puts that in the body of the response
*/
class V2Handler : public Marvin::RequestHandlerBaseV2
{
public:
    static int counter; // to see if there are multiple instances of the handler
    // boost::asio::deadline_timer m_imer;
    boost::uuids::uuid       m_uuid;
    // these are the minimum requirements to send a response
    // the subordinate handlers fill these in
    Marvin::Http::MessageBaseSPtr       response_msg;
    std::string                         response_body;
    bool                                keep_alive;
    
    MessageWriterSPtr                   m_resp;
    Marvin::Http::MessageBaseSPtr       m_msg;
    std::string                         m_body;
    Marvin::HandlerDoneCallbackTypeV2   m_done;
    bool                                m_keep_alive;


    V2Handler(boost::asio::io_service& io);
    ~V2Handler();
    
    void handle(
        Marvin::ServerContextV2&            server_context,
        ISocketSPtr                         connPtr,
        Marvin::HandlerDoneCallbackTypeV2   done
    );
};
#endif /* test_server_h */
