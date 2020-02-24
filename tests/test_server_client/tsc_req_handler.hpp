#ifndef marvin_tsc_req_handler_hpp
#define marvin_tsc_req_handler_hpp

#include <catch2/catch.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include "boost_stuff.hpp"
#include "rb_logger.hpp"
#include "http_header.hpp"
#include "http_server.hpp"
#include "server_context.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "uri_query.hpp"

    /**
    * Class used to handle requests in the test server for both the bb abd bf
    * set of tests. This handler:
    *   -   json encodes the full request (headers and body)
    *   -   json encodes the uuid for the connection handler and the request handler
    *
    * and puts that in the body of the response
    */
    class TscRequestHandler : public RequestHandlerBase
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
        
        MessageWriterSPtr               _resp;
        Marvin::Http::MessageBaseSPtr   _msg;
        std::string                     _body;
        HandlerDoneCallbackType         _done;
        bool                            _keep_alive;


        TscRequestHandler(boost::asio::io_service& io);
        ~TscRequestHandler();
        
        void handleConnect(
            ServerContext&              server_context,
            MessageReaderSPtr           req,
            ISocketSPtr                 connPtr,
            HandlerDoneCallbackType     done);
        void prepare_send_response(
           MessageWriterSPtr                resp,
           Marvin::Http::MessageBaseSPtr    msg,
           std::string                      body,
           HandlerDoneCallbackType          done,
           bool keep_alive
        );
        void send_response();
 
        void handle_get(
            ServerContext&   server_context,
            MessageReaderSPtr req,
            MessageWriterSPtr resp,
            HandlerDoneCallbackType done
        );
        
        void handle_post_invalid(
            ServerContext&   server_context,
            MessageReaderSPtr req,
            MessageWriterSPtr resp,
            HandlerDoneCallbackType done
        );
        void handle_post_echo(
            ServerContext&   server_context,
            MessageReaderSPtr req,
            MessageWriterSPtr resp,
            HandlerDoneCallbackType done
        );
        void handle_post_timeout(
            ServerContext&   server_context,
            MessageReaderSPtr req,
            MessageWriterSPtr resp,
            HandlerDoneCallbackType done
        );

        void handle_post(
            ServerContext&   server_context,
            MessageReaderSPtr req,
            MessageWriterSPtr resp,
            HandlerDoneCallbackType done
        );

        void handleRequest(
            ServerContext&   server_context,
            MessageReaderSPtr req,
            MessageWriterSPtr resp,
            HandlerDoneCallbackType done
        );
    };
#endif /* test_server_h */
