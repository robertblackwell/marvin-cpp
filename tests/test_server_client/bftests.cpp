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
#include <catch2/catch.hpp>

#include<marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_base.hpp>
#include "test_server_client/bf/bf_post.hpp"
#include "test_server_client/bf/bf_pipeline.hpp"
#include "test_server_client/bf/bf_testcase.hpp"
#include "test_server_client/tsc_req_handler.hpp"
#include "server_runner.hpp"
using namespace body_format;

namespace {

std::vector<body_format::Testcase> make_cases()
{
    std::vector<body_format::Testcase> cases = {
        /**
        * Testcases
        */
        body_format::Testcase(
                body_format::TestType::WITH_STRING,
                "With string - 2 buffers 00",
                {
                    "12345987654",
                    "93hdiuybk"
                }
        ),
        body_format::Testcase(
                body_format::TestType::WITH_MBUFFER,
                "With mbuffer - 2 buffers 01",
                {
                    "1234567890",
                    "oiuhgre76",
                    "HGYTRESAWQ"
                }
        ),
        body_format::Testcase(
                body_format::TestType::WITH_BUFFER_CHAIN,
                "With buffer chain - 2 buffers 03",
                {
                    "1234567890",
                    "1m2j3k4i5u6",
                    "qkjgtaitsko",
                    "2"
                }
        )
    };
    return cases;
}

} // namespace

// run each test on a separate io_service
void oneShot( const Testcase& testcase)
{
    boost::asio::io_service io_service;
    body_format::PostTest tst(io_service, testcase);
    tst.exec();
//    std::cout << testcase._description << std::endl;
    io_service.run();
}
void multiple(std::vector<Testcase> tcs)
{
    for(auto const& testcase: tcs) {
        oneShot(testcase);
    }
}
#if 1
TEST_CASE("Oneshots1","")
{
    oneShot(make_cases()[0]);
}
#endif
#if 1
TEST_CASE("Oneshots","")
{
    multiple(make_cases());
}
#endif

#if 1
// run all request on a single io_service at the same time
// but all as separate request streams and separate connections
TEST_CASE("Multiple-alltogether","")
{
    boost::asio::io_service io_service;
    std::vector<body_format::Testcase> cases = make_cases();
    std::vector<std::shared_ptr<body_format::PostTest>> saved;
    for(const body_format::Testcase& c : cases) {
        std::shared_ptr<body_format::PostTest> texec = std::shared_ptr<body_format::PostTest>(new body_format::PostTest(io_service, c));
        saved.push_back(texec);
        texec->exec();
    }
    io_service.run();
}
#endif
#if 1
TEST_CASE("Multiple_pipeline","")
{
    boost::asio::io_service io_service;
    std::vector<body_format::Testcase> cases = make_cases();
    body_format::PipelineTest tst(io_service, cases);
    tst.exec();
    io_service.run();
}
#endif
