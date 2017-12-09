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

#include "catch.hpp"

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "tsc_post.hpp"
#include "tsc_pipeline.hpp"
#include "tsc_testcase.hpp"
#include "tsc_req_handler.hpp"

static EchoTestcase tcase01(
        EchoTestType::WITH_STRING,
        "With string - 2 buffers",
        {
            "1234567890",
            "HGYTRESAWQ"
        }
    );
static EchoTestcase  tcase02(
        EchoTestType::WITH_MBUFFER,
        "With mbuffer - 2 buffers",
        {
            "1234567890",
            "HGYTRESAWQ"
        }
    );
static EchoTestcase tcase03(
        EchoTestType::WITH_BUFFER_CHAIN,
        "With buffer chain - 2 buffers",
        {
            "1234567890",
            "HGYTRESAWQ"
        }
    );
void runTestClient();
void test_one(EchoTestcase& testcase);
void test_multiple(std::vector<EchoTestcase> cases);

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

// sends request with using different forms of body data structure
// tests body parsing
auto f1 = test_one<PostTest, EchoTestcase>;
TEST_CASE("PostTest_consec", "[server, body, consecutive]")
{
    std::vector<EchoTestcase> tcs = {tcase01, tcase02, tcase03};
    for(auto & c : tcs) {
        test_one<PostTest, EchoTestcase>(c);
    }
}

// sends multiple request with body simultaneously
// tests simultaneous operation of server and reuqest handler
TEST_CASE("PostTest_multiple", "[server, body, multiple]")
{
    std::vector<EchoTestcase> tcs = {tcase01, tcase02, tcase03};
    test_multiple<PostTest, EchoTestcase>(tcs);
}

// sends multiple request with body back to back on the same connection
// tests pipelining in server
TEST_CASE("PostTest_pipeline", "[server, body, pipeline]")
{
    std::vector<EchoTestcase> tcs = {tcase01, tcase02, tcase03};
    test_one<PipelineTest, std::vector<EchoTestcase>>(tcs);
}
