//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>

#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>
#include "boost_stuff.hpp"
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

void test_fullmessage(std::vector<Testcase> tcs)
{
    for(auto const& testcase : tcs) {
        boost::asio::io_service io_service;
        MockReadSocketSPtr msock_ptr = std::shared_ptr<MockReadSocket>(new MockReadSocket(io_service, testcase));
        auto tr = new Testrunner(io_service, msock_ptr, testcase);
        tr->run_FullMessageRead();
        io_service.run();
        delete tr;
    }
}
void test_streamingBody(std::vector<Testcase> tcs)
{
    for(auto const& testcase : tcs) {
        boost::asio::io_service io_service;
        MockReadSocketSPtr msock_ptr = std::shared_ptr<MockReadSocket>(new MockReadSocket(io_service, testcase));
        auto tr = new Testrunner(io_service, msock_ptr, testcase);
        tr->run_StreamingBodyRead();
        io_service.run();
        delete tr;
    }
}
TEST_CASE( "Reader_buffering_fullmessage", "")
{
    test_fullmessage(tc_make_buffering());
}
TEST_CASE( "Reader_eof_fullmessage", "")
{
    test_fullmessage(tc_make_eof());
}
TEST_CASE( "Reader_hv_fullmessage", "")
{
    test_fullmessage(tc_make_hv());
}
TEST_CASE( "Reader_buffering_streaming","")
{
    test_streamingBody(tc_make_buffering());
}

void testcase_socketReader(Testcase tc)
{
//    std::cout << "TEST_F::MyFixture::_data " <<  std::endl;
//    Testcase tc = GetParam();
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
void test_vector_socketReader(std::vector<Testcase> tcs)
{
    for(auto const& testcase : tcs) {
        testcase_socketReader(testcase);
    }
}
TEST_CASE("Reader_socket_buffering")
{
    printf("START %s[%d]\n", __FILE__, __LINE__);
    test_vector_socketReader(tc_make_buffering());
    printf("END %s[%d]\n", __FILE__, __LINE__);
}
TEST_CASE("Reader_socket_eof")
{
    printf("START %s[%d]\n", __FILE__, __LINE__);
    test_vector_socketReader(tc_make_eof());
    printf("END %s[%d]\n", __FILE__, __LINE__);
}
TEST_CASE("Reader_socket_hv")
{
    printf("START %s[%d]\n", __FILE__, __LINE__);
    test_vector_socketReader(tc_make_hv());
    printf("END %s[%d]\n", __FILE__, __LINE__);
}
#if 0
TEST_CASE("ReaderFixture_sockettest","")
{
    printf("%s[%d]\n", __FILE__, __LINE__);
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

//INSTANTIATE_TEST_CASE_P( ReaderSocketBufTest, ReaderFixture, testing::ValuesIn(tc_make_buffering()));
//INSTANTIATE_TEST_CASE_P( ReaderSocketEOFTest, ReaderFixture, testing::ValuesIn(tc_make_eof()));
INSTANTIATE_TEST_CASE_P( ReaderSocketEOFTest, ReaderFixture, testing::ValuesIn(tc_make_hv()));
#endif
int main(int argc, char * argv[])
{
    RBLogging::setEnabled(false);

    char* _argv[] = {argv[0], (char*)"-s", (char*)"-r", (char*)"junit"}; // change the filter to restrict the tests that are executed
    int _argc = 4;
    printf("%s\n", __FILE__);
    int result = Catch::Session().run( argc, argv );
    printf("%s\n", __FILE__);
    return result;
}

