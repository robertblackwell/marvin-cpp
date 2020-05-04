
#ifndef marvin_server_v3_connection_handler_hpp
#define marvin_server_v3_connection_handler_hpp

// #include <cstdio>
#include <string>
#include <marvin/boost_stuff.hpp>

#include <marvin/connection/socket_interface.hpp>
#include <marvin/server_v3/server_context.hpp>
#include <marvin/server_v3/request_handler_interface.hpp>

namespace Marvin {

class ServerConnectionManager;
class ConnectionHandler;
using ConnectionHandlerSPtr = std::shared_ptr<ConnectionHandler>;

/**
* \ingroup tcpserver 
*  An instance of this class is created by the TcpServer for every open client connection.
*
*   Such an instance manages the life time and invocation of the necessary request handler 
*   that actually services the incoming messages. This class is protocol agnostic.
*/
class ConnectionHandler
{
    public:
        ConnectionHandler(
            boost::asio::io_service&   io,
            ServerConnectionManager&   connectionManager,
            ISocketSPtr                conn_sptr, 
            RequestHandlerUPtrFactory  factory
        );

        ~ConnectionHandler();
    
        void serve();
        long nativeSocketFD();
        std::string uuid();
    private:
    
        void p_handler_complete(Marvin::ErrorType err);
        void p_handle_connect_complete(bool hijack);
        
        boost::uuids::uuid                      m_uuid;
        std::string                             m_uuid_str;
        boost::asio::io_service&                m_io;
        ServerConnectionManager&                m_connectionManager;
        RequestHandlerInterfaceUPtr             m_requesthandler_uptr;
        RequestHandlerUPtrFactory               m_factory;
    
        ISocketSPtr                             m_connection;
        ServerContext                           m_server_context;
};
} // namespace Marvin
#endif /* ConnectionHandler_hpp */
