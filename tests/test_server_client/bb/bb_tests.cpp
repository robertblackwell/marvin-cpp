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

#include "test_server_client/tsc_req_handler.hpp"
#include "test_server_client/server_runner.hpp"
#include "test_server_client/bb/bb_client.hpp"
#include "test_server_client/bb/bb_testcase.hpp"
#include "test_server_client/bb/bb_testcase_defs.hpp"

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
    std::cout << "after io.run " << testcase._description << std::endl;
}

//INSTANTIATE_TEST_CASE_P(timeout, Oneshot, testing::ValuesIn(body_buffering::make_timeout_cases()));
INSTANTIATE_TEST_CASE_P(diff_buffering, Oneshot, testing::ValuesIn(body_buffering::make_test_cases()));
INSTANTIATE_TEST_CASE_P(eof, Oneshot, testing::ValuesIn(body_buffering::make_eof_cases()));

#if 1
// the client class is too rudimentary for these
// next tests to work
TEST(BodyBuffering, 01)
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
TEST(BodyBuffering, all)
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
#if 0
TEST(EofAll, all)
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
        std::cout << "next one" << std::endl;
    }
    std::cout << "loop done" << std::endl;
}
#endif



