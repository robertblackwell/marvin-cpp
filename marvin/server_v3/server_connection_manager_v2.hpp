
#ifndef HTTP_SERVER_V2_CONNECTION_MANAGER_HPP
#define HTTP_SERVER_V2_CONNECTION_MANAGER_HPP

#include <set>
#include <map>
#include <marvin/error/marvin_error.hpp>
#include <marvin/server_v2/connection_handler_v2.hpp>

/**
* \ingroup Server
* \brief Manages open connection handlers so as to limit the number of simultanious requests that are active
* and to ensure that all request connections can be closed when thes erver shuts down.
*/
namespace Marvin {

class ServerConnectionManagerV2;
/// \ingroup Server
typedef std::shared_ptr<ServerConnectionManagerV2> ServerConnectionManagerV2SPtr;
/// \ingroup Server
typedef std::function<void(Marvin::ErrorType err, ConnectionHandlerV2SPtr conHandler_sptr)> ConnectionManagerCallback;
/// \ingroup Server
typedef std::function<void(Marvin::ErrorType err, ConnectionHandlerV2SPtr conHandler_sptr)> AllowAnotherCallback;

/// \ingroup Server
class ServerConnectionManagerV2
{
    public:
        typedef std::function<void()> AllowAnotherCallback;
    
        static ServerConnectionManagerV2* instance;
        static ServerConnectionManagerV2* get_instance();
        static bool verify();
        
        ServerConnectionManagerV2(const ServerConnectionManagerV2&) = delete;
        ServerConnectionManagerV2& operator=(const ServerConnectionManagerV2&) = delete;

        /**
        * \brief Construct a connection manager. The connection handler must operate in
        * a single threaded manner. That is achieved by requiring it to always
        * execute on the server strand.
        */
        ServerConnectionManagerV2(boost::asio::io_service& io, int max_connections);
    
        /**
        * Called by server to verify that another accept is permitted
        * within the context of the ConnectionManager's rescource allocation scheme.
        *
        * If not the ConnectionManager will wait till there is enough resource
        * and invoke the cb to signal the server to continue. Did it this was
        * so that the ConnectionManager does not need to know how to create
        * ConnectionHandler, RequestHandler
        */
        void allowAnotherConnection(AllowAnotherCallback cb);
    
        /**
        ** \brief Register a connection handler in a table so that it stays around to process request/response
        ** and increments the count of connection handler active
        */
        void registerConnectionHandler(ConnectionHandlerV2* connHandler);
     
        /**
        * \brief deregister the specified connection, removes from the table and decrements
        * the count of active connection handlers.
        *
        * If the there is a "allow another callback"
        ** set invoke this callback if the number of active connection handlers is below the
        ** max allowed
        */
        void deregister(ConnectionHandlerV2* ch);

        /**
        ** Stop all connections.
        */
        void stop_all();

    private:
        /**
        * THese methods actually perform the register, deregister functions
        * and are posted to the serverStrand
        */
        void p_register(ConnectionHandlerV2* ch);
        void p_deregister(ConnectionHandlerV2* ch);
        int    m_connection_count;

        boost::asio::io_service&    m_io;
        int                         m_maxNumberOfConnections;
        int                         m_currentNumberConnections;
        AllowAnotherCallback        m_allow_more_callback;
        
        std::map<ConnectionHandlerV2*, std::unique_ptr<ConnectionHandlerV2>> m_connections;
        std::map<long, long>  m_fd_list;
};
} // namespace Marvin
#endif // HTTP_SERVER_CONNECTION_MANAGER_HPP
