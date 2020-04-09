#include <marvin/server_v3/connection_handler.hpp>

#include <marvin/http/headers_v2.hpp>
#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/server_connection_manager.hpp>
#include <marvin/server_v3/server_context.hpp>

Trog_SETLEVEL(LOG_LEVEL_WARN|LOG_LEVEL_TRACE|LOG_LEVEL_TORTRACE)
namespace Marvin {

ConnectionHandler::ConnectionHandler(
    boost::asio::io_service&    io,
    ServerConnectionManager&    connectionManager,
    ISocketSPtr                 conn_sptr,
    RequestHandlerUPtrFactory   factory
):
    m_uuid(boost::uuids::random_generator()()),
    m_io(io),
    m_connectionManager(connectionManager),
    m_factory(factory)
{
    LogTorTrace("ConnectionHandler constructor");
    /**
    * The connection and the request handler persist acrosss all messages served
    * by a connection handler. This is required to ensure that our MITM proxy
    * can handle keep-alive
    */
    m_connection = conn_sptr;
    m_requesthandler_uptr = m_factory(m_io);
    m_server_context.server_ptr = TcpServer::get_instance();
    m_server_context.connection_handler_ptr = this;
    m_server_context.server_connection_manager_ptr = &connectionManager;
    m_server_context.connection_ptr = conn_sptr.get();
    LogFDTrace(m_connection->nativeSocketFD());
    LogDebug("");
}

ConnectionHandler::~ConnectionHandler()
{
    LogTrace(" ConnectionHandler destructor");
    m_requesthandler_uptr = nullptr;
    m_connection = nullptr;
}

/*!
*   Utility method returns the underlying FD for this connection.
*   Used only for debug racing purposes
*/
long ConnectionHandler::nativeSocketFD()
{
    return m_connection->nativeSocketFD();
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
    LogTrace("ConnectionHandler Server uuid: ", m_uuid,  " fd:", nativeSocketFD());
    m_requesthandler_uptr->handle(m_server_context, m_connection, [this](){
        LogTrace("ConnectionHandler Handler done() call back uuid: ", m_uuid,  " fd:", nativeSocketFD());
        m_connectionManager.deregister(this); 
    });
}
} // namespace Marvin
