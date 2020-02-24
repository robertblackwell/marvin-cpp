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
    auto xx = boost::to_lower_copy(scheme);
    if( boost::to_lower_copy(scheme) == "http" )
    {
        ptr = std::make_shared<TCPConnection>(io_service, scheme, server, port);
    }
    else if( boost::to_lower_copy(scheme) == "https" )
    {
        auto method = (serverFlag) ? boost::asio::ssl::context::method::sslv23_server : boost::asio::ssl::context::method::sslv23_client;
         boost::asio::ssl::context model_ctx(method);
/*
using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;

// Create a context that uses the default paths for
// finding CA certificates.
ssl::context ctx(ssl::context::sslv23);
ctx.set_default_verify_paths();

// Open a socket and connect it to the remote host.
boost::asio::io_context io_context;
ssl_socket sock(io_context, ctx);
tcp::resolver resolver(io_context);
tcp::resolver::query query("host.name", "https");
boost::asio::connect(sock.lowest_layer(), resolver.resolve(query));
sock.lowest_layer().set_option(tcp::no_delay(true));

// Perform SSL handshake and verify the remote host's
// certificate.
sock.set_verify_mode(ssl::verify_peer);
sock.set_verify_callback(ssl::rfc2818_verification("host.name"));
sock.handshake(ssl_socket::client);
*/

        if( serverFlag) {
        } else {
            model_ctx.set_verify_mode(boost::asio::ssl::verify_peer);
            if (!boost::filesystem::is_regular("/usr/local/etc/openssl@1.1/cert.pem")) {
                assert(false);
            }
            model_ctx.load_verify_file("/usr/local/etc/openssl@1.1/cert.pem");
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
