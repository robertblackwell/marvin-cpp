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

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "error.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include "tcp_connection.hpp"
#include "message_reader.hpp"
#include "t_server.hpp"
#include "t_client.hpp"
#include "testcase_defs.hpp"

namespace {

    /**
    * Global setup/teardown for testcases
    */
    class Environment : public ::testing::Environment
    {
        public:
            virtual ~Environment() {}
            // Override this to define how to set up the environment.
            virtual void SetUp() { std::cout << "global setup" << std::endl;}
            // Override this to define how to tear down the environment.
            virtual void TearDown() { std::cout << "global teardown" << std::endl;}
    };
    /**
    * Fixture for testing the message_reader.hpp/cpp.
    *
    * Two test uses a mock object for reader_socket and test in turn reading a whole
    * message in on go, and reading headers and then the body chunk by chunk
    *
    * Another test uses specila purpose client and server connected via a tcp socket
    * and has the server read messages sent by the client. This test also verifies the
    * function of TCPConnection which is used to implement the underlying transport.
    */
    class ReaderFixture : public ::testing::TestWithParam<Testcase>
    {
        public:
        ReaderFixture(): _tc(GetParam()){}
        ~ReaderFixture(){}
        static void SetUpTestCase(){ std::cout << "test case setup" << std::endl;}
        virtual void SetUp(){}
        virtual void TearDown() {}
        static void TearDownTestCase(){ std::cout << "test case teardown" << std::endl;}
        Testcase _tc;
    };


}
TEST_P( ReaderFixture, mockfullmessage)
{
    boost::asio::io_service io_service;
    Testcase testcase = GetParam();
    MockReadSocketSPtr msock_ptr = std::shared_ptr<MockReadSocket>(new MockReadSocket(io_service, testcase));
    auto tr = new Testrunner(io_service, msock_ptr, testcase);
    tr->run_FullMessageRead();
    io_service.run();
    delete tr;
}

TEST_P( ReaderFixture, mockstreaming)
{
    boost::asio::io_service io_service;
    Testcase testcase = GetParam();
    MockReadSocketSPtr msock_ptr = std::shared_ptr<MockReadSocket>(new MockReadSocket(io_service, testcase));
    auto tr = new Testrunner(io_service, msock_ptr, testcase);
    tr->run_StreamingBodyRead();
    io_service.run();
    delete tr;
}

TEST_P(ReaderFixture, sockettest)
{
//    std::cout << "TEST_F::MyFixture::_data " <<  std::endl;
    Testcase tc = GetParam();
//    runTestcase(tc);
    LogDebug("");
    boost::asio::io_service io;
    /**
    * The server reads and verifies the message. Note the server needs
    * the testcase object to know what a correct message is
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
INSTANTIATE_TEST_CASE_P( ReaderSocketBufTest, ReaderFixture, testing::ValuesIn(tc_make_buffering()));
INSTANTIATE_TEST_CASE_P( ReaderSocketEOFTest, ReaderFixture, testing::ValuesIn(tc_make_eof()));

int main(int argc, char * argv[])
{
    RBLogging::setEnabled(false);

    char* _argv[2] = {argv[0], (char*)"--gtest_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    Environment* rdr_env = new Environment();
    ::testing::Environment* tmp = ::testing::AddGlobalTestEnvironment(rdr_env);
    auto ret = RUN_ALL_TESTS();
    return ret;
}

