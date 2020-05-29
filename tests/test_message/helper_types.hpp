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
#include <marvin/message/message_reader_v2.hpp>

namespace Marvin{
namespace Tests{
namespace Message{

/**
 * Input data is a list of raw data baffers as input to a parser.
 */ 
using InputData = std::vector<char*>;
/**
 *  MsgList is a vector of parsed messages that is the result of running 
 *  InputData through a parser.
 */
using MsgList = std::vector<Marvin::MessageBase::SPtr>;
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
struct DataSource {
    int m_block_count;
    /** A array/list of data blocks null terminated*/
    std::vector<char*> m_blocks;
    /**
     *
     * @param blocks An array/list of UTF-8 data buffers/strings each null terminated
     */
    DataSource(std::vector<char*> blocks)
    {
        m_block_count = 0;
        m_blocks = blocks;
    }
    /**
     * Returns the next block of utf-8 null terminated data, NULL when done
     * @return char*, NULL when done
     */
    char* next()
    {
        char* block = m_blocks[m_block_count];
        m_block_count++;
        return block;
    }
    /**
     * @return bool true when no more data
     */
    bool finished()
    {
        return (m_blocks[m_block_count] == NULL);
    }
    std::size_t read_data(void* buffer, std::size_t length)
    {
        char* block = m_blocks[m_block_count];
        if (block == NULL) {
            return 0;
        }
        m_block_count++;
        int block_len = strlen(block);
        memcpy((void*)buffer, block, block_len);
        return block_len;
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
 * This class wraps a complete parser test to make it easier to run
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
    DataSource&         m_data_source;
    VerifyFunctionType  m_verify_func;
    MsgList             m_messages;

    WrappedParserTest(Marvin::Parser& parser, DataSource& data_source, VerifyFunctionType verify_func)
    : m_parser(parser), m_data_source(data_source), m_verify_func(verify_func)
    {}
    void operator()()
    {
        using namespace Marvin;
        
        boost::asio::streambuf streambuffer(2000);
        Marvin::MessageBase::SPtr message_sptr = std::make_shared<MessageBase>();
        m_parser.begin(message_sptr);
        int bytes_read;
        while(true) {
            bytes_read = m_data_source.read_data(streambuffer);
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
                        m_messages.push_back((m_parser.current_message()));
                        message_sptr = std::make_shared<MessageBase>();
                        m_parser.begin(message_sptr);
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
                        m_messages.push_back(message_sptr);
                        /// get ready for the next one
                        /// notice we keep processing the same buffer even with a 
                        /// new message
                        message_sptr = std::make_shared<MessageBase>();
                        m_parser.begin(message_sptr);
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
/**
 * WrappedReaderTest tests MessageReader using a mock of the ISocket interface
 * on test data supplied by DataSource. Supports data the represents multiple
 * back to back messages.
 */
struct WrappedReaderTest
{
    MessageReaderV2         m_rdr;
    DataSource&             m_data_source;
    VerifyFunctionType      m_verify_func;
    MsgList                 m_messages;

    WrappedReaderTest(ISocketSPtr socket_sptr, DataSource& data_source, VerifyFunctionType verify_func)
    : m_rdr(socket_sptr), m_data_source(data_source), m_verify_func(verify_func)
    {}
    void operator()()
    {
        read_one();
    }
    void read_one() 
    {
        m_rdr.async_read_message ([this] (ErrorType err)
        {
            if (err) {
                std::cout << "error: " << err.message () << std::endl;
                m_verify_func (m_messages);
                return;
            } else {
                MessageBase::SPtr msg_sptr = m_rdr.get_message_sptr();
                m_messages.push_back (msg_sptr);
                next_one ();
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