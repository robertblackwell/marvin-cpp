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
#include "connection.hpp"
#include "connection_manager.hpp"
#include "handler_interface.hpp"
#include "request_handler.hpp"


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
    
    void handleNewConnection(std::shared_ptr<boost::asio::ip::tcp::socket> sp);
    
    /// Wait for a request to stop the server.
    void waitForStop();

    /// The io_context used to perform asynchronous operations.
    boost::asio::io_service io_context_;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set signals_;


    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    /// The connection manager which owns all live connections.
    ConnectionManager connection_manager_;

    /// The handler for all incoming requests.
    RequestHandlerInterface& request_handler_;

};

#endif // HTTP_SERVER_HPP
