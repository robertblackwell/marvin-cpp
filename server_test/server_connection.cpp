//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <utility>
#include <vector>
#include "server_connection_manager.hpp"
#include "request_handler.hpp"


ServerConnection::ServerConnection(std::shared_ptr<boost::asio::ip::tcp::socket> socket_sptr,
    ServerConnectionManager& manager, RequestHandlerInterface& handler)
  : socket_sptr_(socket_sptr),
    connection_manager_(manager),
    request_handler_(handler)
{
}

void ServerConnection::start()
{
    startRead();
    //  create socket_wrap
    //  create request_reader(socket_wrap) and response_write(socket_wrap) objects
    //  start request_reader
    //  when request_reader a complete message (v0.0) - finishes headers in the final version
    //      call the handler(req, resp, callback)
    //  wait fo the callback to be invoked
    //      response should be complete, test this and
    //      clean up - dont close socket
    //      if client did not close socket
    //          start again
    
}

void ServerConnection::stop()
{
  socket_sptr_->close();
}
void ServerConnection::onGoodRequest(){
    request_handler_.handle_request(request_, reply_, [this](bool good){
        if( good){
            std::cout << "handler done" << std::endl;
            startWrite();
        }else{
            reply_ = reply::stock_reply(reply::bad_request);
            startWrite();
        }
    });
}
void ServerConnection::onBadRequest(){
    reply_ = reply::stock_reply(reply::bad_request);
    startWrite();
    
}
void ServerConnection::onHandlerDone(){
    
}
void ServerConnection::handleRead(){
    
}
void ServerConnection::startRead()
{
    auto self(shared_from_this());
    socket_sptr_->async_read_some(boost::asio::buffer(buffer_),
      [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred){
          if (!ec){
              
              request_parser::result_type result;
              std::tie(result, std::ignore) = request_parser_.parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);
              
              if (result == request_parser::good){
                  onGoodRequest();
              }else if (result == request_parser::bad){
                  onBadRequest();
              }else{
                  startRead();
              }
          }else if (ec.value() != boost::system::errc::connection_aborted){
              connection_manager_.stop(shared_from_this());
          }
      });
}

void ServerConnection::startWrite()
{
    auto self(shared_from_this());
    boost::asio::async_write(*socket_sptr_, reply_.to_buffers(),
        [this, self](const boost::system::error_code& ec, std::size_t){
            if (!ec){
                // Initiate graceful connection closure.
                boost::system::error_code ignored_ec;
                socket_sptr_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }

            if (ec.value() != boost::system::errc::connection_aborted){
                connection_manager_.stop(shared_from_this());
            }
    });
}

