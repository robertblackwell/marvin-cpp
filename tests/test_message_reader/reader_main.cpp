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
#include <marvin/configure_trog.hpp>

TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

#include "repeating_timer.hpp"
#include "mock_socket.hpp"
#include "testcase.hpp"
#include <marvin/message/message_reader.hpp>
// #include "t_server.hpp"
// #include "t_client.hpp"
#include "testcase_defs.hpp"
#include "test_runner.hpp"
//-------------------------------------------------------------------------------------
// Run a list of testcases (that exercise a MessageReader instance) using a mock socket 
// (that simulates asyn data arrival with a timer) reading an entire 
// message -- using MessageReader::readMessage() -- in a single call 
// to the messagereader
//-------------------------------------------------------------------------------------
void test_fullmessage(std::vector<Testcase> tcs)
{
    for(auto const& testcase : tcs) {
        boost::asio::io_service io_service;
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        // work_guard_type work_guard(io_service.get_executor());
        MockReadSocketSPtr msock_ptr = std::make_shared<MockReadSocket>(io_service, testcase);
        auto tr = new Testrunner(io_service, msock_ptr, testcase);
        io_service.post([tr]()
        {
            tr->run_FullMessageRead();
        });
        io_service.run();
        delete tr;
    }
}
//-------------------------------------------------------------------------------------
// Run a list of testcases (that exercise a MessageReader instance) using a mock socket
// (as above simulating async io with a timer) reading a message in chunks, headers first
// (using MessageReader::readHeader())
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
