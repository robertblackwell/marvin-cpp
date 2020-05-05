
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/beast/core/multi_buffer.hpp>
using multi_buffer = boost::beast::basic_multi_buffer<std::allocator<char>>;
#include <boost/asio/basic_streambuf.hpp>
#include <doctest/doctest.h>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

namespace Marvin {


/**
* A simple implementation of a concrete working http parser. Required to test the abstract Parser class.
 */
class ConcreteParser
{
public:
    Parser*  m_parser_ptr;
    MessageBase* m_message_ptr;
    ConcreteParser()
    {
        m_message_ptr = new MessageBase();
        m_parser_ptr = new Parser(m_message_ptr);
    }
    void OnParseBegin() 
    {
        std::cout << __func__ << std::endl;
    }
    void OnHeadersComplete(MessageInterface* msg, void* body_start_ptr, std::size_t remainder) 
    {
        std::cout << __func__ << std::endl;
    }
    void OnMessageComplete(MessageInterface* msg) {
    }
    void OnParseError() 
    {
        std::cout << __func__ << std::endl;
    }
    void OnBodyData(void* buf, int len) 
    {
        std::cout << __func__ << std::endl;
    }
    void OnChunkBegin(int chunkLength) 
    {
        std::cout << __func__ << std::endl;
    }
    void OnChunkData(void* buf, int len) 
    {
        MBufferSPtr mb_sptr = Marvin::MBuffer::makeSPtr(buf, len);
        BufferChainSPtr chain_sptr = m_message_ptr->getContentBuffer();
        if (chain_sptr == nullptr) {
            chain_sptr = BufferChain::makeSPtr(mb_sptr);
            m_message_ptr->setContentBuffer(chain_sptr);
        } else {
            chain_sptr->push_back(mb_sptr);
        }
    }
    void OnChunkEnd() 
    {
        std::cout << __func__ << std::endl;
    }
    bool messageComplete() 
    {
        std::cout << __func__ << std::endl;
        return m_parser_ptr->isFinishedMessage();
    }
    int consume(boost::asio::streambuf& streambuf)
    {

    }

};
} // namespace

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
void verify_result(std::vector<Marvin::MessageInterface*> messages)
{
    Marvin::MessageBase* m0 = dynamic_cast<Marvin::MessageBase*>(messages[0]);
    CHECK(m0->httpVersMajor() == 1);
    CHECK(m0->httpVersMinor() == 1);
    CHECK(m0->statusCode() == 200);
    CHECK(m0->header("CONTENT-LENGTH") == "10");
    CHECK(m0->header("CONNECTION") == "keep-alive");
    CHECK(m0->header("PROXY-CONNECTION") == "keep-alive");
    auto b0 = m0->getContentBuffer()->to_string();
    CHECK(m0->getContent()->to_string() == "1234567890");
    Marvin::MessageBase* m1 = dynamic_cast<Marvin::MessageBase*>(messages[1]);
    CHECK(m1->httpVersMajor() == 1);
    CHECK(m1->httpVersMinor() == 1);
    CHECK(m1->statusCode() == 201);
    CHECK(m1->header("CONTENT-LENGTH") == "11");
    CHECK(m1->header("CONNECTION") == "keep-alive");
    CHECK(m1->header("PROXY-CONNECTION") == "keep-alive");
    auto b1 = m1->getContentBuffer()->to_string();
    CHECK(m1->getContent()->to_string() == "ABCDEFGHIJK");
}
TEST_CASE("new parsing interface")
{
    SUBCASE("parsing1 - lowest level") 
    {
        Marvin::ConcreteParser* concrete_parser_ptr = new Marvin::ConcreteParser();
        std::vector<Marvin::MessageInterface*> messages;
        
        boost::asio::streambuf streambuffer(2000);
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
                char* get_b = (char*)get_buf.data();
                int get_sz = get_buf.size();
                char* b = get_b;
                len = get_sz;
                Marvin::Parser::ReturnValue ret = concrete_parser_ptr->m_parser_ptr->consume((void*) b, len);
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
                        messages.push_back((concrete_parser_ptr->m_parser_ptr->currentMessage()));
                        concrete_parser_ptr = new Marvin::ConcreteParser();
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
    SUBCASE("boost::asio::const_buffer") 
    {
        Marvin::ConcreteParser* concrete_parser_ptr = new Marvin::ConcreteParser();
        std::vector<Marvin::MessageInterface*> messages;
        
        boost::asio::streambuf streambuffer(2000);
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
                Marvin::Parser::ReturnValue ret = concrete_parser_ptr->m_parser_ptr->consume(get_buf);
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
                        messages.push_back((concrete_parser_ptr->m_parser_ptr->currentMessage()));
                        concrete_parser_ptr = new Marvin::ConcreteParser();
                    break;
                }
            }
        }
        verify_result(messages);
    }
    SUBCASE("parsing1 - boost::asio::streambuf") 
    {
        Marvin::ConcreteParser* concrete_parser_ptr = new Marvin::ConcreteParser();
        std::vector<Marvin::MessageInterface*> messages;
        
        boost::asio::streambuf streambuffer(2000);

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
                Marvin::Parser::ReturnValue ret = concrete_parser_ptr->m_parser_ptr->consume(streambuffer);
                switch(ret.return_code) {
                    case Marvin::Parser::ReturnCode::error:
                        REQUIRE(false);
                    break;
                    case Marvin::Parser::ReturnCode::end_of_data:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_header:
                    break;
                    case Marvin::Parser::ReturnCode::end_of_message:
                        messages.push_back((concrete_parser_ptr->m_parser_ptr->currentMessage()));
                        concrete_parser_ptr = new Marvin::ConcreteParser();
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
        Marvin::ConcreteParser* concrete_parser_ptr = new Marvin::ConcreteParser();
        std::vector<Marvin::MessageInterface*> messages;
        
        boost::asio::streambuf streambuffer(2000);

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
                Marvin::Parser::ReturnValue ret = concrete_parser_ptr->m_parser_ptr->consume(streambuffer, true);
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
                        messages.push_back((concrete_parser_ptr->m_parser_ptr->currentMessage()));
                        concrete_parser_ptr = new Marvin::ConcreteParser();
                    break;
                }
            }
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
                Marvin::Parser::ReturnValue ret = concrete_parser_ptr->m_parser_ptr->consume(streambuffer, false);
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
                        messages.push_back((concrete_parser_ptr->m_parser_ptr->currentMessage()));
                        concrete_parser_ptr = new Marvin::ConcreteParser();
                    break;
                }
            }
        }
        verify_result(messages);
    }
} // end etstcase
