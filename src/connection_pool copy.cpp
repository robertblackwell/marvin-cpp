//
//  connection_pool.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/14/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include "connection_pool.hpp"

#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "client_connection.hpp"

//---------------------------------------------------------------------------------------------------
// A list of all available connections. They are primed with an io_service but NOT host information
//---------------------------------------------------------------------------------------------------
class AllConnectionsType
{
    private:
    std::vector<std::shared_ptr<ClientConnections>>  _connections;
    boost::asio::io_service _io;
    
    public:
    AllConnections(boost::asio::io_service& io, std::size_t max): _io(io)
    {
        for(int i = 0; i < max; i++){
            add(std::shared_ptr<ClientConnection>(new ClientConnection(_io)) );
        }
    }
    std::size_t size()
    {
        return _connections.size();
    }
    void add(ClientConnection* conn)
    {
        _connections.push_back(conn);
    }
}
//---------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------
class FreeConnectionsType
{
    private:
        std::vector<ClientConnections>  _connections;
    
    public:
        FreeConnections(AllConnections& all): _io(io)
        {
            for(const& c: all){
                add(c.get());
            }
        }
        std::size_t size()
        {
            return _connections.size();
        }
        
        ClientConnection*
        removeLast()
        {
            auto oldest = _connections.back();
            _connection.erase(oldest);
            return oldest;
        }
        
        void add(ClientConnection* conn)
        {
            _connections.push_back(conn);
        }

}
//---------------------------------------------------------------------------------------------------
// A class to hold a connection once it has been assigned to a host.
// @PROBLEM - how to know if the connection is still open ??
//---------------------------------------------------------------------------------------------------
class AssignedConnection
{
    private:
        std::string         _hostId;
        ClientConnection*   _conn;
    public:
        AssignedConnection(std::string hostId, ClientConnection* conn): _hostId(hostId), _conn(conn) {}
}
//---------------------------------------------------------------------------------------------------
// A list of assigned (and assumed open) connections that are not being used
// @PROBLEM - what happens if it closes while on this list
//---------------------------------------------------------------------------------------------------
class IdleConnectionsType
{
    private:
        std::vector<AssignedConnections>  _connections;
        boost::asio::io_service _io;
    
    public:
        IdleConnections(boost::asio::io_service& io): _io(io)
        {
        }
        std::size_t size()
        {
            return _connections.size();
        }
        AssignedConnection*
        removeOldest()
        {
            auto oldest = _connections.back();
            _connection.erase(oldest);
            return oldest;
        }
    
        void
        add(AssignedConnection* conn)
        {
            _connections.push_back(conn);
        }

};
//---------------------------------------------------------------------------------------------------
// List of assigned connections currently "in use"
//---------------------------------------------------------------------------------------------------
class InUseConnectionsType
{
    private:
    std::vector<AssignedConnections>  _connections;
    boost::asio::io_service _io;
    
    public:
    
    InUseConnections(boost::asio::io_service& io): _io(io)
    {
    }
    std::size_t size()
    {
        return _connections.size();
    }
    
    void
    remove(AssignedConnection* aConn)
    {
        if( _connections.find(aConn) == std::map::end { assert(false);}
        _connections.erase(aConn);
        
    }
    
    void
    add(AssignedConnection* conn)
    {
        _connections.push_back(conn);
    }



};
//---------------------------------------------------------------------------------------------------
// Holds a pending request for a connection
//---------------------------------------------------------------------------------------------------
typedef  std::function<void()> ConnectionRequestCallbackType;
class ConnectionRequest
{
    private:
        std::string             _hostId;
        std:function<void()>    _callBack;
    public:
    
    ConnectionRequest(std::string hostId, ConnectionRequestCallbackType cb)
    {
        _hostId = hostId;
        +_callBack = cb;
    }
}

//---------------------------------------------------------------------------------------------------
// List of requests for a connection that have been put into "wait"
//---------------------------------------------------------------------------------------------------
class WaitingRequestsType
{
    private:
    std::vector<ConnectionRequest>  _waitingRequests;
    boost::asio::io_service _io;
    
    public:
    WaitingRequests(boost::asio::io_service& io): _io(io)
    {
    }
    
    std::size_t
    size()
    {
        return _connections.size();
    }
    
    ConnectionRequest*
    removeOldest()
    {
        auto oldest = _connectionRequests.back();
        _connection.erase(oldest);
        return oldest;
    }
    
    void
    add(ConnectionRequest* connReq)
    {
        _connectionRequests.push_back(connReq);
    }



};
//---------------------------------------------------------------------------------------------------
// map of counters by assigned host - that is host that have been assigned to a connection
// where the connection is either inuse of idle
//---------------------------------------------------------------------------------------------------
class HostsCounterType
{
    private:
    std::map<std::string, std::size_t>  _count;
    
    public:
    HostsCounterType(){}
    void addHost(std::string hostId)
    {
        if( _count.find(hostId) != std::map::end { assert(false);}
        _count[hostId] = 0;
    }
    void removeHost(std::string hostId)
    {
        if( _count.erase(hostId) != 1) { assert(false); };
    }
    void incrementHost(std::string hostId)
    {
        if( _count.find(hostId) != std::map::end { assert(false);}
        _count[hostId] = _count[hostId]++;
    }
    void decrementHost(std::string hostId)
    {
        if( _count.find(hostId) != std::map::end { assert(false);}
        _count[hostId]-- = _count[hostId]--;
    }
    std::size_t countFor(std::string hostId)
    {
        if( _count.find(hostId) != std::map::end { assert(false);}
        return _count[hostId];
    }
    
};

ConnectionPool* globalConnectionPool = NULL;

ConnectionPool::ConnectionPool(boost::asio::io_service& io_service): io(io_service), resolver_(io_service)
{
    
}
ConnectionPool* ConnectionPool::getInstance(boost::asio::io_service& io)
{
    if( globalConnectionPool == NULL ){
        globalConnectionPool = new ConnectionPool(io);
    }
    return globalConnectionPool;
}
/**
 * get a connection to the scheme::server
 */
void ConnectionPool::asyncGetClientConnection(
            std::string scheme, // http: or https:
            std::string server, // also called hostname
            std::string service,// http/https or port number
            ConnectCallbackType cb
)
{
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),
                                                server,
                                                service,
                                                tcp::resolver::query::canonical_name);
    
    ClientConnection* conn = new ClientConnection(io, scheme, server, service);
    //
    // a bunch of logic here about find existing, add to connection table etc
    //
    conn->asyncConnect([this, conn, cb](Marvin::ErrorType& ec, ClientConnection* conn){
        if( !ec ){
            postSuccess(cb, conn);
        }else{
            postFail(cb, ec);
        }
    });

}
void ConnectionPool::releaseClientConnection(ClientConnection* conn)
{
    LogDebug("");
    delete conn;
}
void ConnectionPool::postSuccess(ConnectCallbackType cb, ClientConnection* conn)
{
    Marvin::ErrorType merr = Marvin::make_error_ok();
    auto pf = std::bind(cb, merr, conn);
    io.post(pf);
}
void ConnectionPool::postFail(ConnectCallbackType cb, Marvin::ErrorType& ec)
{
    Marvin::ErrorType merr = ec;
    auto pf = std::bind(cb, merr, nullptr);
    io.post(pf);
}

