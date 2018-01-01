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
#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>
#include "boost_stuff.hpp"
#include "tcp_connection.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "http_server.hpp"
#include "request_handler_base.hpp"

TEST_CASE("connect_failBadHost", "")
{
    boost::asio::io_service io;
    auto conn_sptr = std::make_shared<TCPConnection>(io, "https", "ddddgoogle.com", "443");
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
TEST_CASE("connect_failTimeout", "")
{
    std::cout << "START::connect_failTimeout" << std::endl;
    boost::asio::io_service io;
    auto conn_sptr = std::make_shared<TCPConnection>(io, "http", "localhost", "3333");
    conn_sptr->asyncConnect([](Marvin::ErrorType& err, ISocket* conn)
    {
        INFO(Marvin::make_error_description(err));
        REQUIRE( err == boost::system::errc::connection_refused );
    });
    io.run();
    dofail();
    std::cout << "END::connect_failTimeout" << std::endl;
}

TEST_CASE("connect_succeed","")
{
    INFO("connect_succeed");
    boost::asio::io_service io;
    auto conn_sptr = std::make_shared<TCPConnection>(io, "https", "google.com", "443");
    conn_sptr->asyncConnect([](Marvin::ErrorType& err, ISocket* conn)
    {
        INFO(Marvin::make_error_description(err));
        REQUIRE(! err);
    });
    io.run();
}

int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
    char* _argv[2] = {argv[0], (char*)"-r tap"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    printf("connect\n");
    int result = Catch::Session().run( argc, argv );
    printf("connect\n");
    return result;
}

