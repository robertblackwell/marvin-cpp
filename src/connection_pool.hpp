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

class Connection;
//class AllConnectionsType;
//class FreeConnectionsType;
//class IdleConnectionsType;
class InUseConnectionsType;
class WaitingRequestsType;
//class HostsCounterType;

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
class ConnectionPool
{
public:
    static ConnectionPoolr* getInstance(boost::asio::io_service& io);
           
    ConnectionPool(boost::asio::io_service& io);
    
    void init(boost::asio::io_service& io);
    
    void asyncGetClientConnection(
        std::string scheme,
        std::string server,
        std::string port,
        ConnectCallbackType cb
    );
    
    void releaseClientConnection(ClientConnection* conn);

private:

    void postSuccess(ConnectCallbackType cb, ClientConnection* conn);
    void postFail(ConnectCallbackType cb, Marvin::ErrorType& ec);
    
    boost::asio::io_service&        io;
    boost::asio::ip::tcp::resolver  resolver_;
    
    std::size_t                     _maxConnections;
//    AllConnectionsType              _all;
//    FreeConnectionsType             _free;
//    IdleConnectionsType             _idle;
    InUseConnectionsType            _inUse;
    WaitingRequestsType             _waitingRequests;
//    HostsCounterType                _hostsCounter;

};




#endif /* connection_pool_hpp */
