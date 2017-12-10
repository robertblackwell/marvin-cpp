
#ifndef HTTP_SERVER_CONNECTION_MANAGER_HPP
#define HTTP_SERVER_CONNECTION_MANAGER_HPP

#include <set>
#include <map>
#include "marvin_error.hpp"
#include "connection_handler.hpp"

/** Manages open connection handlers so that they may be cleanly stopped when the server
* needs to shut down.
*
* And limits the number of connection handlers active at any time so the server cannot get swamped.
*/

class ServerConnectionManager;

typedef std::shared_ptr<ServerConnectionManager> ServerConnectionManagerSPtr;
typedef std::function<void(Marvin::ErrorType err, ConnectionHandlerSPtr conHandler_sptr)> ConnectionManagerCallback;
typedef std::function<void(Marvin::ErrorType err, ConnectionHandlerSPtr conHandler_sptr)> AllowAnotherCallback;

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
        * Construct a connection manager. The connection handler must operate in
        * a single threaded manner. That is achieved by requiring it to always
        * execute on the server strand.
        */
        ServerConnectionManager(boost::asio::io_service& io, boost::asio::strand& serverStrand, int max_connections);
    
        /**
        * called by server to verify that another accept is permitted
        * within the context of the ConnectionManager's rescource allocation scheme.
        * If not the ConnectionManager will wait till there is enough resource
        * and invoke the cb to signal the server to continue. Did it this was
        * so that the ConnectionManager does not need to know how to create
        * ConnectionHandler, RequestHandler
        */
        void allowAnotherConnection(AllowAnotherCallback cb);
    
        /**
        ** Register a connection handler in a table so that it stays around to process request/response
        ** and increments the count of connection handler active
        */
        void registerConnectionHandler(ConnectionHandler* connHandler);
     
        /**
        ** deregister the specified connection, removes from the table and decrements
        ** the count of active connection handlers. If the there is a "allow another callback"
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
        void _register(ConnectionHandler* ch);
        void _deregister(ConnectionHandler* ch);

        boost::asio::io_service&    _io;
        boost::asio::strand&        _serverStrand;
        int                         _maxNumberOfConnections;
        int                         _currentNumberConnections;
        AllowAnotherCallback        _callback;
        
        std::map<ConnectionHandler*, std::unique_ptr<ConnectionHandler>> _connections;
        std::map<long, long>  _fd_list;
};

#endif // HTTP_SERVER_CONNECTION_MANAGER_HPP
