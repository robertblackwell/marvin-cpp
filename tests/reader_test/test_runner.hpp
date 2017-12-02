#ifndef test_runner_hpp
#define test_runner_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include "marvin_error.hpp"
#include "http_header.hpp"
#include "error.hpp"
#include "bufferV2.hpp"
#include "message_reader_v2.hpp"
#include "testcase.hpp"
#include "mock_read_socket.hpp"

class Testrunner;
typedef std::shared_ptr<Testrunner> TestrunnerSPtr;

std::string chain_to_string(BufferChain chain);
/**
* Class TestRunner - Creates an instance of MessageReaderV2 using
* its ReadSocketInterface and then exercises that MessageReaderV2
* using a single Testcase.
* Can either:
*   -   use the MessageREaderV2 to read an entire message
*   -   read the headers and then read chunks of body as separate reads
*/
class Testrunner
{
    
public:
    MessageReaderV2*            rdr_;
    ReadSocketInterface*        conn_;
    boost::asio::io_service&    io_;
    std::string                 body;
    std::ostringstream          bodyStream;
    Testcase                    _tcObj;
    std::string                 body_accumulator;

    /**
    * Constructor - tcIndex is an index into the set of testcases
    * that the class TestCases knows about
    */
    Testrunner(boost::asio::io_service& io, ReadSocketInterface* rd_sock, Testcase tcObj);
    ~Testrunner();
    /**
    * runs a test that reads reads a full message
    */
    void run_FullMessageRead();
    /**
    * runs a test that reads the headers only
    */
    void run_StreamingBodyRead();

    void makeReader();
    void onMessage(Marvin::ErrorType er);
    void onBody(Marvin::ErrorType er, BufferChain chunk);
    void onHeaders(Marvin::ErrorType er);
};
#endif
