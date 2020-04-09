//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <functional>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket, boost::asio::ssl::context& context)
    : socket_(std::move(socket), context)
  {
  }

  void start()
  {
    do_handshake();
  }

private:
  void do_handshake()
  {
    auto self(shared_from_this());
    socket_.async_handshake(boost::asio::ssl::stream_base::server, 
        [this, self](const boost::system::error_code& error)
        {
          if (!error)
          {
            do_read();
          }
        });
  }

  void do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(m_data),
        [this, self](const boost::system::error_code& ec, std::size_t length)
        {
          if (!ec)
          {
            std::string request(&(this->m_data[0]), length);
            do_write(length);
          }
        });
  }

  void do_write(std::size_t length)
  {
    m_reply_message = "HTTP/1.1 200 OK \r\nContent-length: 10\r\nConnection: close\r\n\r\n0123456789";
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(m_reply_message.c_str(), m_reply_message.size()),
        [this, self](const boost::system::error_code& ec,
          std::size_t /*length*/)
        {
          if (!ec)
          {
            do_read();
          }
        });
  }

  boost::asio::ssl::stream<tcp::socket> socket_;
  char m_data[1024];
  std::string m_reply_message;
};

class server
{
public:
  server(boost::asio::io_context& io_context, unsigned short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      context_(boost::asio::ssl::context::sslv23)
  {
    context_.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    context_.set_password_callback(std::bind(&server::get_password, this));
    context_.use_certificate_chain_file("/home/robert/Projects/marvin++/tests/test_ssl_server/server.pem");
    context_.use_private_key_file("/home/robert/Projects/marvin++/tests/test_ssl_server/server.pem", boost::asio::ssl::context::pem);
    context_.use_tmp_dh_file("/home/robert/Projects/marvin++/tests/test_ssl_server/dh2048.pem");

    do_accept();
  }

private:
  std::string get_password() const
  {
    return "test";
  }

  void do_accept()
  {
    acceptor_.async_accept(
        [this](const boost::system::error_code& error, tcp::socket socket)
        {
          std::cout << __PRETTY_FUNCTION__  << std::endl;
          if (!error)
          {
            std::make_shared<session>(std::move(socket), context_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  boost::asio::ssl::context context_;
};

int main(int argc, char* argv[])
{
  try
  {
    int port = 9992;
    boost::asio::io_context io_context;

    using namespace std; // For atoi.
    server s(io_context, port);

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
