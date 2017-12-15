//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>

#include <gtest/gtest.h>

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "bf_post.hpp"
#include "bf_pipeline.hpp"
#include "bf_testcase.hpp"
#include "bf_req_handler.hpp"

#if 0
using namespace body_format;

static Testcase tcase01(
        TestType::WITH_STRING,
        "With string - 2 buffers",
        {
            "12345987654",
            "93hdiuybk"
        }
    );
static Testcase  tcase02(
        TestType::WITH_MBUFFER,
        "With mbuffer - 2 buffers",
        {
            "1234567890",
            "oiuhgre76",
            "HGYTRESAWQ"
        }
    );
static Testcase tcase03(
        TestType::WITH_BUFFER_CHAIN,
        "With buffer chain - 2 buffers",
        {
            "1234567890",
            "1m2j3k4i5u6",
            "qkjgtaitsko",
            "2"
        }
    );
void runTestClient();
void test_one(Testcase& testcase);
void test_multiple(std::vector<Testcase> cases);

template<class TESTEXEC, class TESTCASE>
void test_one(TESTCASE testcase)
{
    boost::asio::io_service io_service;
    TESTEXEC tst(io_service, testcase);
    tst.exec();
    io_service.run();
}
template<class TESTEXEC, class TESTCASE>
void test_multiple(std::vector<TESTCASE> cases)
{
    boost::asio::io_service io_service;
    
    std::vector<std::shared_ptr<TESTEXEC>> saved;
    for(TESTCASE& c : cases) {
        std::shared_ptr<TESTEXEC> texec = std::shared_ptr<TESTEXEC>(new TESTEXEC(io_service, c));
        saved.push_back(texec);
        texec->exec();
    }
    io_service.run();
}
#if 1
TEST(PostOne, One)
{
    test_one<PostTest, Testcase>(tcase01);
}
#endif
#if 0
// tests body parsing
// sends request with using different forms of body data structure
TEST_CASE("PostTest_consec", "[server, body, consecutive]")
{
    std::vector<Testcase> tcs = {tcase01, tcase02, tcase03};
    for(auto & c : tcs) {
        test_one<PostTest, Testcase>(c);
    }
}
#endif
#if 0
// tests simultaneous operation of server and reuqest handler
// sends multiple request with body simultaneously
TEST_CASE("PostTest_multiple", "[server, body, multiple]")
{
    std::vector<Testcase> tcs = {tcase01, tcase02, tcase03, tcase01, tcase02, tcase03, tcase01, tcase02, tcase03};
    test_multiple<PostTest, Testcase>(tcs);
}
#endif
#if 0
// tests pipelining and keep-alive/close in server and request handler
// sends multiple request with body back to back on the same connection
TEST_CASE("PostTest_pipeline", "[server, body, pipeline]")
{
//    std::vector<EchoTestcase> tcs = {tcase01, tcase02, tcase03};
    std::vector<Testcase> tcs = {tcase01, tcase02, tcase03, tcase01, tcase02, tcase03, tcase01, tcase02, tcase03};
    test_one<PipelineTest, std::vector<Testcase>>(tcs);
}
#endif
#endif
