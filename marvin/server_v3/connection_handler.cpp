#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/server_v3/connection_handler.hpp>

#include <marvin/http/header_fields.hpp>
#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/server_connection_manager.hpp>
#include <marvin/server_v3/server_context.hpp>

namespace Marvin {

ConnectionHandler::ConnectionHandler(
    boost::asio::io_service&    io,
    ServerConnectionManager&    connectionManager,
    ISocketSPtr                 conn_sptr,
    RequestHandlerUPtrFactory   factory
):
    m_uuid(boost::uuids::random_generator()()),
    m_uuid_str(boost::uuids::to_string(m_uuid)),
    m_io(io),
    m_connectionManager(connectionManager),
    m_factory(factory)
{
    TROG_TRACE_CTOR();
    /**
    * The connection and the request handler persist acrosss all messages served
    * by a connection handler. This is required to ensure that our MITM proxy
    * can handle keep-alive
    */
    m_connection = conn_sptr;
    m_requesthandler_uptr = m_factory(m_io);
    m_server_context.server_ptr = connectionManager.get_tcp_server_ptr();
    m_server_context.connection_handler_ptr = this;
    m_server_context.server_connection_manager_ptr = &connectionManager;
    m_server_context.connection_ptr = conn_sptr.get();
    m_server_context.uuid = m_uuid;
   TROG_TRACE_FD(m_connection->native_socket_fd());
}

ConnectionHandler::~ConnectionHandler()
{
   TROG_TRACE3(" ConnectionHandler destructor");
    m_requesthandler_uptr = nullptr;
    m_connection = nullptr;
}

/*!
*   Utility method returns the underlying FD for this connection.
*   Used only for debug racing purposes
*/
long ConnectionHandler::native_socket_fd()
{
    return m_connection->native_socket_fd();
}
std::string ConnectionHandler::uuid()
{
    return boost::uuids::to_string(m_uuid);
}
/*!
* Come here to invoke the handler and wait for its completion callback to be invoked
*/
void ConnectionHandler::serve()
{
    TROG_TRACE3("ConnectionHandler Server uuid: ", m_uuid, " fd:", native_socket_fd());
    m_requesthandler_uptr->handle(m_server_context, m_connection, [this](){
       TROG_TRACE3("ConnectionHandler Handler done() call back uuid: ", m_uuid, " fd:", native_socket_fd());
        m_connectionManager.deregister(this); 
    });
}
} // namespace Marvin
