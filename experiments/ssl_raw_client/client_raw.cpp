//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "connection_interface.hpp"
#include "tls_connection.hpp"

#define REQUEST "GET https://ssllabs.com/ HTTP/1.1\r\nHost: www.ssllabs.com\r\n\r\n";
#define MAX_LENGTH 1024
#define DUMP_SERVER_CERT 0
#define DEFAULT_VERIFY_SERVER_CERT 0
class client
{
    public:
        client(
            boost::asio::io_service& io_context,
            boost::asio::ssl::context& context,
            boost::asio::ip::tcp::resolver::iterator endpoints
        ) : socket_(io_context, context)
        {
            socket_.set_verify_mode(boost::asio::ssl::verify_peer);
#if DUMP_SERVER_CERT == 1
            socket_.set_verify_callback(boost::bind(&client::verify_certificate, this, _1, _2));
#endif
            
#if DEFAULT_VERIFY_SERVER_CERT == 1
            SSL_CTX_set_default_verify_file(SSL_CTX *ctx);
            ssl::context ctx(ssl::context::sslv23_client);
            ctx.set_default_verify_paths();
#endif
            boost::asio::async_connect(
                socket_.lowest_layer(),
                endpoints,
                boost::bind(&client::on_connect_complete, this, boost::asio::placeholders::error)
            );
        }

    bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
    {
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.

        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
#if DUMP_SERVER_CERT == 1
        std::cout << "XXVerifying " << subject_name << "\n";
        std::cout << "Preverified " << preverified << std::endl;
        char* fn = (char*)X509_get_default_cert_file();
        char* dn = (char*)X509_get_default_cert_dir();
        char* n1 = (char*)X509_get_default_cert_area();
        char* n2 = (char*)X509_get_default_cert_dir_env();
        char* n3 = (char*)X509_get_default_cert_file_env();
#endif
        return preverified ;
    }

    /**
    * On successful connect initiate TLS handshake
    */
    void on_connect_complete(const boost::system::error_code& error)
    {
        if (!error) {
            socket_.async_handshake(
                boost::asio::ssl::stream_base::client,
                boost::bind(&client::on_handshake_complete, this, boost::asio::placeholders::error)
            );
        }else{
            std::cout << "Connect failed: " << error.message() << "\n";
        }
    }

    void on_handshake_complete(const boost::system::error_code& error)
    {
        if (!error){
//            std::cout << "Enter message: ";
//            std::cin.getline(request_, max_length);
//            const char* tmp = "GET https://ssllabs.com/ HTTP/1.1\r\nHost: www.ssllabs.com\r\n\r\n";
            const char* tmp = REQUEST;
            strcpy(request_, tmp);
            size_t request_length = strlen(request_);

            boost::asio::async_write(
                socket_,
                boost::asio::buffer(request_, request_length),
                boost::bind(
                    &client::on_write_complete,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
            );
        }else{
            std::cout << "Handshake failed: " << error.message() << "\n";
        }
    }
    void after_handshake(const boost::system::error_code& err)
    {
    }
    void on_write_complete(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (!error){
            boost::asio::async_read(
                socket_,
                boost::asio::buffer(reply_, bytes_transferred),
                boost::bind(
                    &client::on_read_complete,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
            );
        }else{
            std::cout << "Write failed: " << error.message() << "\n";
        }
    }

    void on_read_complete(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if( !error ) {
            std::cout << "Reply: ";
            std::cout.write(reply_, bytes_transferred);
            std::cout << "\n";
        }else{
            std::cout << "Read failed: " << error.message() << "\n";
        }
    }

    private:
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
        char request_[MAX_LENGTH];
        char reply_[MAX_LENGTH];
};

int main(int argc, char* argv[])
{
    try
    {

        std::string server("www.ssllabs.com");
        std::string port("https");
        boost::asio::io_service io_context;

        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::query query(server, port);

        boost::asio::ip::tcp::resolver::iterator endpoints = resolver.resolve(query);
        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);

//            SSL_CTX_set_default_verify_file(SSL_CTX *ctx);
//            ssl::context ctx(ssl::context::sslv23);
//            ctx.set_default_verify_paths();

        ctx.load_verify_file("/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/ssl/mozilla.cacert.pem");
        ctx.set_default_verify_paths();
        const char *dir;

        dir = X509_get_default_cert_dir();

        client c(io_context, ctx, endpoints);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
