//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include "server.hpp"
#include <signal.h>
#include <utility>

Server::Server(RequestHandlerInterface& handler)
  : io_context_(1),
    signals_(io_context_),
    acceptor_(io_context_),
    connection_manager_(),
    request_handler_(handler)
{
    // Register to handle the signals that indicate when the Server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    #if defined(SIGQUIT)
    signals_.add(SIGQUIT);
    #endif // defined(SIGQUIT)

    waitForStop();
    
// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
//  boost::asio::ip::tcp::resolver resolver(io_context_);
//  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
//  acceptor_.open(endpoint.protocol());
//  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
//  acceptor_.bind(endpoint);
//  acceptor_.listen();

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 9991);
    
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
}
void Server::listen(){
    acceptor_.listen();
    startAccept();
    io_context_.run();
    
}
void Server::startAccept(){
    // new socket for the next connection
    auto sock_sptr = std::shared_ptr<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket(io_context_));
        
    acceptor_.async_accept(
       *sock_sptr,
       [this, sock_sptr](const boost::system::error_code& ec){
           if (!acceptor_.is_open()){
               return;
           }
           if (!ec){
               std::cout << "got a connection" << std::endl;
               handleNewConnection(sock_sptr);
           }else{
               std::cout << ec.value() << " " << ec.category().name() << " " << ec.message()  << std::endl;
           }
           startAccept();
        });
    
}
void Server::handleNewConnection(std::shared_ptr<boost::asio::ip::tcp::socket> sp){

    std::shared_ptr<Connection> conn_sptr = std::shared_ptr<Connection>(new Connection(sp, connection_manager_, request_handler_));

    connection_manager_.start(conn_sptr);
    
    //
    //          connection_manager_.start(std::make_shared<connection>(
    //              std::move(socket), connection_manager_, request_handler_));
}

void Server::waitForStop()
{
  signals_.async_wait(
                      [this](boost::system::error_code /*ec*/, int /*signo*/)
      {
        // The Server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_context::run()
        // call will exit.
        acceptor_.close();
//        connection_manager_.stop_all();
      });
}


