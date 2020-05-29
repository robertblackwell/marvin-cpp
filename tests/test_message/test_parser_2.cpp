
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/basic_streambuf.hpp>
#include <doctest/doctest.h>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>


namespace {

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
    using MsgList = std::vector<Marvin::MessageBase::SPtr>;
    using VerifyFunctionType = std::function<void(MsgList msg_list)>;
    
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
        Marvin::MessageBase::SPtr message_sptr = std::make_shared<Marvin::MessageBase>();
        m_parser.begin(message_sptr);
        int bytes_read;
        while(true) {
            bytes_read = m_line_source.read_data(streambuffer);
            if (bytes_read == 0) {
                /// eof processing should be here - but not relevant to this test
                /// just signals end of test data
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
                        message_sptr = std::make_shared<Marvin::MessageBase>();
                        m_parser.begin(message_sptr);
                    break;
                }
                auto z = streambuffer.data().data();
                char* z_p = (char*)z;
                auto z_sz = streambuffer.data().size();
                std::cout << "z: " << z << " content: " << get_b <<  std::endl;
            }
        }
        m_verify_func(m_messages);
    }
};
std::vector<char*>& test_data()
{
    static std::vector<char*> lines = {
        (char*) "HTTP/1.1 200 OK 11Reason Phrase\r\n\0        ",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep\0    ",
        (char*) "-alive\r\n\0    ",
        (char*) "Content-length: 10\r\n\r\n",
        (char*) "1234567",
        (char*) "890HTTP/1.1 ",
        (char*) "201 OK 22Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n",
        (char*) "Content-length: 11\r\n",
        (char*) "\r\n",
        (char*) "ABCDEFGHIJK\0      ",
        NULL
        };
    return lines;
}
std::vector<char*>& test_eof_data()
{
    static std::vector<char*> lines = {
        (char*) "HTTP/1.1 200 OK 11Reason Phrase\r\n\0        ",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep\0    ",
        (char*) "-alive\r\n\0    ",
        (char*) "\r\n",
        (char*) "1234567890",
        NULL
    };
    return lines;
}

void verify_result(std::vector<Marvin::MessageBase::SPtr> messages)
{
    REQUIRE(messages.size() > 0);
    Marvin::MessageBase::SPtr m0 = messages[0];
    Marvin::HeaderFields& h = m0->headers();
    REQUIRE(m0 != nullptr);
    CHECK(m0->version_major() == 1);
    CHECK(m0->version_minor() == 1);
    CHECK(m0->status_code() == 200);
    CHECK(h.at_key("CONTENT-LENGTH").get() == "10");
    CHECK(h.at_key("CONNECTION").get() == "keep-alive");
    CHECK(h.at_key("PROXY-CONNECTION").get() == "keep-alive");
    auto b0 = m0->get_body_buffer_chain()->to_string();
    CHECK(m0->get_body()->to_string() == "1234567890");
    Marvin::MessageBase::SPtr m1 = messages[1];
    Marvin::HeaderFields& h2 = m1->headers();
    REQUIRE(m1 != nullptr);
    CHECK(m1->version_major() == 1);
    CHECK(m1->version_minor() == 1);
    CHECK(m1->status_code() == 201);
    CHECK(h2.at_key("CONTENT-LENGTH").get() == "11");
    CHECK(h2.at_key("CONNECTION").get() == "keep-alive");
    CHECK(h2.at_key("PROXY-CONNECTION").get() == "keep-alive");
    auto b1 = m1->get_body_buffer_chain()->to_string();
    CHECK(m1->get_body()->to_string() == "ABCDEFGHIJK");
}
void verify_eof_test(std::vector<Marvin::MessageBase::SPtr> messages)
{
    Marvin::MessageBase::SPtr m0 = messages[0];
    Marvin::HeaderFields& h = m0->headers();
    CHECK(m0->version_major() == 1);
    CHECK(m0->version_minor() == 1);
    CHECK(m0->status_code() == 200);
    CHECK(h.at_key("CONNECTION").get() == "keep-alive");
    CHECK(h.at_key("PROXY-CONNECTION").get() == "keep-alive");
    auto b0 = m0->get_body_buffer_chain()->to_string();
    CHECK(m0->get_body()->to_string() == "1234567890");

}
} // anonymous namespace
TEST_CASE("wrappedparser")
{
    using namespace Marvin;
    Parser parser;
    LineSource line_source{test_data()};
    WrappedParserTest test{parser, line_source, verify_result};
    test();
}
TEST_CASE("two messages")
{
    ///
    /// This is test that demonstrates a number of import topics.
    ///
    /// Using a single data set these tests demonstrate using all overloads of the 
    ///  
    /// Marvin::Parser::consume() method
    ///
    /// and demonstrate how to use these overloads in oder to correctly 
    /// process incoming data where:
    ///
    /// -   a buffer contains both the last piece of a message header as well as the first
    ///     piece of a message body.
    /// and/or
    ///
    /// -   part of a second message is included in the last buffer of the first
    ///     message 
    ///
    /// In both these cases the secret to handling this data correctly is to ensure that
    /// any buffer not FULLY processed by a call to Parser::consume() is passed to the next
    /// call to Parser::consume() so that the "left over" data can be processed before any new data.
    ///
    /// The easiest way to achieve this is by using boost::asio::streambuf as the container for
    /// data presented to Parser::consume();
    ///
    /// This is illustrated below in the various tests.
    ///
    /// The first three subcases deal with both situations 
    /// (overlap between header and body, overlap between consecutive messages)
    ///  while parsing an entire message.
    ///
    /// Subcase 4 splits the parsing so as to get the header first and hence has
    /// to provide a two step process in order to get the body after the header
    ///
    /// Another case demonstates how to finalize parsing when the sender of the message
    /// will signal end of message by closing the connection which the parser expects 
    /// to see as EOF (a zero length buffer presented for parsing)
    ///
    SUBCASE("raw-buffers") 
    {
        using namespace Marvin;
        Parser* parser_ptr = new Parser();
        std::vector<Marvin::MessageBase::SPtr> messages;
        
        boost::asio::streambuf streambuffer(2000);
        LineSource line_source(test_data());
        Marvin::MessageBase::SPtr message_sptr = std::make_shared<Marvin::MessageBase>();
        parser_ptr->begin(message_sptr);
        int bytes_read;
        while(true) {
            bytes_read = line_source.read_data(streambuffer);
            if (bytes_read == 0) {
                /// eof processing should be here - but not relevant to this test
                /// just signals end of test data
                break;
            }
            while (streambuffer.data().size() > 0) {
                boost::asio::const_buffer get_buf = streambuffer.data();
                char* get_b = (char*)get_buf.data();
                int get_sz = get_buf.size();
                char* b = get_b;
                int len = get_sz;
                Marvin::Parser::ReturnValue ret = parser_ptr->consume((void*) b, len);
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
                        messages.push_back(message_sptr);
                        /// get ready for the next one
                        /// notice we keep processing the same buffer even with a 
                        /// new message
                        message_sptr = std::make_shared<Marvin::MessageBase>();
                        parser_ptr->begin(message_sptr);
                    break;
                }
                auto z = streambuffer.data().data();
                char* z_p = (char*)z;
                auto z_sz = streambuffer.data().size();
                std::cout << "z: " << z << " content: " << get_b <<  std::endl;
            }
        }
        verify_result(messages);
    }
    SUBCASE("boost_buffers") 
    {
        Marvin::Parser* parser_ptr = new Marvin::Parser();
        std::vector<Marvin::MessageBase::SPtr> messages;
        
        boost::asio::streambuf streambuffer(2000);
        parser_ptr->begin(std::make_shared<Marvin::MessageBase>());
        std::vector<char*>& lines = test_data();
        for(int i = 0; lines[i] != NULL  ;i++)
            {
            char* buf = lines[i];
            int len = (int)strlen(buf);
            int len_saved = len;
            ///
            /// this is the prep for a make-believe read that adds data to the 
            /// streambuffer put are
            ///
            boost::asio::mutable_buffer put_mb = streambuffer.prepare(len+10);
            char*put_b = (char*)put_mb.data();
            memcpy(put_b, buf, len);
            ///
            /// read is complete so commit the data read
            ///
            streambuffer.commit(len);
            ///
            /// back from read - get ready to process the data until
            /// the streambuffer get area is empty
            ///
            while (streambuffer.data().size() > 0) {
                ///
                /// this is a lot simpler as streambuffer knows how to deliver a boost::asio::const_buffer
                ///
                boost::asio::const_buffer get_buf = streambuffer.data();
                Marvin::Parser::ReturnValue ret = parser_ptr->consume(get_buf);
                ///
                /// still have to do our own book keeping re data consumed
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
                        messages.push_back((parser_ptr->current_message()));
                        parser_ptr->begin(std::make_shared<Marvin::MessageBase>());
                    break;
                }
            }
        }
        verify_result(messages);
    }
    SUBCASE("boost_streambuf") 
    {
        Marvin::Parser* parser_ptr = new Marvin::Parser();
        std::vector<Marvin::MessageBase::SPtr> messages;
        
        boost::asio::streambuf streambuffer(2000);
        parser_ptr->begin(std::make_shared<Marvin::MessageBase>());

        std::vector<char*>& lines = test_data();
        for(int i = 0; lines[i] != NULL  ;i++)
            {
            char* buf = lines[i];
            int len = (int)strlen(buf);
            int len_saved = len;
            ///
            /// this is the prep for a make-believe read that adds data to the 
            /// streambuffer put are
            ///
            boost::asio::mutable_buffer put_mb = streambuffer.prepare(len+10);
            char*put_b = (char*)put_mb.data();
            memcpy(put_b, buf, len);
            ///
            /// read is complete so commit the data read
            ///
            streambuffer.commit(len);
            ///
            /// back from read - get ready to process the data until
            /// the streambuffer get area is empty
            ///
            while (streambuffer.data().size() > 0) {
                boost::asio::const_buffer get_buf = streambuffer.data();
                ///
                /// no messing around we will simple pass the streambuffer
                ///
                Marvin::Parser::ReturnValue ret = parser_ptr->consume(streambuffer);
                ///
                /// also note now consume() takes care of the book keeping re data consumed
                ///
                switch(ret.return_code) {
                    case Marvin::Parser::ReturnCode::error:
                        REQUIRE(false);
                    break;
                    case Marvin::Parser::ReturnCode::end_of_data:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_header:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_message:
                        messages.push_back((parser_ptr->current_message()));
                        parser_ptr->begin(std::make_shared<Marvin::MessageBase>());
                    break;
                }
                auto z = streambuffer.data().data();
                char* z_p = (char*)z;
                auto z_sz = streambuffer.data().size();
                std::cout << "z: " << z << " content: " << buf <<  std::endl;
            }
        }
        verify_result(messages);
    }
    SUBCASE("parsing header then body - boost::asio::streambuf") 
    {
        Marvin::Parser* parser_ptr = new Marvin::Parser();
        std::vector<Marvin::MessageBase::SPtr> messages;
        
        boost::asio::streambuf streambuffer(2000);
        parser_ptr->begin(std::make_shared<Marvin::MessageBase>());

        std::vector<char*>& lines = test_data();
        for(int i = 0; lines[i] != NULL  ;i++)
            {
            char* buf = lines[i];
            int len = (int)strlen(buf);
            int len_saved = len;
            ///
            /// this is the prep for a make-believe read that adds data to the 
            /// streambuffer put are
            ///
            boost::asio::mutable_buffer put_mb = streambuffer.prepare(len+10);
            char*put_b = (char*)put_mb.data();
            memcpy(put_b, buf, len);
            ///
            /// read is complete so commit the data read
            ///
            streambuffer.commit(len);
            ///
            /// back from read - get ready to process the data until
            /// the streambuffer get area is empty
            ///
            bool headers_done = false;
            ///
            /// first lets process the headers
            /// this loop processes buffers until the headers are finished
            /// and then processes no more
            ///
            while ((!headers_done)&&(streambuffer.data().size() > 0)) {
                Marvin::Parser::ReturnValue ret = parser_ptr->consume(streambuffer, true);
                switch(ret.return_code) {
                    case Marvin::Parser::ReturnCode::error:
                        REQUIRE(false);
                    break;
                    case Marvin::Parser::ReturnCode::end_of_data:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_header:
                        headers_done = true;
                    break;
                    case Marvin::Parser::ReturnCode::end_of_message:
                        messages.push_back((parser_ptr->current_message()));
                        parser_ptr->begin(std::make_shared<Marvin::MessageBase>());
                    break;
                }
            }
            ///
            /// NOTE when transitioning from header to body 
            /// ====
            ///     streambuffer.data().size() MAY not be zero  
            ///     ==========================================
            ///
            ///     in which case the following loop will process that buffer
            ///     before any others
            ///
            /// now lets do the remainder of the message - note the false in the call
            /// to consume
            /// this loop only processes buffers after the headers have completed
            ///
            while ((headers_done)&&(streambuffer.data().size() > 0)) {
                boost::asio::const_buffer get_buf = streambuffer.data();
                char* get_b = (char*)get_buf.data();
                int get_sz = get_buf.size();
                char* b = get_b;
                len = get_sz;
                Marvin::Parser::ReturnValue ret = parser_ptr->consume(streambuffer, false);
                switch(ret.return_code) {
                    case Marvin::Parser::ReturnCode::error:
                        REQUIRE(false);
                    break;
                    case Marvin::Parser::ReturnCode::end_of_data:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_header:
                        headers_done = true;
                    break;
                    case Marvin::Parser::ReturnCode::end_of_message:
                        messages.push_back((parser_ptr->current_message()));
                        parser_ptr->begin(std::make_shared<Marvin::MessageBase>());
                    break;
                }
            }
        }
        verify_result(messages);
    }
} // end testcase

TEST_CASE("eof")
{
    SUBCASE("eof 1") 
    {
        using namespace Marvin;
        Parser* parser_ptr = new Parser();
        std::vector<Marvin::MessageBase::SPtr> messages;
        
        boost::asio::streambuf streambuffer(2000);
        LineSource line_source(test_eof_data());
        Marvin::MessageBase::SPtr message_sptr = std::make_shared<Marvin::MessageBase>();
        parser_ptr->begin(message_sptr);
        int bytes_read;
        while(true) {
            bytes_read = line_source.read_data(streambuffer);
            if (bytes_read == 0) {
                /// example of eof processing
                Marvin::Parser::ReturnValue ret = parser_ptr->end();
                switch(ret.return_code) {
                    case Marvin::Parser::ReturnCode::error:
                    case Marvin::Parser::ReturnCode::end_of_data:
                        REQUIRE(false);
                    break;
                    case Marvin::Parser::ReturnCode::end_of_header:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_message:
                        messages.push_back((parser_ptr->current_message()));
                        parser_ptr->begin(std::make_shared<Marvin::MessageBase>());
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
                Marvin::Parser::ReturnValue ret = parser_ptr->consume((void*) b, len);
                ///
                /// keep track of data comsumed
                ///
                streambuffer.consume(len - ret.bytes_remaining);
                switch(ret.return_code) {
                    case Marvin::Parser::ReturnCode::error:
                        REQUIRE(false);
                    break;
                    case Marvin::Parser::ReturnCode::end_of_data:
                    case Marvin::Parser::ReturnCode::end_of_header:
                        /// not of interest as we want to parse an entire message
                    break;
                    case Marvin::Parser::ReturnCode::end_of_message:
                        /// save the just parsed message
                        messages.push_back(message_sptr);
                        /// get ready for the next one
                        /// notice we keep processing the same buffer even with a 
                        /// new message
                        message_sptr = std::make_shared<Marvin::MessageBase>();
                        parser_ptr->begin(message_sptr);
                    break;
                }
                auto z = streambuffer.data().data();
                char* z_p = (char*)z;
                auto z_sz = streambuffer.data().size();
                std::cout << "z: " << z << " content: " << get_b <<  std::endl;
            }
        }
        verify_eof_test(messages);
    }
}