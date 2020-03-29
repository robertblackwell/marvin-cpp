
#ifndef server_context_v2_hpp
#define server_context_v2_hpp
#include <marvin/connection/socket_interface.hpp>

// TAKE CARE - this object is only for information purposes and the values in this object are only ever traced
// no destructor as the refs are weak.
namespace Marvin {
class HttpServerV2;
class ServerConnectionManagerV2;
class ConnectionHandlerV2;

struct ServerContextV2
{
    HttpServerV2*               server_ptr;
    ServerConnectionManagerV2*    server_connection_manager_ptr;
    ConnectionHandlerV2*          connection_handler_ptr;
    ::ISocket*        			connection_ptr;
};
} // namespace Marvin
#endif /* server_context_hpp */
