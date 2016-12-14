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
#include "marvin_error.hpp"
#include "server_connection_manager.hpp"
#include "handler_interface.hpp"
#include "request_handler.hpp"
#include "client_connection.hpp"
#include "message_reader.hpp"
#include "message_writer.hpp"


/// The top-level class of the HTTP server.
class Server
{
public:
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit Server(RequestHandlerInterface& handler);

    void listen();
    
private:
    /// Perform an asynchronous accept operation.
    void startAccept();
    void handleAccept(ConnectionHandler* handler, const boost::system::error_code& err);
    void readMessageHandler(Marvin::ErrorType& err);
    
    /// Wait for a request to stop the server.
    void waitForStop();

    boost::asio::io_service         _io;
    boost::asio::signal_set         _signals;
    boost::asio::ip::tcp::acceptor  _acceptor;
    ServerConnectionManager         _connectionManager;
    RequestHandlerInterface&        _requestHandler;
};

#endif // HTTP_SERVER_HPP
