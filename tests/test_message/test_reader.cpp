//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <string>

#include <doctest/doctest.h>

#include <marvin/message/message_reader_v2.hpp>
#include "mock_socket.hpp"
#include "testcase.hpp"
#include "helper_types.hpp"
#include "data_single.hpp"
#include "data_two.hpp"
#include "data_eof.hpp"
using namespace Marvin;
using namespace Tests;
using namespace Message;
#if 0
//-------------------------------------------------------------------------------------
// Run a list of testcases (that exercise a MessageReader instance) using a mock socket 
// (that simulates asyn data arrival with a timer) reading an entire 
// message -- using MessageReader::async_read_message() -- in a single call
// to the messagereader
//-------------------------------------------------------------------------------------
void test_fullmessage(std::vector<Testcase> tcs)
{
    for(auto & testcase : tcs) {
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
    for(auto & testcase : tcs) {
        boost::asio::io_service io_service;
        MockReadSocketSPtr msock_ptr = std::make_shared<MockReadSocket>(io_service, testcase);
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
#endif
namespace Marvin {
namespace Tests {
namespace Parser {
#if 1
TEST_CASE ("single")
{
    TestCollection& tc = single_message_test_data_2();
    for(auto entry: tc) {
        boost::asio::io_service io_service;
        std::string key = entry.first;
        TestSet ts = entry.second;
        std::cout << "Commencing test of Parser test data set: " << ts.description  << std::endl;
        DataSource data_source(ts.input_data);
        Marvin::Parser parser;
        MockReadSocketSPtr sock_sptr = std::make_shared<MockReadSocket>(io_service, data_source);
        WrappedReaderTest wpt(sock_sptr, data_source, ts.verify_function);
        wpt();
        io_service.run();
        std::cout << "Completed test of Parser test data set: " << ts.description  << std::endl;
    }
}
#endif
#if 1
TEST_CASE("on_test_data")
{
    TestCollection& tc = test_data_two_message();
    for(auto entry: tc) {
        boost::asio::io_service io_service;
        std::string key = entry.first;
        TestSet ts = entry.second;
        std::cout << "Commencing test of Parser test data set: " << ts.description  << std::endl;
        DataSource data_source(ts.input_data);
        MockReadSocketSPtr sock_sptr = std::make_shared<MockReadSocket>(io_service, data_source);
        WrappedReaderTest wpt(sock_sptr, data_source, ts.verify_function);
        wpt();
        io_service.run();
        std::cout << "Completed test of Parser test data set: " << ts.description  << std::endl;
    }
}
#endif
TEST_CASE("eof")
{
    TestCollection& tc = test_data_eof();
    for(auto entry: tc) {
        boost::asio::io_service io_service;
        std::string key = entry.first;
        TestSet ts = entry.second;
        std::cout << "Commencing test of Parser test data set: " << ts.description  << std::endl;
        DataSource data_source(ts.input_data);
        MockReadSocketSPtr sock_sptr = std::make_shared<MockReadSocket>(io_service, data_source);
        WrappedReaderTest wpt(sock_sptr, data_source, ts.verify_function);
        wpt();
        io_service.run();
        std::cout << "Completed test of Parser test data set: " << ts.description  << std::endl;
    }
}

} // Parser
} // namespace Tests
} // namespace Marvin