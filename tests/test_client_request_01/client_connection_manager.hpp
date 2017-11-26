//
//  ConnectionManager.hpp
//  asio-mitm
//
//  Created by ROBERT BLACKWELL on 11/20/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef connection_manager_hpp
#define connection_manager_hpp
#include <stdio.h>

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include "boost_stuff.hpp"
#include "callback_typedefs.hpp"

class Connection;

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
//      req = make_conn_request(host_id, callback)
//      if free.size() > 0
//          if host_counter(host_id) < max_per_host
//              conn = free.allocateTo(req.host_id)
//              conn.host_id = req.host_id
//              in_use.add(conn, host_id)
//              host_counter.incrementFor(host_id)
//              req.callback(conn)
//          else
//              waitingRequests.add(req)
//      else
//          if idle.size() == 0
//              waitingRequests.add(req)
//          else
//              conn = idle.findForHost(req.host_id)
//              if not found
//                  conn = idle.highjackOldestFor(req.host_id)
//              conn.host_id = req.host_id
//              in_use.add(conn, host_id);
//              host_counter.incrementFor(host_id)
//              req.callback(conn)
//
//  release_connection(conn)
//      idle.add(conn)
//      in_use.remove(conn, conn.host_id)
//      host_counter.decrementFor(conn.host_id)
//      req = waitingRequest.removeOldest()
//      get_a_connection(req.host_id, req.callback)
//
class ConnectionManager
{
public:
    static ConnectionManager* getInstance(boost::asio::io_service& io);
           
    ConnectionManager(boost::asio::io_service& io);
    
    void init(boost::asio::io_service& io);
    
    void asyncGetConnection(
        std::string scheme,
        std::string server,
        std::string port,
        ConnectCallbackType cb
    );
    
    void releaseConnection(Connection* conn);

private:

    void postSuccess(ConnectCallbackType cb, Connection* conn);
    void postFail(ConnectCallbackType cb, Marvin::ErrorType& ec);
    
    boost::asio::io_service&        io;
    boost::asio::ip::tcp::resolver  resolver_;

};

#endif /* ConnectionManager_hpp */
