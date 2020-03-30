
#ifndef marvin_connection_handler_v2_hpp
#define marvin_connection_handler_v2_hpp
/// \ingroup Server
#include <cstdio>

#include <marvin/boost_stuff.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>

#include <marvin/server_v3/server_context.hpp>
#include <marvin/server_v3/request_handler_base.hpp>

#include <marvin/connection/socket_interface.hpp>
namespace Marvin {
class ServerConnectionManager;
class ConnectionHandler;
/// \ingroup Server
using ConnectionHandlerSPtr = std::shared_ptr<ConnectionHandler>;

/// An instance of this class is created by the server for every open client connection;
/// this instance manages the life time and invocation of the request handler that actually services the
/// incoming messages along a single connection. This class is protocol agnostic.
class ConnectionHandler
{
    public:
        ConnectionHandler(
            boost::asio::io_service&     io,
            ServerConnectionManager&   connectionManager,
            ::ISocketSPtr                  conn_sptr, 
            RequestHandlerFactory      factory
        );

        ~ConnectionHandler();
    
        void serve();
        long nativeSocketFD();
        std::string uuid();
    private:
    
        void p_handler_complete(Marvin::ErrorType err);
        void p_handle_connect_complete(bool hijack);
        
        boost::uuids::uuid                     m_uuid;
        boost::asio::io_service&               m_io;
        ServerConnectionManager&             m_connectionManager;
        std::unique_ptr<RequestHandlerBase>  m_requestHandlerUnPtr;
        RequestHandlerFactory                m_factory;
    
        ISocketSPtr                            m_connection;
        ServerContext                        m_server_context;
};
} // namespace Marvin
#endif /* ConnectionHandler_hpp */
