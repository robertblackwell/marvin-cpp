//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>

#include <catch2/catch.h>
#include <marvin/include/boost_stuff.hpp>
#include <marvin/external_src/rb_logger.hpp>

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "error.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include <marvin/connection/tcp_connection.hpp>
#include <marvin/message/message_reader.hpp>
#include "t_server.hpp"
#include "t_client.hpp"
#include "testcase_defs.hpp"

/**
* This cli program provides a harness for testing the MessageReader class
* against a set of testcases where the MessaageReader reads from a
* socket.
* Each testcase is processed by setting up a single boost::asio::io_service
* and then running on that io_service a simple (and special purpose) tcp server and
* a simple (and special ppurpose) tcp client. The client takes message buffers from the testcase
* and sends those to the server. The server parses those incoming buffers (using an instance of MessageReader)
* into Http messages and verifies the result against the "expected"
* values contained in a copy of the same testcase.
*
*   -   the client is the sender of buffers
*   -   the server is the reader of messages and the verofier of the results
*
*/
/**  --------------------------------------------------------------------------------------
* run client and server the same thread to test MessageReader
*---------------------------------------------------------------------------------------*/
#if 1
static void runTestcase(Testcase tc)
{
    LogDebug("");
    boost::asio::io_service io;
    /**
    * The server reads and verifies the message. NOte the server needs
    * the testcase object to verify the message is received correctly
    */
    TServerSPtr srv = std::shared_ptr<TServer>(new TServer(io, tc));
    srv->listen(9991, [](MessageReaderSPtr rdr) {
        //go here is a win
    });
    /**
    * The client sends the message
    */
    TClientSPtr client = std::shared_ptr<TClient>(new TClient(io, "http", "localhost", "9991", tc));
    client->send_testcase_buffers([](Marvin::ErrorType err){
        //printf("all buffers have been sent\n");
    });
    io.run();
    LogDebug("");
}
#endif
// tests ending a message with out chunked encoding or content-length
TEST(messagereader, endofmessage)
{
    ASSERT_TRUE(true);
    TestcaseDefinitions tcs = makeTCS_eof();
    for(int i = 0; i < tcs.number_of_testcases(); i++) {
        runTestcase(tcs.get_case(i));
    }
}
// tests reading messages with all kinds of buffering
// to ensure parse crosses from heading to body correctly
// and that chunked encoding works correctly
TEST(messagereader, buffering)
{
    ASSERT_TRUE(true);
    TestcaseDefinitions tcs = makeTestcaseDefinitions_01();
    for(int i = 0; i < tcs.number_of_testcases(); i++) {
        runTestcase(tcs.get_case(i));
    }
}
TestcaseDefinitions tc_defs;
namespace {
    class MyFixture1 : public ::testing::TestWithParam<int>
    {
        public:
        MyFixture1(): _tcdefs(), _tc(tc_defs.get_case(GetParam()))
        {
            int index = GetParam();
            auto tt = tc_defs;
            auto tc = tc_defs.get_case_ptr(index);
        }
        ~MyFixture1(){}
        virtual void SetUp()
        {
            std::cout << "setup" << std::endl;
        }
        virtual void TearDown()
        {
        
        }
        TestcaseDefinitions _tcdefs;
        Testcase _tc;
    };
    class MyFixture2 : public ::testing::TestWithParam<Testcase>
    {
        public:
        MyFixture2(): _tcdefs(), _tc(GetParam())
        {
            std::cout << "" << std::endl;
        }
        ~MyFixture2(){}
        virtual void SetUp()
        {
            std::cout << "setup" << std::endl;
        }
        virtual void TearDown()
        {
        
        }
        TestcaseDefinitions _tcdefs;
        Testcase _tc;
    };


}
TEST_P(MyFixture1, test01)
{
    std::cout << "TEST_F::MyFixture::_data " <<  std::endl;
}
INSTANTIATE_TEST_CASE_P(experiment, MyFixture1, testing::Values(1,2,3));

TEST_P(MyFixture2, test01)
{
    std::cout << "TEST_F::MyFixture::_data " <<  std::endl;
}
INSTANTIATE_TEST_CASE_P(experiment, MyFixture2, testing::Values(tc_defs.get_case(1),tc_defs.get_case(2),tc_defs.get_case(3)));
INSTANTIATE_TEST_CASE_P(experiment2, MyFixture2, testing::ValuesIn(makeTestcaseDefinitions_01().cases));
#if 0
class TCFixture : public ::testing::TestWithParam<Testcase>
{
    
};
TEST_P(TCFixture, run)
{
    auto tc = GetParam();
    runTestcase(tc);
}
TestcaseDefinitions tcs;
INSTANTIATE_TEST_CASE_P(instanceName, TCFixture, ::testing::ValuesIn(tcs.cases));
#endif
int main(int argc, char * argv[])
{
    RBLogging::setEnabled(false);
    tc_defs = makeTestcaseDefinitions_01();
    char* _argv[2] = {argv[0], (char*)"--catch_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    auto ret = RUN_ALL_TESTS();
    return ret;
}

