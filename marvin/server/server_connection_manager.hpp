
#ifndef HTTP_SERVER_CONNECTION_MANAGER_HPP
#define HTTP_SERVER_CONNECTION_MANAGER_HPP

#include <set>
#include <map>
#include "marvin_error.hpp"


/** Manages open connection handlers so that they may be cleanly stopped when the server
* needs to shut down.
*
* And (TODO) limit the number so the server cannot get swamped. This will require  change
* to the interface
*
*/
/// TConnectionHandler must be an instantiation of the template ConnectionHandler

template<class TConnectionHandler>
class ServerConnectionManager
{
    public:
        typedef std::shared_ptr<TConnectionHandler> TConnectionHandlerSPtr;
        typedef std::function<void(Marvin::ErrorType err, TConnectionHandlerSPtr conn_sptr)> TConnHandlerCallback ;
    
        ServerConnectionManager(const ServerConnectionManager&) = delete;
        ServerConnectionManager& operator=(const ServerConnectionManager&) = delete;

        /**
        * Construct a connection manager. The connection handler must operate in
        * a single threaded manner. That is achieved by requiring it to always
        * execute on the server strand.
        */
        ServerConnectionManager(boost::asio::io_service& io, boost::asio::strand& serverStrand);

        /**
        * Acquire a new ConnectionHandler instance. This returns a new connection handler via
        * a call back so that the return of a new connection handler can be delayed
        * until an already active connection handler is released
        */
        void acquireConnectionHandler(TConnHandlerCallback cb);
//        void acquireConnectionHandler(std::function<void(Mavin::ErrorType err, TConnectionHandlerSPtr conn_sptr> cb);
    
        /**
        * Release a connection handler.
        */
        void releaseConnectionHandler(TConnectionHandlerSPtr conn_sptr);
    
        /**
        ** Register a connection handler in a table so that it stays around to process request/response
        */
        void registerConnectionHandler(TConnectionHandler* connHandler);
     
        /**
        ** deregister the specified connection.
        */
        void deregister(TConnectionHandler* ch);

        /**
        ** Stop all connections.
        */
        void stop_all();

    private:
        void _deregister(TConnectionHandler* ch);

        boost::asio::io_service&    _io;
        boost::asio::strand&        _serverStrand;
        
        std::map<TConnectionHandler*, std::unique_ptr<TConnectionHandler>> _connections;
};

#include "server_connection_manager.ipp"

#endif // HTTP_SERVER_CONNECTION_MANAGER_HPP
