
#ifndef server_context_hpp
#define server_context_hpp

// TAKE CARE - this object is only for information purposes and the values in this object are only ever traced
// no destructor as the refs are weak.

class HTTPServer;
class ServerConnectionManager;
class ConnectionHandler;
class ISocket;

struct ServerContext
{
    HTTPServer*                 server_ptr;
    ServerConnectionManager*    server_connection_manager_ptr;
    ConnectionHandler*          connection_handler_ptr;
    ISocket*        			connection_ptr;
};

#endif /* server_context_hpp */
