//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/*
#include <iostream>
#include "server.hpp"
#include <signal.h>
#include <utility>
#include "rb_logger.hpp"
#include "marvin_error.hpp"
#include "connection.hpp"
#include "connection_handler.hpp"
#include "read_socket_interface.hpp"
#include "message_reader.hpp"
#include "message_writer.hpp"
*/

template<class TRequestHandler>
Server<TRequestHandler>::Server()
  : _io(1),
    _signals(_io),
    _acceptor(_io),
    _serverStrand(_io),
    _connectionManager()
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
//-------------------------------------------------------------------------------------
// postOnStrand - wraps a parameterless function in _strand and posts to _io
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void Server<TRequestHandler>::postOnStrand(std::function<void()> fn)
{
    auto wrappedFn = _serverStrand.wrap(fn);
    _io.post(wrappedFn);
}
//-------------------------------------------------------------------------------------
// listen - starts the accept cycle
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void Server<TRequestHandler>::listen()
{
    _acceptor.listen();
    
    // start the accept process on the _serverStrand
    auto hf = std::bind(&Server<TRequestHandler>::startAccept, this);
    postOnStrand(hf);
    _io.run();
    
}
//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void Server<TRequestHandler>::readMessageHandler(Marvin::ErrorType& err)
{
}
//-------------------------------------------------------------------------------------
// handleAccept - called on _strand to handle a new client connection
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void Server<TRequestHandler>::handleAccept(
                                                                ConnectionHandler<TRequestHandler>* connHandler,
                                                                const boost::system::error_code& err)
{
    if (! _acceptor.is_open()){
        delete connHandler;
        LogWarn("Accept is not open ???? WTF - lets TERM the server");
        return; // something is wrong
    }
    if (!err){
        LogDebug("got a connection", connHandler->nativeSocketFD());
        
        _connectionManager.registerConnectionHandler(connHandler);
        //
        // at this point we are running on _serveStrand start the connectionHandler with a post to
        // liberate it from the strand
        //
        auto hf = std::bind(&ConnectionHandler<TRequestHandler>::serve, connHandler);
        _io.post(hf);
//        _io.post(connHandler->serve());
    }else{
        LogWarn("Accept error value:",err.value()," cat:", err.category().name(), "message: ",err.message());
        delete connHandler;
    }
    startAccept();
    
}
//-------------------------------------------------------------------------------------
// startAccept
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void Server<TRequestHandler>::startAccept()
{
    ConnectionInterface* conptr = new HttpConnection(_io);
    
    RequestHandlerInterface* hi = new TRequestHandler();
    
    ConnectionHandler<TRequestHandler>* connectionHandler =
        new ConnectionHandler<TRequestHandler>(_io, _connectionManager, conptr);

    auto hf = _serverStrand.wrap(
                    std::bind(&Server::handleAccept, this, connectionHandler, std::placeholders::_1)
                    );
    conptr->asyncAccept(_acceptor, hf);
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void Server<TRequestHandler>::waitForStop()
{
    LogDebug("");
    auto hf = _serverStrand.wrap(
                    std::bind(&Server::doStop, this, std::placeholders::_1)
                    );

  _signals.async_wait(hf);
}
template<class TRequestHandler> void Server<TRequestHandler>::doStop(const Marvin::ErrorType& err)
{
    LogDebug("");
    _acceptor.close();
//  connection_manager_.stop_all();
}

