//
//  connection_interface.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/21/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <stdio.h>
#include <cassert>
#include "connection_interface.hpp"
#include "http_connection.hpp"
#include "tls_connection.hpp"

ConnectionInterface* connectionFactory(
            boost::asio::io_service& io_service,
            const std::string& scheme,
            const std::string& server,
            const std::string& port
){
    ConnectionInterface* ptr;
    if( scheme == "http" ){
        ptr = new HttpConnection(io_service, scheme, server, port);
    }else if( scheme == "https" ){
        ptr = new TLSConnection(io_service, scheme, server, port);
    } else{
        assert(false);
    }
    return ptr;
}
ConnectionInterface* connectionFactory(
            boost::asio::io_service& io_service,
            const std::string& scheme
){
    ConnectionInterface* ptr;
    if( scheme == "http" ){
        ptr = new HttpConnection(io_service);
    }else if( scheme == "https" ){
        ptr = new TLSConnection(io_service);
    } else{
        assert(false);
    }
    return ptr;
}
