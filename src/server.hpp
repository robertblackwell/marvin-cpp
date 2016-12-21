//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <signal.h>
#include <utility>

#include "marvin_error.hpp"
#include "server_connection_manager.hpp"
#include "request_handler_interface.hpp"
#include "request_handler.hpp"
#include "connection.hpp"
#include "message_reader.hpp"
#include "message_writer.hpp"
#include "rb_logger.hpp"
#include "connection_handler.hpp"

/// The top-level class of the HTTP server.
template<class H> class Server
{
public:
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit Server();

    void listen();
    
private:
    /// Perform an asynchronous accept operation.
    void startAccept();
    void handleAccept(ConnectionHandler<H>* handler, const boost::system::error_code& err);
    void readMessageHandler(Marvin::ErrorType& err);
    
    /// Wait for a request to stop the server.
    void waitForStop();

    boost::asio::io_service                     _io;
    boost::asio::signal_set                     _signals;
    boost::asio::ip::tcp::acceptor              _acceptor;
    ServerConnectionManager<ConnectionHandler<H>>  _connectionManager;

};
#include "server.cpp"
#endif // HTTP_SERVER_HPP
