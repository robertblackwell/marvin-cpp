#ifndef test_runner_hpp
#define test_runner_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <marvin/error/marvin_error.hpp>
#include <marvin/http/header_fields.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/message/message_reader_v2.hpp>
#include "testcase.hpp"
#include "mock_socket.hpp"

class Testrunner;
typedef std::shared_ptr<Testrunner> TestrunnerSPtr;

std::string chain_to_string(Marvin::BufferChain chain);
/**
* Class TestRunner - Creates an instance of MessageReader using
* its IReadSocket and then exercises that MessageReader
* using a single Testcase.
* Can either:
*   -   use the MessageReader to read an entire message
*   -   read the headers and then read chunks of body as separate reads
*/
class Testrunner
{

public:
    Marvin::MessageReaderV2::SPtr  m_rdr;
    Marvin::MessageBase::SPtr      m_msg_sptr;
    Marvin::ISocketSPtr            m_conn;
    boost::asio::io_service&       m_io;
    std::string                    m_body;
    std::ostringstream             m_bodyStream;
    Testcase                       m_tcObj;
    std::string                    m_body_accumulator;

    /**
    * Constructor - tcIndex is an index into the set of testcases
    * that the class TestCases knows about
    */
    Testrunner(boost::asio::io_service& io, Marvin::ISocketSPtr rd_sock, Testcase tcObj);
    ~Testrunner();
    /**
    * runs a test that reads reads a full message
    */
    void run_FullMessageRead();
    void run_MultipleMessageRead();
    void run_MultipleContinue();
    /**
    * runs a test that reads the headers only
    */
    void run_StreamingBodyRead();

    void makeReader();
    void onMessage(Marvin::ErrorType er);
    void onBody(Marvin::ErrorType er, Marvin::BufferChain::SPtr chunkSPtr);
    void onHeaders(Marvin::ErrorType er);
};
#endif
