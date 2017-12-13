#ifndef test_runner_hpp
#define test_runner_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <gtest/gtest.h>
#include "marvin_error.hpp"
#include "http_header.hpp"
#include "error.hpp"
#include "buffer.hpp"
#include "message_reader.hpp"
#include "testcase.hpp"
#include "mock_read_socket.hpp"

class Testrunner;
typedef std::shared_ptr<Testrunner> TestrunnerSPtr;

std::string chain_to_string(BufferChain chain);
/**
* Class TestRunner - Creates an instance of MessageReader using
* its ReadSocketInterface and then exercises that MessageReader
* using a single Testcase.
* Can either:
*   -   use the MessageReader to read an entire message
*   -   read the headers and then read chunks of body as separate reads
*/
class Testrunner
{
    
public:
    MessageReaderSPtr         rdr_;
    ReadSocketInterfaceSPtr     conn_;
    boost::asio::io_service&    io_;
    std::string                 body;
    std::ostringstream          bodyStream;
    Testcase                    _tcObj;
    std::string                 body_accumulator;

    /**
    * Constructor - tcIndex is an index into the set of testcases
    * that the class TestCases knows about
    */
    Testrunner(boost::asio::io_service& io, ReadSocketInterfaceSPtr rd_sock, Testcase tcObj);
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
