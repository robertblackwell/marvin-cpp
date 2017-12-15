//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <thread>
#include <pthread.h>
#include <gtest/gtest.h>

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "bb_client.hpp"
#include "bb_testcase.hpp"
#include "tsc_req_handler.hpp"
#include "bb_testcase_defs.hpp"
#include "server_runner.hpp"

namespace {
    /**
    * Parameterized fixture for testing each message stand alone
    * which requires a server to be started
    */
    class Oneshot : public ::testing::Test, public ::testing::WithParamInterface<body_buffering::Testcase>
    {
        public:
        Oneshot(): _tc(GetParam())
        {}
        ~Oneshot(){}
        virtual void SetUp(){}
        virtual void TearDown() {}
        body_buffering::Testcase   _tc;
    };

}
// run each test on a separate io_service - tests that round trip
// handles different buffering of the request and/or handles eof without
// correupting message
TEST_P(Oneshot, 001)
{
    const body_buffering::Testcase& testcase = GetParam();
    boost::asio::io_service io_service;
    body_buffering::TClient tst(io_service, "9991", testcase);
    tst.exec();
    std::cout << testcase._description << std::endl;
    io_service.run();
}

INSTANTIATE_TEST_CASE_P(diff_buffering, Oneshot, testing::ValuesIn(body_buffering::make_test_cases()));
INSTANTIATE_TEST_CASE_P(eof, Oneshot, testing::ValuesIn(body_buffering::make_eof_cases()));

#if 0
TEST(BodyBuffering, 01)
{
    body_buffering::Testcase testcase = body_buffering::make_test_cases()[1];
    boost::asio::io_service io_service;
    body_buffering::TClient tst(io_service, "9991", testcase);
    tst.exec();
    io_service.run();
}
#if 1
TEST(BodyBuffering, all)
{
//    body_buffering::Testcase testcase = tdefs.get_case(1);
    auto cases = body_buffering::make_test_cases();
    boost::asio::io_service io_service;
    for(auto const& testcase: cases) {
        body_buffering::TClient tst(io_service, "9991", testcase);
        tst.exec();
    }
    io_service.run();
}
TEST(EofAll, all)
{
//    body_buffering::Testcase testcase = tdefs.get_case(1);
    auto cases = body_buffering::make_test_cases();
    boost::asio::io_service io_service;
    for(auto const& testcase: cases) {
        body_buffering::TClient tst(io_service, "9991", testcase);
        tst.exec();
    }
    io_service.run();
}
#endif
#endif

int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
#define EX_RUNNER
#ifdef EX_RUNNER
    ServerRunner<TscRequestHandler> s_runner;
    s_runner.setup();
#endif
    char* _argv[2] = {argv[0], (char*)"--gtest_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    auto res = RUN_ALL_TESTS();
#ifdef EX_RUNNER
    sleep(2);
    s_runner.teardown();
#endif
    return res;
}
