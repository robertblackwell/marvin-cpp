//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <cstdlib>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/message_factory.hpp>
#include <marvin/client/client.hpp>
#include <marvin/forwarding//forward_helpers.hpp>
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/certificates/certificates.hpp>

using namespace Marvin;
using namespace Marvin::Http;

namespace {

/// \brief makes a GET request to the url provided and expects status=200/302 and a non empty content
    MessageBaseSPtr  makeRequest(std::string request_uri) {
        MessageBaseSPtr msg = std::shared_ptr<Marvin::Http::MessageBase>(new Marvin::Http::MessageBase());
#if 1
        Marvin::Uri uri(request_uri);
#else
        Uri uri("https://www.ssllabs.com:443");
#endif
        ::Marvin::Http::makeRequest(*msg, HttpMethod::GET, uri);
        msg->header(HeadersV2::Host, std::string("www.ssllabs.com"));//uri.host());
        msg->header(Marvin::Http::HeadersV2::Connection, Marvin::Http::HeadersV2::ConnectionClose);
        msg->header(Marvin::Http::HeadersV2::AcceptEncoding, "identity");
        msg->header(Marvin::Http::HeadersV2::TE, "");
        // Http versions defaults to 1.1, so force it to the same as the request
        msg->setContent("");
        return msg;
    }
} // namespace
#if 0
TEST_CASE("connect_failBadHost")
{
    boost::asio::io_service io;
    auto conn_sptr = std::make_shared<Connection>(io, "https", "ddddgoogle.com", "443");
    conn_sptr->asyncConnect([](Marvin::ErrorType& err, ISocket* conn)
    {
        INFO(Marvin::make_error_description(err));
//        auto x1 = boost::system::errc::host_unreachable;
//        auto y = err.default_error_condition();
//        auto b1 = (x1 == y);
//        auto b2 = (err == x1);
        auto not_found = boost::asio::error::make_error_code(boost::asio::error::netdb_errors::host_not_found);
        auto not_found_try_again = boost::asio::error::make_error_code(boost::asio::error::netdb_errors::host_not_found_try_again);
        {
        auto res = ( (err == not_found) || (err == not_found_try_again));
        CHECK( res);
        }
    });
    io.run();
}
void dofail(){
    CHECK(true);
}
#if 1
TEST_CASE("connect_succeed")
{
    INFO("connect_succeed");
    boost::asio::io_service io;
    auto conn_sptr = std::make_shared<Connection>(io, "https", "google.com", "443");
    conn_sptr->asyncConnect([](Marvin::ErrorType& err, ISocket* conn)
    {
        INFO(Marvin::make_error_description(err));
        REQUIRE(! err);
    });
    io.run();
}
#endif
#if 1
TEST_CASE("ssl_connect")
{
    boost::asio::io_service io;
//    ISocketSPtr conn_sptr = socketFactory(false, io, "https", "bankofamerica.com", "443");
    auto conn_sptr = std::make_shared<Connection>(io, "https", "google.com", "443");
    conn_sptr->asyncConnect([](Marvin::ErrorType& err, ISocket* conn)
    {
        INFO(Marvin::make_error_description(err));
        REQUIRE(! err);
    });
    io.run();

}
#endif
TEST_CASE("new_connection")
{
    // test can go secure AFTER connect
    boost::asio::io_service io;
    boost::asio::ssl::context ctx(boost::asio::ssl::context::method::sslv23);
    if (!boost::filesystem::is_regular("/usr/local/etc/openssl@1.1/cert.pem")) {
        assert(false);
    }
    std::string default_bundle_path = "/usr/local/etc/openssl@1.1/cert.pem"; 
    X509_STORE *store = X509_STORE_new();
    X509_STORE_load_locations(store, (const char*)default_bundle_path.c_str(), NULL);
    X509_STORE_up_ref(store);
    // attach X509_STORE to boost ssl context
    // SSL_CTX_set_cert_store(ctx.native_handle(), store);

//    ConnectionSPtr conn_sptr( new Connection(io, "https", "bankofamerica.com", "443", std::move(ctx)));
    ConnectionSPtr conn_sptr( new Connection(io, "https", "bankofamerica.com", "443"));
    conn_sptr->asyncConnect([conn_sptr, store](Marvin::ErrorType& err, ISocket* conn) {
        INFO(Marvin::make_error_description(err));
        REQUIRE(! err);
        conn_sptr->becomeSecureClient(store);
        conn_sptr->asyncHandshake([conn_sptr](const boost::system::error_code& err) {
            std::cout << "hello" << std::endl;
            INFO(Marvin::make_error_description(err));
            REQUIRE(conn_sptr->getServerCertificate());
            REQUIRE(!err);
        });
    });
    io.run();
}
#endif
TEST_CASE("new_connection_connect")
// test can go secure BEFORE connect
{
    boost::asio::io_service io;
    boost::asio::ssl::context ctx(boost::asio::ssl::context::method::sslv23);
    if (!boost::filesystem::is_regular("/usr/local/etc/openssl@1.1/cert.pem")) {
        std::cout << "DID NOT FIND /usr/local/etc/openssl@1.1/cert.pem" << std::endl;
        assert(false);
    }
    std::string default_bundle_path = "/usr/local/etc/openssl@1.1/cert.pem"; 
    X509_STORE *store = X509_STORE_new();
    X509_STORE_load_locations(store, (const char*)default_bundle_path.c_str(), NULL);
    X509_STORE_up_ref(store);
    ConnectionSPtr conn_sptr( new Connection(io, "https", "bankofamerica.com", "443"));
    conn_sptr->becomeSecureClient(store);
    conn_sptr->asyncConnect([conn_sptr, store](Marvin::ErrorType& err, ISocket* conn) {
        INFO(Marvin::make_error_description(err));
        REQUIRE(! err);
        std::cout << "hello" << std::endl;
        INFO(Marvin::make_error_description(err));
        REQUIRE(conn_sptr->getServerCertificate());
        REQUIRE(!err);
    });
    io.run();

}

TEST_CASE("new_connection_GET")
// test can go secure BEFORE connect
{
    boost::asio::io_service io;
//    boost::asio::ssl::context ctx(boost::asio::ssl::context::method::sslv23);
//    if (!boost::filesystem::is_regular("/usr/local/etc/openssl@1.1/cert.pem")) {
//        assert(false);
//    }
//    std::string default_bundle_path = "/usr/local/etc/openssl@1.1/cert.pem";
//    X509_STORE *store = X509_STORE_new();
//    X509_STORE_load_locations(store, (const char*)default_bundle_path.c_str(), NULL);
//    X509_STORE_up_ref(store);
    Certificates certs = Certificates::getInstance();
    X509_STORE* store = certs.getX509StorePtr();
//    ConnectionSPtr conn_sptr(new Connection(io, "https", "www.ssllabs.com", "443"));
    ConnectionSPtr conn_sptr = std::dynamic_pointer_cast<Connection>(socketFactory(io, "https", "www.ssllabs.com", "443"));
    conn_sptr->becomeSecureClient(store);
    ClientSPtr client;
    conn_sptr->asyncConnect([conn_sptr, &client, &io](Marvin::ErrorType& err, ISocket* conn) {
        INFO(Marvin::make_error_description(err));
        REQUIRE(! err);
        std::cout << "hello" << std::endl;
        INFO(Marvin::make_error_description(err));
        REQUIRE(conn_sptr->getServerCertificate());
        REQUIRE(!err);
        client = std::make_shared<Client>(io, conn_sptr);
        MessageBaseSPtr msg = makeRequest(std::string("https://www.ssllabs.com:443"));
        std::string s = msg->str();
        client->asyncWrite(msg, [](Marvin::ErrorType& err, MessageReaderSPtr reader){
            std::cout << "got response" << std::endl;
            std::cout << reader->str() << std::endl;
            REQUIRE(reader->status_code() == 200);
            REQUIRE(reader->status() == "OK");
            std::cout << "Body:" << std::endl << reader->getContent()->to_string() << std::endl;
        });
        std::cout << "hello" << std::endl;
    });
    io.run();
}
