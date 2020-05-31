#ifndef marvin_server_v3_app_interface_hpp
#define marvin_server_v3_app_interface_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/server_context.hpp>
#include <marvin/server_v3/request_handler_interface.hpp>
#include <marvin/server_v3/timer.hpp>
namespace Marvin {
/**
* Interface definition for classes used to handle requests in server_v3.
*
* Any class that is act as the "handler" for http(s) requests passed from an instance
* of TcpServer must conform to this interface.
*/
class AppInterface : public Marvin::RequestHandlerInterface
{
public:

    AppInterface(boost::asio::io_service& io);
    ~AppInterface();
    
    virtual void handle(
        Marvin::ServerContext&            server_context,
        ISocketSPtr                       connPtr,
        Marvin::HandlerDoneCallbackType   done
    ) = 0;

};
} // namespace
#endif /* test_server_h */
