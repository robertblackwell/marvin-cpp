//
#include <marvin/http/http_header.hpp>
#include <marvin/server_v2/connection_handler_v2.hpp>
#include <marvin/server_v2/server_connection_manager_v2.hpp>
#include <marvin/server_v2/http_server_v2.hpp>
#include <marvin/server_v2/server_context_v2.hpp>

RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
using namespace Marvin;

ConnectionHandlerV2::ConnectionHandlerV2(
    boost::asio::io_service&    io,
    ServerConnectionManagerV2&  connectionManager,
    ::ISocketSPtr                 conn_sptr,
    RequestHandlerFactoryV2     factory
):
    m_uuid(boost::uuids::random_generator()()),
    m_io(io),
    m_connectionManager(connectionManager),
    m_factory(factory)
{
    LogTorTrace();
    /**
    * The connection and the request handler persist acrosss all messages served
    * by a connection handler. This is required to ensure that our MITM proxy
    * can handle keep-alive
    */
    m_connection = conn_sptr;
    m_requestHandlerUnPtr = std::unique_ptr<RequestHandlerBaseV2>(m_factory(m_io));
    m_server_context.server_ptr = HttpServerV2::get_instance();
    m_server_context.connection_handler_ptr = this;
    m_server_context.server_connection_manager_ptr = &connectionManager;
    m_server_context.connection_ptr = conn_sptr.get();
    LogDebug("");
}

ConnectionHandlerV2::~ConnectionHandlerV2()
{
    LogTorTrace();
    m_requestHandlerUnPtr = nullptr;
    m_connection = nullptr;
}

/*!
*   Utility method returns the underlying FD for this connection.
*   Used only for debug racing purposes
*/
long ConnectionHandlerV2::nativeSocketFD()
{
    return m_connection->nativeSocketFD();
}
std::string ConnectionHandlerV2::uuid()
{
    return boost::uuids::to_string(m_uuid);
}
/*!
* Come here to invoke the handler and wait for its completion callback to be invoked
*/
void ConnectionHandlerV2::serve()
{
    LogTrace(" uuid: ", m_uuid,  " fd:", nativeSocketFD());
    m_requestHandlerUnPtr->handle(m_server_context, m_connection, [this](){
        LogTrace(" uuid: ", m_uuid,  " fd:", nativeSocketFD());
        m_connectionManager.deregister(this); 
    });
}
