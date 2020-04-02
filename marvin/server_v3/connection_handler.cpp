//
#include <marvin/http/headers_v2.hpp>
#include <marvin/server_v3/connection_handler.hpp>
#include <marvin/server_v3/server_connection_manager.hpp>
#include <marvin/server_v3/http_server.hpp>
#include <marvin/server_v3/server_context.hpp>

RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
using namespace Marvin;

ConnectionHandler::ConnectionHandler(
    boost::asio::io_service&    io,
    ServerConnectionManager&  connectionManager,
    ::ISocketSPtr                 conn_sptr,
    RequestHandlerFactory     factory
):
    m_uuid(boost::uuids::random_generator()()),
    m_io(io),
    m_connectionManager(connectionManager),
    m_factory(factory)
{
    LogWarn("YYY ConnectionHandler constructor");
    /**
    * The connection and the request handler persist acrosss all messages served
    * by a connection handler. This is required to ensure that our MITM proxy
    * can handle keep-alive
    */
    m_connection = conn_sptr;
    m_requestHandlerUnPtr = std::unique_ptr<RequestHandlerBase>(m_factory(m_io));
    m_server_context.server_ptr = HttpServer::get_instance();
    m_server_context.connection_handler_ptr = this;
    m_server_context.server_connection_manager_ptr = &connectionManager;
    m_server_context.connection_ptr = conn_sptr.get();
    LogFDTrace(m_connection->nativeSocketFD());
    LogDebug("");
}

ConnectionHandler::~ConnectionHandler()
{
    LogWarn("YYY ConnectionHandler destructor");
    m_requestHandlerUnPtr = nullptr;
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
    LogTrace("XX ConnectionHandler Server uuid: ", m_uuid,  " fd:", nativeSocketFD());
    m_requestHandlerUnPtr->handle(m_server_context, m_connection, [this](){
        LogWarn("XX ConnectionHandler Handler done() call back uuid: ", m_uuid,  " fd:", nativeSocketFD());
        m_connectionManager.deregister(this); 
    });
}
