//
//  server_context.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/10/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef server_context_hpp
#define server_context_hpp

#if 0
#include <string>
#include <signal.h>
#include <utility>

#include <marvin/boost_stuff.hpp>
#include <marvin/error/marvin_error.hpp>
#include <marvin/server/http_server.hpp>
#include <marvin/server/server_connection_manager.hpp>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/connection/tcp_connection.hpp>
#include <marvin/connection/tls_connection.hpp>
#include <marvin/server/connection_handler.hpp>
#endif

// TAKE CARE - this object is only for information purposes and the values in this object are only ever traced
// no destructor as the refs are weak.

class HTTPServer;
class ServerConnectionManager;
class ConnectionHandler;
class ISocket;

struct ServerContext
{
    HTTPServer*                 server_ptr;
    ServerConnectionManager*    server_connection_manager_ptr;
    ConnectionHandler*          connection_handler_ptr;
    ISocket*        			connection_ptr;
};

#endif /* server_context_hpp */
