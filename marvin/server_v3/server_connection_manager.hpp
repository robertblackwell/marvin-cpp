
#ifndef marvin_server_v3_server_connection_manager_hpp
#define marvin_server_v3_server_connection_manager_hpp

#include <set>
#include <map>
#include <marvin/error/marvin_error.hpp>
#include <marvin/server_v3/connection_handler.hpp>

/**
* \ingroup tcpserver
* \brief Manages open connection handlers so as to limit the number of simultanious requests that are active
* and to ensure that all request connections can be closed when thes erver shuts down.
*/
namespace Marvin {

class ServerConnectionManager;
/// \ingroup Server
typedef std::shared_ptr<ServerConnectionManager> ServerConnectionManagerSPtr;
/// \ingroup Server
typedef std::function<void(Marvin::ErrorType err, ConnectionHandlerSPtr conHandler_sptr)> ConnectionManagerCallback;
/// \ingroup Server
typedef std::function<void(Marvin::ErrorType err, ConnectionHandlerSPtr conHandler_sptr)> AllowAnotherCallback;

/**
* Each instance of TcpServer has one instance of ServerConnectionManager to do the bookkeeping
* related to outstanding open clientconnections.
*
*
*/
class ServerConnectionManager
{
    public:
        typedef std::function<void()> AllowAnotherCallback;
    
        static bool verify();
        
        ServerConnectionManager(const ServerConnectionManager&) = delete;
        ServerConnectionManager& operator=(const ServerConnectionManager&) = delete;

        /**
        * Construct a connection manager. The connection handler must operate in
        * a single threaded manner.
        */
        ServerConnectionManager(boost::asio::io_service& io, TcpServer* tcp_server_ptr, int max_connections);
        ~ServerConnectionManager();
        /**
        * Called by server to verify that another accept is permitted
        * within the context of the ConnectionManager's rescource allocation scheme.
        *
        * If not the ConnectionManager will wait till there is enough resource
        * and invoke the cb to signal the server to continue. Did it this was
        * so that the ConnectionManager does not need to know how to create
        * ConnectionHandler, RequestHandler
        */
        void allow_another_connection(AllowAnotherCallback cb);
    
        /**
        ** \brief Register a connection handler in a table so that it stays around to process request/response
        ** and increments the count of connection handler active
        */
        void register_connection_handler(ConnectionHandler* connHandler);
     
        /**
        * \brief deregister the specified connection, removes from the table and decrements
        * the count of active connection handlers.
        *
        * If the there is a "allow another callback"
        ** set invoke this callback if the number of active connection handlers is below the
        ** max allowed
        */
        void deregister(ConnectionHandler* ch);

        TcpServer* get_tcp_server_ptr();

        /**
        ** Stop all connections.
        */
        void stop_all();

    private:
        /**
        * These methods perform the register, deregister functions
        */
        void p_register(ConnectionHandler* ch);
        void p_deregister(ConnectionHandler* ch);

        TcpServer*                  m_parent_server_ptr;
        int                         m_connection_count;
        boost::asio::io_service&    m_io;
        int                         m_maxNumberOfConnections;
        int                         m_currentNumberConnections;
        AllowAnotherCallback        m_allow_more_callback;
        
        std::map<ConnectionHandler*, std::unique_ptr<ConnectionHandler>> m_connections;
        std::map<long, long>        m_fd_list;
};
} // namespace Marvin
#endif // HTTP_SERVER_CONNECTION_MANAGER_HPP
