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


ISocketSPtr socketFactory(
            bool serverFlag, // true if the socket is for a server
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
){
    ISocketSPtr ptr;
    if( boost::to_lower_copy(scheme) == "http" )
    {
        ptr = std::make_shared<TCPConnection>(io_service, scheme, server, port);
    }
    else if( boost::to_lower_copy(scheme) == "https" )
    {
        auto method = (serverFlag) ? boost::asio::ssl::context::method::sslv23_server : boost::asio::ssl::context::method::sslv23_client;
        boost::asio::ssl::context model_ctx(io_service, method);
        if( serverFlag) {
        } else {
            model_ctx.set_verify_mode(boost::asio::ssl::verify_peer);
            model_ctx.load_verify_file("/Users/rob/CA/allroots/combined-cacert.pem");
//            model_ctx.set_default_verify_paths();
        }
//        ptr = new TCPConnection(io_service, scheme, server, port);
        ptr = std::make_shared<SSLConnection>(io_service, scheme, server, port, std::move(model_ctx));
    }
    else
    {
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
