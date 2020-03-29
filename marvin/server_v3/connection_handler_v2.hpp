
#ifndef marvin_connection_handler_v2_hpp
#define marvin_connection_handler_v2_hpp
/// \ingroup Server
#include <cstdio>

#include <marvin/boost_stuff.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>

#include <marvin/server_v2/server_context_v2.hpp>
#include <marvin/server_v2/request_handler_base_v2.hpp>

#include <marvin/connection/socket_interface.hpp>
namespace Marvin {
class ServerConnectionManager;
class ConnectionHandlerV2;
/// \ingroup Server
using ConnectionHandlerV2SPtr = std::shared_ptr<ConnectionHandlerV2>;

/// An instance of this class is created by the server for every open client connection;
/// this instance manages the life time and invocation of the request handler that actually services the
/// incoming messages along a single connection. This class is protocol agnostic.
class ConnectionHandlerV2
{
    public:
        ConnectionHandlerV2(
            boost::asio::io_service&     io,
            ServerConnectionManagerV2&   connectionManager,
            ::ISocketSPtr                  conn_sptr, 
            RequestHandlerFactoryV2      factory
        );

        ~ConnectionHandlerV2();
    
        void serve();
        long nativeSocketFD();
        std::string uuid();
    private:
    
        void p_handler_complete(Marvin::ErrorType err);
        void p_handle_connect_complete(bool hijack);
        
        boost::uuids::uuid                     m_uuid;
        boost::asio::io_service&               m_io;
        ServerConnectionManagerV2&             m_connectionManager;
        std::unique_ptr<RequestHandlerBaseV2>  m_requestHandlerUnPtr;
        RequestHandlerFactoryV2                m_factory;
    
        ISocketSPtr                            m_connection;
        ServerContextV2                        m_server_context;
};
} // namespace Marvin
#endif /* ConnectionHandlerV2_hpp */
