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
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "error.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include "tcp_connection.hpp"
#include "message_reader_v2.hpp"
#include "t_server.hpp"
#include "t_client.hpp"
#include "testcase_defs.hpp"

/**
* This cli program provides a harness for testing the MessageReaderV2 class
* against a set of testcases where the MessaageReaderV2 reads from a
* socket.
* Each testcase is processed by setting up a single boost::asio::io_service
* and then running on that io_service a simple (and special purpose) tcp server and
* a simple (and special ppurpose) tcp client. The client takes message buffers from the testcase
* and sends those to the server. The server parses those incoming buffers (using an instance of MessageReaderV2)
* into Http messages and verifies the result against the "expected"
* values contained in a copy of the same testcase.
*
*   -   the client is the sender of buffers
*   -   the server is the reader of messages and the verofier of the results
*
*/
/**  --------------------------------------------------------------------------------------
* run client and server the same thread to test MessageReaderV2
*---------------------------------------------------------------------------------------*/
static void runTestcase(Testcase tc)
{
    LogDebug("");
    boost::asio::io_service io;
    /**
    * The server reads and verifies the message. NOte the server needs
    * the testcase object to verify the message is received correctly
    */
    TServerSPtr srv = std::shared_ptr<TServer>(new TServer(io, tc));
    srv->listen(9991, [](MessageReaderV2SPtr rdr) {
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
// tests ending a message with out chunked encoding or content-length
TEST_CASE("MessageReader_Socket_test_end_of_message")
{
    TestcaseDefinitions tcs = makeTCS_eof();
    for(int i = 0; i < tcs.number_of_testcases(); i++) {
        runTestcase(tcs.get_case(i));
    }
}
// tests reading messages with all kinds of buffering
// to ensure parse crosses from heading to body correctly
// and that chunked encoding works correctly
TEST_CASE("MessageReader_Socket_test_buffering")
{
    TestcaseDefinitions tcs = makeTestcaseDefinitions_01();
    for(int i = 0; i < tcs.number_of_testcases(); i++) {
        runTestcase(tcs.get_case(i));
    }
}

#pragma mark - main
int main(int argc, char * argv[]) {
    RBLogging::setEnabled(false);
    int result = Catch::Session().run( argc, argv );
}
