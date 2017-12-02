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
TServerSPtr
serverFunction(boost::asio::io_service& io, Testcase tc)
{
    LogDebug("");
    TServerSPtr srv = std::shared_ptr<TServer>(new TServer(io, tc));
    srv->listen(9991, [](MessageReaderV2SPtr rdr) {
//        printf("server got a message reader");
        /// get here with a connected reader
        /// start reading messages and test to see if they conform to th expected Tescase results
    });
    return srv;
}

/**
* Just send all the buffers privided in the Testcase. Maybe leave a little room between them so that
* they dont get merged by lower layers
*/
TClientSPtr clientFunction(boost::asio::io_service& io, Testcase tc)
 {
    LogDebug("");
    TClientSPtr client = std::shared_ptr<TClient>(new TClient(io, "http", "localhost", "9991", tc));
    client->send_testcase_buffers([](Marvin::ErrorType err){
//         printf("all buffers have been sent\n");
    });
    LogDebug("");
    return client;
 }
/**  --------------------------------------------------------------------------------------
* run client and server the same thread
*---------------------------------------------------------------------------------------*/
void runTestcase(Testcase tcase)
{
    LogDebug("");
    boost::asio::io_service io;
    TServerSPtr sp = serverFunction(io, tcase);
    TClientSPtr cp = clientFunction(io, tcase);
    io.run();
    LogDebug("");
}


int main(int argc, char* argv[])
{
//    twoProcesses();
    RBLogging::setEnabled(false);
//    TestcaseDefinitions tcs = makeTestcaseDefinitions_01();
    TestcaseDefinitions tcs = makeTCS_eof();

    for(int i = 0; i < tcs.number_of_testcases(); i++) {
        runTestcase(tcs.get_case(i));
    }
    printf("Progrm exit");
    return 0;
    
}
