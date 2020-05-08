#ifndef marvin_tests_parser_helpers_hpp
#define marvin_tests_parser_helpers_hpp
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include <boost/asio/basic_streambuf.hpp>
#include <doctest/doctest.h>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>
#include <marvin/message/message_reader.hpp>

namespace Marvin{
namespace Tests{
namespace Parser{

/**
 * Input data is a list of raw data baffers as input to a parser.
 */ 
using InputData = std::vector<char*>;
/**
 *  MsgList is a vector of parsed messages that is the result of running 
 *  InputData through a parser.
 */
using MsgList = std::vector<Marvin::MessageBase*>;
/**
 * A VerifyFunction is a callable that examines a MsgList to
 * check that is gives the expected result;
 */
using VerifyFunctionType = std::function<void(MsgList msg_list)>;

/** 
 * A test set consists of an INputData instance (input data buffers)
 * and a verify_function that can verify the correctness of the outcome
 * from parsing those input buffers
*/
struct TestSet 
{
    std::string         description;
    InputData           input_data;
    VerifyFunctionType  verify_function;
    TestSet(){}
    TestSet(std::string desc, InputData in_data, VerifyFunctionType vfunc)
    {
        description = desc; input_data = in_data; verify_function = vfunc;
    }
};

using TestCollection = std::map<std::string, TestSet>;

/**
 * Purpose of this class is to demo use of the parser in a situation
 * that simulates synchronously reading from some source of bytes
 */
struct LineSource {
    int m_line_count;
    std::vector<char*> m_lines;
    LineSource(std::vector<char*> lines)
    {
        m_line_count = 0;
        m_lines = lines;
    }
    std::size_t read_data(void* buffer, std::size_t length)
    {
        char* line = m_lines[m_line_count];
        if (line == NULL) {
            return 0;
        }
        m_line_count++;
        int line_len = strlen(line);
        memcpy((void*)buffer, line, line_len);
        return line_len;
    }
    std::size_t read_data(boost::asio::mutable_buffer& mutablebuffer)
    {
        void* p = mutablebuffer.data();
        std::size_t size = mutablebuffer.size();
        return read_data(p, size);
    }
    std::size_t read_data(boost::asio::streambuf& streambuffer)
    {
        boost::asio::mutable_buffer mb = streambuffer.prepare(100);
        std::size_t bytes_read = read_data(mb);
        streambuffer.commit(bytes_read);
        return bytes_read;
    }
};
/**
 * As can be seen below using the Marvin::Parser directly requires
 * a good deal of boiler plate code which was OK while developing the parser
 * and to illustrate how to use it with differnt kinds fo buffers
 * but it is tedious.
 * This class will wrap a complete parser test to make it easier to run
 * the sme test on different test data.
 * 
 * WrappedParser test supports test data consisting of multiple back to back messages
 * and also correctly handles messages that require EOF to signal end-of-message
 */
struct WrappedParserTest
{
    // using MsgList = std::vector<Marvin::MessageBase*>;
    // using VerifyFunctionType = std::function<void(MsgList msg_list)>;
    
    Marvin::Parser&     m_parser;
    LineSource&         m_line_source;
    VerifyFunctionType  m_verify_func;
    MsgList             m_messages;

    WrappedParserTest(Marvin::Parser& parser, LineSource& line_source, VerifyFunctionType verify_func)
    : m_parser(parser), m_line_source(line_source), m_verify_func(verify_func)
    {}
    void operator()()
    {
        using namespace Marvin;
        
        boost::asio::streambuf streambuffer(2000);
        Marvin::MessageBase* message_ptr = new Marvin::MessageBase();
        m_parser.begin(message_ptr);
        int bytes_read;
        while(true) {
            bytes_read = m_line_source.read_data(streambuffer);
            if (bytes_read == 0) {
                /// example of eof processing
                Marvin::Parser::ReturnValue ret = m_parser.end();
                switch(ret.return_code) {
                    case Marvin::Parser::ReturnCode::error:
                        REQUIRE(false);
                    case Marvin::Parser::ReturnCode::end_of_data:
                    case Marvin::Parser::ReturnCode::end_of_header:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_message:
                        m_messages.push_back((m_parser.currentMessage()));
                        m_parser.begin(new Marvin::MessageBase());
                    break;
                }
                break;
            }

            while (streambuffer.data().size() > 0) {
                boost::asio::const_buffer get_buf = streambuffer.data();
                char* get_b = (char*)get_buf.data();
                int get_sz = get_buf.size();
                char* b = get_b;
                int len = get_sz;
                Marvin::Parser::ReturnValue ret = m_parser.consume((void*) b, len);
                ///
                /// keep track of data comsumed
                ///
                streambuffer.consume(len - ret.bytes_remaining);
                switch(ret.return_code) {
                    case Marvin::Parser::ReturnCode::error:
                        REQUIRE(false);
                    break;
                    case Marvin::Parser::ReturnCode::end_of_data:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_header:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_message:
                        /// save the just parsed message
                        m_messages.push_back(message_ptr);
                        /// get ready for the next one
                        /// notice we keep processing the same buffer even with a 
                        /// new message
                        message_ptr = new MessageBase();
                        m_parser.begin(message_ptr);
                    break;
                }
                auto z = streambuffer.data().data();
                char* z_p = (char*)z;
                auto z_sz = streambuffer.data().size();
                // std::cout << "z: " << z << " content: " << get_b <<  std::endl;
            }
        }
        m_verify_func(m_messages);
    }
};

struct WrappedReaderTest
{
    MessageReader           m_rdr;
    LineSource&             m_line_source;
    VerifyFunctionType      m_verify_func;
    MsgList                 m_messages;

    WrappedReaderTest(ISocketSPtr socket_sptr, LineSource& line_source, VerifyFunctionType verify_func)
    : m_rdr(socket_sptr), m_line_source(line_source), m_verify_func(verify_func)
    {}
    void operator()()
    {
        read_one();
    }
    void read_one() 
    {
        m_rdr.readMessage([this](ErrorType err)
        {
            if(err) {
                m_verify_func(m_messages);
                return;
            } else {
                MessageBase& msg{m_rdr};
                m_messages.push_back(&msg);
                next_one();
            }
        });
    }
    void next_one()
    {
        read_one();
    }
};
} // namespace Parser
} // namespace Tests
} // namespace Marvin

#endif