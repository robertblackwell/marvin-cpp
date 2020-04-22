#ifndef marvin_tsc_req_handler_hpp
#define marvin_tsc_req_handler_hpp

#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/configure_trog.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/server_v2/http_server.hpp>
#include <marvin/server_v2/server_context.hpp>
#include <marvin/server_v2/request_handler_base.hpp>
#include <marvin/http/uri_query.hpp>

    /**
    * Class used to handle requests in the test server for both the bb abd bf
    * set of tests. This handler:
    *   -   json encodes the full request (headers and body)
    *   -   json encodes the uuid for the connection handler and the request handler
    *
    * and puts that in the body of the response
    */
    class V2Handler : public V2::RequestHandlerBase
    {
    public:
        static int counter; // to see if there are multiple instances of the handler
        boost::asio::deadline_timer _timer;
        boost::uuids::uuid          _uuid;
        // these are the minimum requirements to send a response
        // the subordinate handlers fill these in
        Marvin::Http::MessageBaseSPtr   response_msg;
        std::string                     response_body;
        bool                            keep_alive;
        
        MessageReaderSPtr               m_rdr;
        MessageWriterSPtr               m_wrtr;
        Marvin::Http::MessageBaseSPtr   m_msg;
        std::string                     m_body;
        V2::HandlerDoneCallbackType     m_done;
        bool                            m_keep_alive;


        V2Handler(boost::asio::io_service& io);
        ~V2Handler();
        
        void handleConnect(
            V2::ServerContext&              server_context,
            MessageReaderSPtr           req,
            ISocketSPtr                 connPtr,
            V2::HandlerDoneCallbackType     done);
        void handle_get(
            V2::ServerContext&   server_context,
            MessageReaderSPtr req,
            MessageWriterSPtr resp,
            V2::HandlerDoneCallbackType done
        );

        void handleRequest(
            V2::ServerContext&   server_context,
            boost::asio::io_service& io,
            MessageReaderSPtr req,
            MessageWriterSPtr resp,
            ISocketSPtr  connPtr,
            V2::HandlerDoneCallbackType done
        );
    };
#endif /* test_server_h */
