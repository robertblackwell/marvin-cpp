#include <marvin/server_v3/connection_handler.hpp>

#include <marvin/http/headers_v2.hpp>
#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/server_connection_manager.hpp>
#include <marvin/server_v3/server_context.hpp>

TROG_SET_FILE_LEVEL(Trog::LogLevelWarn|Trog::LogLevelTrace3|Trog::LogLevelCTorTrace)
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
    m_server_context.server_ptr = connectionManager.getTcpServerPtr();
    m_server_context.connection_handler_ptr = this;
    m_server_context.server_connection_manager_ptr = &connectionManager;
    m_server_context.connection_ptr = conn_sptr.get();
    m_server_context.uuid = m_uuid;
   TROG_TRACE_FD(m_connection->nativeSocketFD());
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
    TROG_TRACE3("ConnectionHandler Server uuid: ", m_uuid,  " fd:", nativeSocketFD());
    m_requesthandler_uptr->handle(m_server_context, m_connection, [this](){
       TROG_TRACE3("ConnectionHandler Handler done() call back uuid: ", m_uuid,  " fd:", nativeSocketFD());
        m_connectionManager.deregister(this); 
    });
}
} // namespace Marvin
