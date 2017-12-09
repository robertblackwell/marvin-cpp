//
//  connection_pool.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/14/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef connection_pool_hpp
#define connection_pool_hpp

#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include "connection_interface.hpp"
#include "connection_pool.hpp"

//---------------------------------------------------------------------------------------------------
// InUseConnections - List of assigned connections currently "in use"
//---------------------------------------------------------------------------------------------------
class InUseConnectionsType
{
    private:
    std::map<ConnectionInterface*, ConnectionInterface*>  _connections;
    
    public:
    
    InUseConnectionsType();
    std::size_t size();
    
    void remove(ConnectionInterface* aConn);
    
    void add(ConnectionInterface* conn);
};
//---------------------------------------------------------------------------------------------------
// ConnectionRequest - Holds a pending request for a connection
//---------------------------------------------------------------------------------------------------
typedef  std::function<void()> ConnectionRequestCallbackType;
class ConnectionRequest
{
    public:
        std::string             _scheme;
        std::string             _server;
        std::string             _service;
        ConnectCallbackType    _callback;
    
        ConnectionRequest(
            std::string scheme,
            std::string server,
            std::string service,
            ConnectCallbackType cb
        );
        ~ConnectionRequest();
};


//---------------------------------------------------------------------------------------------------
// WaitingRequests - List of requests for a connection that have been put into "wait"
//---------------------------------------------------------------------------------------------------
class WaitingRequestsType
{
    private:
    std::vector<ConnectionRequest*>  _waitingRequests;
    
    public:
    WaitingRequestsType();
    std::size_t    size();

    ConnectionRequest*  find(std::string scheme,std::string server,std::string service);
    
    ConnectionRequest*  removeOldest();
    void add(ConnectionRequest* connReq);
};

//
//  This singleton class manages a pool of connections, the basic algorithm is:
//
//  all_connections
//  ===============
//  is a fixed size list of all the connection objects available, but initially they are not assigned to a host.
//
//  access requirement - not changed, static array
//
//  free
//  ====
//  a list of all the connections that are not assigned to a host. This list will quickly become empty
//
//  in_use
//  ======
//  is a list of all the connections that are a) assigned to a host and actually in use
//  access requirement add to top, search from bottom to top, removed from anywhere
//
//  access requirement - add to top, remove from anywhere - no searching
//
//  idle
//  ====
//  is a list of connections that have been assigned to a host but have since been released
//  so are available for re-use.
//
//  access requirement add to top, search from bottom to top, removed from anywhere
//
//  waiting_requests
//  ================
//  a list of requests to obtain a connection
//
//  add to top, search from bottom to top, remove from anywhere
//
//  host_counter
//  ============
//  a list of hosts that have connections assigned and are in_use or idle, plus a waiting_list for each host
//
//
//  algorithm - rqeuest a connection
//
//  get_a_connection(host_id, callback)
//
//      if inUse.size()= max_connections
//          req = make_conn_request(host_id, callback)
//          waitingRequests.add(req)
//      else
//          conn = create_and_connect()
//          inUse.add(conn)
//          connection_count++
//          req.callback(conn)
//
//  release_connection(conn)
//      if waitinRequests.size() > 0
//          req = waitRequests.findOldestRequestForHost(conn.host_id)
//          req.callback(conn)
//      else
//          inUse.remove(conn)
//          conn.close
//          delete conn
//          connection_count--
//
class ConnectionHandlerManager
{
public:
    static ConnectionHandlerManager* getInstance(boost::asio::io_service& io);
           
    ConnectionHandlerManager(boost::asio::io_service& io, boost::asio::strand& strand);
    
    void init(boost::asio::io_service& io);
    
    void asyncAcquireConnectionHandler(
        std::string scheme,
        std::string server,
        std::string port,
        ConnectCallbackType cb
    );
    
    void releaseConnectionHandler(ConnectionHandler* conn_handler);

private:
    
    // this is the real interface - but is wrapped in a strand by the public call
    void __asyncAcquireConnection(
        std::string scheme,
        std::string server,
        std::string port,
        ConnectCallbackType cb
    );

    // this is the real interface - but is wrapped in a strand by the public call
    void __releaseConnection(ConnectionInterface* conn);
    
    void createNewConnection(
                std::string scheme, // http: or https:
                std::string server, // also called hostname
                std::string service,// http/https or port number
                ConnectCallbackType cb
    );

    void postSuccess(ConnectCallbackType cb, ConnectionInterface* conn);
    void postFail(ConnectCallbackType cb, Marvin::ErrorType& ec);
    
    boost::asio::io_service&        io;
    boost::asio::ip::tcp::resolver  resolver_;
    
    std::size_t                     _maxConnections;
    InUseConnectionsType            _inUse;
    WaitingRequestsType             _waitingRequests;
    
    // a srand on which to execute all the pools functions to prevent thread contention
    boost::asio::strand             _poolStrand;

};




#endif /* connection_pool_hpp */
