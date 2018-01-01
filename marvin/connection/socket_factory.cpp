//
//  i_socket.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/21/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <stdio.h>
#include <cassert>
#include "i_socket.hpp"
#include "tcp_connection.hpp"
#include "tls_connection.hpp"

ISocket* socketFactory(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
){
    ISocket* ptr;
    if( boost::to_lower_copy(scheme) == "http" ){
        ptr = new TCPConnection(io_service, scheme, server, port);
    }else if( boost::to_lower_copy(scheme) == "https" ){
        ptr = new TCPConnection(io_service, scheme, server, port);
//        ptr = new TLSConnection(io_service, scheme, server, port);
    } else{
        assert(false);
    }
    return ptr;
}
ISocket* socketFactory(
            boost::asio::io_service& io_service,
            const std::string scheme
){
    ISocket* ptr;
    if( scheme == "http" ){
        ptr = new TCPConnection(io_service);
    }else if( scheme == "https" ){
        ptr = new TCPConnection(io_service);
//        ptr = new TLSConnection(io_service);
    } else{
        assert(false);
    }
    return ptr;
}
//ISocket::~ISocket(){}
