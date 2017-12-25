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
#include "test_server_client/bf/bf_post.hpp"
#include "test_server_client/bf/bf_pipeline.hpp"
#include "test_server_client/bf/bf_testcase.hpp"
#include "test_server_client/tsc_req_handler.hpp"
#include "server_runner.hpp"
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
    /**
    * Parameterized fixture for testing each message stand alone
    * which requires a server to be started
    */
    class BFOneshot : public ::testing::Test, public ::testing::WithParamInterface<body_format::Testcase>
    {
        public:
        BFOneshot(): _tc(GetParam())
        {}
        ~BFOneshot(){}
        virtual void SetUp(){}
        virtual void TearDown() {}
        body_format::Testcase   _tc;
    };

}
// run each test on a separate io_service
TEST_P(BFOneshot, 001)
{
    const body_format::Testcase& testcase = GetParam();
    boost::asio::io_service io_service;
    body_format::PostTest tst(io_service, testcase);
    tst.exec();
    std::cout << testcase._description << std::endl;
    io_service.run();
}

INSTANTIATE_TEST_CASE_P(BFconsecutive, BFOneshot, testing::Values(tcase01, tcase02, tcase03));


// run all request on a single io_service at the same time
// but all as separate request streams and separate connections
TEST(BFMultiple, alltogether)
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

TEST(BFMultiple, pipeline)
{
    boost::asio::io_service io_service;
    std::vector<body_format::Testcase> cases = make_cases();
    body_format::PipelineTest tst(io_service, cases);
    tst.exec();
    io_service.run();
}

