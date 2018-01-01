
#ifndef HTTP_SERVER_CONNECTION_MANAGER_HPP
#define HTTP_SERVER_CONNECTION_MANAGER_HPP

#include <set>
#include <map>
#include "marvin_error.hpp"
#include "connection_handler.hpp"

/**
* \ingroup Server
* \brief Manages open connection handlers so as to limit the number of simultanious requests that are active
* and to ensure that all request connections can be closed when thes erver shuts down.
*/

class ServerConnectionManager;
/// \ingroup Server
typedef std::shared_ptr<ServerConnectionManager> ServerConnectionManagerSPtr;
/// \ingroup Server
typedef std::function<void(Marvin::ErrorType err, ConnectionHandlerSPtr conHandler_sptr)> ConnectionManagerCallback;
/// \ingroup Server
typedef std::function<void(Marvin::ErrorType err, ConnectionHandlerSPtr conHandler_sptr)> AllowAnotherCallback;

/// \ingroup Server
class ServerConnectionManager
{
    public:
        typedef std::function<void()> AllowAnotherCallback;
    
        static ServerConnectionManager* instance;
        static ServerConnectionManager* get_instance();
        static bool verify();
        
        ServerConnectionManager(const ServerConnectionManager&) = delete;
        ServerConnectionManager& operator=(const ServerConnectionManager&) = delete;

        /**
        * \brief Construct a connection manager. The connection handler must operate in
        * a single threaded manner. That is achieved by requiring it to always
        * execute on the server strand.
        */
        ServerConnectionManager(boost::asio::io_service& io, boost::asio::strand& serverStrand, int max_connections);
    
        /**
        * \brief called by server to verify that another accept is permitted
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
        void registerConnectionHandler(ConnectionHandler* connHandler);
     
        /**
        * \brief deregister the specified connection, removes from the table and decrements
        * the count of active connection handlers.
        *
        * If the there is a "allow another callback"
        ** set invoke this callback if the number of active connection handlers is below the
        ** max allowed
        */
        void deregister(ConnectionHandler* ch);

        /**
        ** Stop all connections.
        */
        void stop_all();

    private:
        /**
        * THese methods actually perform the register, deregister functions
        * and are posted to the serverStrand
        */
        void p_register(ConnectionHandler* ch);
        void p_deregister(ConnectionHandler* ch);

        boost::asio::io_service&    m_io;
        boost::asio::strand&        m_serverStrand;
        int                         m_maxNumberOfConnections;
        int                         m_currentNumberConnections;
        AllowAnotherCallback        m_callback;
        
        std::map<ConnectionHandler*, std::unique_ptr<ConnectionHandler>> m_connections;
        std::map<long, long>  m_fd_list;
};

#endif // HTTP_SERVER_CONNECTION_MANAGER_HPP
