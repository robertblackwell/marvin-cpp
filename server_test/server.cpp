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
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "marvin_error.hpp"
#include "client_connection.hpp"
#include "connection_handler.hpp"
#include "read_socket_interface.hpp"
#include "message_reader.hpp"
#include "message_writer.hpp"

Server::Server(RequestHandlerInterface& handler)
  : _io(1),
    _signals(_io),
    _acceptor(_io),
    _connectionManager(),
    _requestHandler(handler)
{
    // Register to handle the signals that indicate when the Server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    _signals.add(SIGINT);
    _signals.add(SIGTERM);
    #if defined(SIGQUIT)
    _signals.add(SIGQUIT);
    #endif // defined(SIGQUIT)

    waitForStop();
    
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 9991);
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
}
void Server::listen(){
    _acceptor.listen();
    startAccept();
    _io.run();
    
}
void Server::readMessageHandler(Marvin::ErrorType& err)
{
}
void Server::handleAccept(ConnectionHandler* connHandler, const boost::system::error_code& err)
{
    if (! _acceptor.is_open()){
        delete connHandler;
        LogWarn("Accept is not open ???? WTF - lets TERM the server");
        return; // something is wrong
    }
    if (!err){
        LogDebug("got a connection", connHandler->nativeSocketFD());
        
        _connectionManager.registerConnectionHandler(connHandler);
        connHandler->serve();
    }else{
        LogWarn("Accept error value:",err.value()," cat:", err.category().name(), "message: ",err.message());
        delete connHandler;
    }
    startAccept();
    
}
void Server::startAccept()
{
    ClientConnection* conptr = new ClientConnection(_io);
    ConnectionHandler* connectionHandler =
        new ConnectionHandler(_io, _connectionManager, _requestHandler, conptr);
    
    boost::asio::ip::tcp::socket& sock_ref = conptr->getSocketRef();

    auto hf = std::bind(&Server::handleAccept, this, connectionHandler, std::placeholders::_1);
    _acceptor.async_accept(sock_ref, hf);
}
void Server::waitForStop()
{
  _signals.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/)
      {
        // The Server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_context::run()
        // call will exit.
        _acceptor.close();
//        connection_manager_.stop_all();
      });
}


