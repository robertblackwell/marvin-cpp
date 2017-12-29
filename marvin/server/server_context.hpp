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

#include "boost_stuff.hpp"
#include "marvin_error.hpp"
#include "http_server.hpp"
#include "server_connection_manager.hpp"
#include "request_handler_base.hpp"
#include "i_socket.hpp"
#include "tcp_connection.hpp"
#include "tls_connection.hpp"
#include "connection_handler.hpp"
#endif

class HTTPServer;
class ServerConnectionManager;
class ConnectionHandler;
class ISocket;

struct ServerContext
{
    HTTPServer*                 server_ptr;
    ServerConnectionManager*    server_connection_manager_ptr;
    ConnectionHandler*          connection_handler_ptr;
    ISocket*        connection_ptr;
};

#endif /* server_context_hpp */
