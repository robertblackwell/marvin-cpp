
#ifndef marvin_server_v3_server_context_hpp
#define marvin_server_v3_server_context_hpp
#include <marvin/connection/socket_interface.hpp>

// TAKE CARE - this object is only for information purposes and the values in this object are only ever traced
// no destructor as the refs are weak.
namespace Marvin {
class TcpServer;
class ServerConnectionManager;
class ConnectionHandler;

/**
* \ingroup tcpserver
*
*/
struct ServerContext
{
    TcpServer*                  server_ptr;
    ServerConnectionManager*    server_connection_manager_ptr;
    ConnectionHandler*          connection_handler_ptr;
    ISocket*        			connection_ptr;
    boost::uuids::uuid          uuid;

    ~ServerContext()
    {
        // do nothing - these pointers are not owned by this class
    }

};
} // namespace Marvin
#endif /* server_context_hpp */
