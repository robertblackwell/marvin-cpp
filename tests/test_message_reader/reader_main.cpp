//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>

RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

#include "error.hpp"
#include "repeating_timer.hpp"
#include "mock_socket.hpp"
#include "testcase.hpp"
#include <marvin/message/message_reader.hpp>
#include "t_server.hpp"
#include "t_client.hpp"
#include "testcase_defs.hpp"
//-------------------------------------------------------------------------------------
// Run a list of testcases (that exercise a MessageReader instance) using a mock socket 
// reading an entire message -- using MessageReader::readMessage() -- in a single call 
// to the messagereader
//-------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------
// Run a list of testcases (that exercise a MessageReader instance) using a mock socket 
// reading a message in chunks, headers first (using MessageReader::readHeader())
// and then reads the message body in chunks (using MessageReader.readBodyData())
// until EOM
//-------------------------------------------------------------------------------------
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
TEST_CASE( "Reader_buffering_fullmessage")
{
    printf("START %s[%d]\n", __FILE__, __LINE__);
    test_fullmessage(tc_make_buffering());
    printf("END %s[%d]\n", __FILE__, __LINE__);
}
TEST_CASE( "Reader_eof_fullmessage")
{
    printf("START %s[%d]\n", __FILE__, __LINE__);
    test_fullmessage(tc_make_eof());
    printf("END %s[%d]\n", __FILE__, __LINE__);
}
TEST_CASE( "Reader_hv_fullmessage")
{
    printf("START %s[%d]\n", __FILE__, __LINE__);
    test_fullmessage(tc_make_hv());
    printf("END %s[%d]\n", __FILE__, __LINE__);
}
TEST_CASE( "Reader_buffering_streaming")
{
    printf("START %s[%d]\n", __FILE__, __LINE__);
    test_streamingBody(tc_make_buffering());
    printf("END %s[%d]\n", __FILE__, __LINE__);
}
//------------------------------------------------------------------------------------------------------
// Run a single testcases using a tcp (not http) client and server pair built specifically for this
// application. The client sends a message consisting of a single line of text that identifies the 
// message the server is to send in reply. The server then sends that message as a predetermined list of
// buffers that form an http message (both requests and responses). 
// The client reads the reply message in one of two modes -- full message or headers and body in chunks
// and when complete checks the result against the expected value (via testcase_result.hpp/cpp).
//
// This allows the testing of a message reader where there is no restriction on the type of
// message that can be sent - requests, response, chunked encoding, incorrectly encoded.
//-------------------------------------------------------------------------------------------------------
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
        std::cout << "got here" << std::endl;
        //go here is a win
    });
    /**
    * The client sends the message
    */
    TClientSPtr client = std::shared_ptr<TClient>(new TClient(io, "http", "localhost", "9991", tc));
    client->send_testcase_buffers([](Marvin::ErrorType err){
        printf("all buffers have been sent\n");
    });
    io.run();
    std::cout << "after io run" << std::endl;
    LogDebug("");
}
//------------------------------------------------------------------------------------------------------
// Tests a list of testcases using the custom clioent and server.
//------------------------------------------------------------------------------------------------------
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
