#ifndef tsc_req_handler_hpp
#define tsc_req_handler_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

#include "boost_stuff.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "rb_logger.hpp"
#include "http_header.hpp"
//#include "UriParser.hpp"
//#include "url.hpp"
#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "uri_query.hpp"

namespace body_format
{
    /**
    * Class used to handle requests in the test server
    */
    class RequestHandler : public RequestHandlerBase
    {
    public:
        static int counter; // to see if there are multiple instances of the handler
        boost::asio::deadline_timer _timer;
        boost::uuids::uuid          _uuid;

        RequestHandler(boost::asio::io_service& io);
        ~RequestHandler();
        
        void handleConnect(
            MessageReaderV2SPtr           req,
            ConnectionInterfaceSPtr     connPtr,
            HandlerDoneCallbackType    done);

        void pathHandler_A(
            MessageReaderV2SPtr req,
            MessageWriterV2SPtr resp,
            HandlerDoneCallbackType done
        );
        void pathHandler_B(
            MessageReaderV2SPtr req,
            MessageWriterV2SPtr resp,
            HandlerDoneCallbackType done
        );
        void pathHandler_C(
            MessageReaderV2SPtr req,
            MessageWriterV2SPtr resp,
            HandlerDoneCallbackType done
        );
        std::string get_dispatcher(std::string p);
        std::string post_dispatcher(MessageReaderV2SPtr req);
        
        void handleRequest(
            MessageReaderV2SPtr req,
            MessageWriterV2SPtr resp,
            HandlerDoneCallbackType done
        );
    };

}
#endif /* test_server_h */
