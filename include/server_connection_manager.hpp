
#ifndef HTTP_SERVER_CONNECTION_MANAGER_HPP
#define HTTP_SERVER_CONNECTION_MANAGER_HPP

#include <set>
#include <map>


/// Manages open connection handlers so that they may be cleanly stopped when the server
/// needs to shut down.

/// TConnectionHandler must be an instantiation of the template ConnectionHandler
template<class TConnectionHandler>
class ServerConnectionManager
{
public:
    ServerConnectionManager(const ServerConnectionManager&) = delete;
    ServerConnectionManager& operator=(const ServerConnectionManager&) = delete;

    /// Construct a connection manager.
    ServerConnectionManager();

    void registerConnectionHandler(TConnectionHandler* connHandler);
 
    /// deregister the specified connection.
    void deregister(TConnectionHandler* ch);
    /// Stop the specified connection.
    void stop(TConnectionHandler* ch);

    /// Stop all connections.
    void stop_all();

private:
  /// The managed connections.
#define CM_SMARTPOINTER  
#ifdef CM_SMARTPOINTER
  std::map<TConnectionHandler*, std::unique_ptr<TConnectionHandler>> _connections;
#else
  std::set<TConnectionHandler*> _connections;
#endif
};

#include "server_connection_manager.ipp"

#endif // HTTP_SERVER_CONNECTION_MANAGER_HPP
