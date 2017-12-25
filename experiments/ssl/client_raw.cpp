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
#include <boost/filesystem.hpp>

using namespace boost::asio;

//#include "i_socket.hpp"
//#include "tls_connection.hpp"

#define REQUEST "GET https://ssllabs.com/ HTTP/1.1\r\nHost: www.ssllabs.com\r\n\r\n";
#define MAX_LENGTH 1024
#define DUMP_SERVER_CERT 1

#define DEFAULT_VERIFY_SERVER_CERT 0

#define CUSTOM_CERTFILE 1
#define CUSTOM_CERTFILE_NAME "/usr/local/ssl/custom_certs.pem"

class client
{
    public:
        client(
            io_service& io_service,
            ssl::context& ssl_ctx,
            ip::tcp::resolver::iterator endpoints
        ) : _socket(io_service, ssl_ctx)
        {
#if DUMP_SERVER_CERT == 1
            /**
            * Set the verify callback - may be turned off when using certificate store
            */
            this->_socket.set_verify_callback(boost::bind(&client::verify_certificate, this, _1, _2));
#endif
            
            async_connect(
                this->_socket.lowest_layer(),
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
        return 0;
        return preverified ;
    }

    /**
    * On successful connect initiate TLS handshake
    */
    void on_connect_complete(const boost::system::error_code& error)
    {
        if (!error) {
            this->_socket.async_handshake(
                ssl::stream_base::client,
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

            async_write(
                this->_socket,
                buffer(request_, request_length),
                boost::bind(
                    &client::on_write_complete,
                    this,
                    placeholders::error,
                    placeholders::bytes_transferred
                )
            );
        }else{
            std::cout << "Handshake failed: " << error.message() << "\n";
        }
    }
    void on_write_complete(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (!error){
            async_read(
                this->_socket,
                buffer(reply_, bytes_transferred),
                boost::bind(
                    &client::on_read_complete,
                    this,
                    placeholders::error,
                    placeholders::bytes_transferred
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
        ::ssl::stream<boost::asio::ip::tcp::socket> _socket;
        char request_[MAX_LENGTH];
        char reply_[MAX_LENGTH];
};

//
// Make a request to www.ssllabds.com with the default cert store - this should work
//
void testHarness(std::string server, std::string certStorePath, bool useDefaultStore)
{
    try
    {

//        std::string server("www.ssllabs.com");
        std::string port("https");
        ::io_service io_service;

        ::ip::tcp::resolver resolver(io_service);
        ::ip::tcp::resolver::query query(server, port);

        ::ip::tcp::resolver::iterator endpoints = resolver.resolve(query);

        ::ssl::context ctx(boost::asio::ssl::context::sslv23_client);
        ctx.set_verify_mode(boost::asio::ssl::verify_peer);
        

        if (useDefaultStore) {
            ctx.set_default_verify_paths();
        } else {
            std::string p = (certStorePath);
            ctx.use_certificate_file(p, ::ssl::context_base::file_format::pem);
        }
        try {
            client c(io_service, ctx, endpoints);
        } catch(std::exception& e1) {
            std::cerr << "Exception: " << e1.what() << "\n";
        }
        io_service.run();
        std::cout << "" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
void testHarness(std::string server)
{
    testHarness(server, "", true);
}
void testHarness(std::string server, std::string certStorePath)
{
    testHarness(server, certStorePath, false);
}

void testSslLabs()
{
    testHarness("www.ssllabs.com");
//    testHarness("www.ssllabs.com", "/Users/rob/MyCurrentProjects/Pixie/MarvibCpp/experiment/ssl/cacert.pem");
//    testHarness("www.ssllabs.com", "/Users/rob/MyCurrentProjects/Pixie/MarvibCpp/experiment/ssl/x_cacert.pem");
}
int main(int argc, char* argv[])
{
    testSslLabs();
    return 0;
}
