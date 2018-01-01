//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include "boost_stuff.hpp"
#include <thread>
#include <pthread.h>
#include <catch/catch.hpp>

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "http_server.hpp"
#include "request_handler_base.hpp"

#include "test_server_client/tsc_req_handler.hpp"
#include "server_runner.hpp"
#include "test_server_client/bb/bb_client.hpp"
#include "test_server_client/bb/bb_testcase.hpp"
#include "test_server_client/bb/bb_testcase_defs.hpp"

namespace {
// run each test on a separate io_service - tests that round trip
// handles different buffering of the request and/or handles eof without
// correupting message
void oneShot(const body_buffering::Testcase& testcase)
{
//    const body_buffering::Testcase& testcase = GetParam();
    boost::asio::io_service io_service;
    body_buffering::TClient tst(io_service, "9991", testcase);
    tst.exec();
//    std::cout << testcase._description << std::endl;
    io_service.run();
//    std::cout << "after io.run " << testcase._description << std::endl;
}
void multiple(std::vector<body_buffering::Testcase> tcs)
{
    for(auto const& testcase: tcs) {
        oneShot(testcase);
    }
}
} // namespace

TEST_CASE("oneshot-bodybuffering-timeout","[to]")
{
    /// set short connection timeouts
    TCPConnection::setConfig_connectTimeOut(1000);
    TCPConnection::setConfig_readTimeOut(1000);
    TCPConnection::setConfig_writeTimeOut(1000);
    multiple(body_buffering::make_timeout_cases());
}
/// \warning cannot test eof here as client never gets a response
/// need server to close with EOF
/// too hard to do for the moment
#ifdef RT_TEST_EOF
TEST_CASE("oneshot-bodybuffering-eof","[eof]")
{
    /// set long connection timeouts for debugging
    TCPConnection::setConfig_connectTimeOut(100000);
    TCPConnection::setConfig_readTimeOut(100000);
    TCPConnection::setConfig_writeTimeOut(100000);
    multiple(body_buffering::make_eof_cases());
}
TEST_CASE("EofAll, all", "[eofall]")
{
//    body_buffering::Testcase testcase = tdefs.get_case(1);
    auto cases = body_buffering::make_eof_cases();
    std::vector<body_buffering::TClientSPtr> save;
    for(auto const& testcase: cases) {
        boost::asio::io_service io_service;
        std::shared_ptr<body_buffering::TClient> sp = std::make_shared<body_buffering::TClient>(io_service, "9991", testcase);
        save.push_back(sp);
        sp->exec();
        io_service.run();
//        std::cout << "next one" << std::endl;
    }
//    std::cout << "loop done" << std::endl;
}
#endif
TEST_CASE("oneshot-bodybuffering","[tc]")
{
    multiple(body_buffering::make_test_cases());
}

#if 1
// the client class is too rudimentary for these
// next tests to work
TEST_CASE("BodyBuffering-2-inparallel","[parallel2]")
{
    body_buffering::Testcase testcase1 = body_buffering::make_test_cases()[1];
    body_buffering::Testcase testcase2 = body_buffering::make_test_cases()[2];
    boost::asio::io_service io_service;
    body_buffering::TClient tst1(io_service, "9991", testcase1);
    tst1.exec();
    body_buffering::TClient tst2(io_service, "9991", testcase2);
    tst2.exec();
    io_service.run();
}
#endif
#if 1
TEST_CASE("bodybuffering-all-in-parallel","[parallelall]")
{
//    body_buffering::Testcase testcase = tdefs.get_case(1);
    auto cases = body_buffering::make_test_cases();
    boost::asio::io_service io_service;
    std::vector<body_buffering::TClientSPtr> save;
    for(auto const& testcase: cases) {
        std::shared_ptr<body_buffering::TClient> sp = std::make_shared<body_buffering::TClient>(io_service, "9991", testcase);
        save.push_back(sp);
        sp->exec();
    }
    io_service.run();
}
#endif



