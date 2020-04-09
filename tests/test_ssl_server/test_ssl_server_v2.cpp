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
#include <boost/filesystem.hpp>
#include <cert/cert_authority.hpp>      // for AuthoritySPtr
#include <cert/cert_certificate.hpp>    // for Certificate
#include <cert/cert_identity.hpp>       // for Identity
#include <cert/cert_store_locator.hpp>  // for LocatorSPtr
#include <cert/cert_store_store.hpp>    // for StoreSPtr

#include <marvin/certificates/certificates.hpp>

using boost::asio::ip::tcp;
using namespace boost::asio;
using namespace boost::asio::ssl;
using namespace boost::filesystem;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket)
        : m_tcp_socket(std::move(socket))
    {
    }

    void start()
    {
        become_secure();
        do_handshake();
    }

private:
    void do_handshake()
    {
        auto self(shared_from_this());
        m_ssl_stream_sptr->async_handshake(ssl::stream_base::server, [this, self](const boost::system::error_code& error)
        {
            if (!error) {
                do_read();
            }
        });
    }

    void do_read()
    {
        auto self(shared_from_this());
        m_ssl_stream_sptr->async_read_some(buffer(m_data), [this, self](const boost::system::error_code& ec, std::size_t length)
        {
            if (!ec) {
                std::string request(&(this->m_data[0]), length);
                do_write(length);
            }
        });
    }

    void do_write(std::size_t length)
    {
        m_reply_message = "HTTP/1.1 200 OK \r\nContent-length: 10\r\nConnection: close\r\n\r\n0123456789";
        auto self(shared_from_this());
        boost::asio::async_write(
            *m_ssl_stream_sptr, buffer(m_reply_message.c_str(), m_reply_message.size()), [this, self](const boost::system::error_code& ec, std::size_t /*length*/)
        {
            if (!ec) {
                do_read();
            }
        });
    }
    void become_secure()
    {
        m_ssl_context_sptr = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::method::sslv23);
        m_ssl_context_sptr->set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);
        SSL_CTX* ssl_ctx_ptr = m_ssl_context_sptr->native_handle();
        path cert_path{"/home/robert/Projects/marvin++/tests/test_ssl_server/server.pem"};
        path key_path{"/home/robert/Projects/marvin++/tests/test_ssl_server/server.pem"};
        Cert::Certificate server_cert{cert_path};
        EVP_PKEY* pkey_ptr = Cert::x509::PKey_ReadPrivateKeyFrom(key_path.string(), "test");
        SSL_CTX_use_PrivateKey(ssl_ctx_ptr, pkey_ptr);
        SSL_CTX_use_certificate(ssl_ctx_ptr, server_cert.native());
#if 0
        ssl::stream<ip::tcp::socket&> tmp_strm(m_tcp_socket, *m_ssl_context_sptr);
        ssl::stream<ip::tcp::socket&>* tmp_strm_ptr = new ssl::stream<ip::tcp::socket&>(m_tcp_socket, *m_ssl_context_sptr);
#else
        m_ssl_stream_sptr = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>>(m_tcp_socket, *m_ssl_context_sptr);
#endif
    }
    boost::asio::ip::tcp::socket m_tcp_socket;
    std::shared_ptr<boost::asio::ssl::context> m_ssl_context_sptr;
    std::shared_ptr<boost::asio::ssl::stream<tcp::socket&>> m_ssl_stream_sptr;

    char m_data[1024];
    std::string m_reply_message;
};

class server
{
public:
    server(boost::asio::io_context& io_context, unsigned short port)
            : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    std::string get_password() const
    {
        return "test";
    }

    void do_accept()
    {
        m_acceptor.async_accept([this](const boost::system::error_code& error, tcp::socket socket)
        {
            std::cout << __PRETTY_FUNCTION__  << std::endl;
            if (!error) {
                auto session_sptr = std::make_shared<session>(std::move(socket));
                session_sptr->start();
            }
            do_accept();
        });
    }
    tcp::acceptor m_acceptor;
};

int main(int argc, char* argv[])
{
    try {
        int port = 9992;
        boost::asio::io_context io_context;

        using namespace std; // For atoi.
        server s(io_context, port);

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
