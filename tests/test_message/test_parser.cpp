
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/asio.hpp>

#include <doctest/doctest.h>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

namespace Marvin {
/**
* A simple implementation of a concrete working http parser. Required to test the abstract Parser class.
 */
class ConcreteParser: public Parser {
public:
    MessageBase* m_message;
    
    MessageInterface* currentMessage() {
        return m_message;
    };
    void OnParseBegin() 
    {
        m_message = new MessageBase();
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
        BufferChainSPtr chain_sptr = m_message->getContentBuffer();
        if (chain_sptr == nullptr) {
            chain_sptr = BufferChain::makeSPtr(mb_sptr);
            m_message->setContentBuffer(chain_sptr);
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
        return message_done();
    }
    
    int consume(boost::asio::streambuf& streambuf)
    {

    }

};
} // namespace

TEST_CASE("simple good message ") {
    
    char* str[] = {
    (char*) "GET /apath HTTP/1.1\r\nHost: ahost\r\n",
    (char*) "Connection: keep-alive\r\nProxy-Connection: keep-alive\r\n",
    (char*) "Content-Length: 10\r\n\r\n",
    (char*) "9123456789",
    NULL
    };
    
    Marvin::ConcreteParser parser;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        char* buf = str[i];
        int len = (int)strlen(buf);
        int nparsed = parser.appendBytes((void*) buf, len);
        CHECK(nparsed == len);
    }
    Marvin::MessageBase* msg_p = dynamic_cast<Marvin::MessageBase*>(parser.currentMessage());
    CHECK(msg_p->httpVersMajor() == 1);
    CHECK(msg_p->httpVersMinor() == 1);
    CHECK(msg_p->getMethodAsString() == "GET");
    CHECK(msg_p->header("CONTENT-LENGTH") == "10");
    CHECK(msg_p->header("CONNECTION") == "keep-alive");
    CHECK(msg_p->header("PROXY-CONNECTION") == "keep-alive");
    auto b = msg_p->getContentBuffer()->to_string();
    CHECK(msg_p->getContent()->to_string() == "9123456789");
}

TEST_CASE("test streaming - two or more messages back to back") {
    // to test this our concrete Parser will need to acquire some extra capability
    // specifically the ability to tell someone that the message has arrived
    // also test header line can be spread over non contiguous buffers
    char* str[] = {
        (char*) "HTTP/1.1 200 OK 11Reason Phrase\r\n\0        ",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep\0    ",
        (char*) "-alive\r\n\0    ",
        (char*) "Content-length: 10\r\n\r\n",
        (char*) "1234567890",
        (char*) "HTTP/1.1 201 OK 22Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n",
        (char*) "Content-length: 11\r\n",
        (char*) "\r\n",
        (char*) "ABCDEFGHIJK\0      ",
        NULL
    };
    Marvin::ConcreteParser parser;
    std::vector<Marvin::MessageInterface*> messages;
    
    for(int i = 0; str[i] != NULL  ;i++)
    {
        char* buf = str[i];
        int len = (int)strlen(buf);
        char* b = (char*)malloc(len+10);
        memcpy(b, buf, len);
        // std::cout << "b : " << std::hex << (long)b << " b_end: " << (long)&(b[len-1]) << std::dec << "len: " << len << " content: " << buf <<  std::endl;
        int nparsed = parser.appendBytes((void*) b, len);
        // std::cout << "nparsed: " << nparsed  << "len: " << len << " content: " << buf <<  std::endl;
        free(b);
        if (parser.message_done()) {
            messages.push_back(parser.currentMessage());
            parser.setUpParserCallbacks();
            parser.setUpNextMessage();
        }
        CHECK(nparsed == len);
    }
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

TEST_CASE("test streaming - fragmented buffer - two or more messages back to back") {
    // to test this our concrete Parser will need to acquire some extra capability
    // specifically the ability to tell someone that the message has arrived
    // also test header line can be spread over non contiguous buffers
    char* str[] = {
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
    Marvin::ConcreteParser* parser_ptr = new Marvin::ConcreteParser();
    std::vector<Marvin::MessageInterface*> messages;
    
    for(int i = 0; str[i] != NULL  ;i++)
    {
        char* buf = str[i];
        int len = (int)strlen(buf);
        char* b = (char*)malloc(len+10);
        memcpy(b, buf, len);
        // std::cout << "b : " << std::hex << (long)b << " b_end: " << (long)&(b[len-1]) << std::dec << "len: " << len << " content: " << buf <<  std::endl;
        for(int j = 0; j < len; j++) {
            char ch = b[j];
            int nparsed = parser_ptr->appendBytes((void*) &(b[j]), 1);
            std::cout << "nparsed: " << nparsed << " is error ? :: " << (int)parser_ptr->isError() << std::endl;
            if (parser_ptr->message_done()) {
                messages.push_back((*parser_ptr).currentMessage());
                parser_ptr = new Marvin::ConcreteParser();
                // parser_ptr->setUpParserCallbacks();
                // parser_ptr->setUpNextMessage();
            }
        }
        // std::cout << "nparsed: " << nparsed  << "len: " << len << " content: " << buf <<  std::endl;
        free(b);
        // CHECK(nparsed == len);
    }
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


TEST_CASE("test chunked message ") {

    std::cout << "good chunked message" << std::endl;
    char* str[] = {
        (char*) "HTTP/1.1 201 OK Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n",
        (char*) "Transfer-Encoding: chunked\r\n",
        (char*) "\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0f\r\n1234567",
        (char*) "890XXXXX\r\n",
        (char*) "0f\r\n1234567890YYYYY\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0f\r\n1234567890HGHGH\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0\r\n",
        (char*) "\r\n",
        NULL
    };
    
    Marvin::ConcreteParser parser;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        char* buf = str[i];
        int len = (int)strlen(buf);
        // put next chunk of incoming data into a buffer, make it oversize so the next one
        // cannot be contiguous
        char* b = (char*)malloc(len+10);
        memcpy(b, buf, len);
        
        int nparsed = parser.appendBytes((void*) b, len);
        CHECK(nparsed == len);
    }
    Marvin::MessageBase* msg_p = dynamic_cast<Marvin::MessageBase*>(parser.currentMessage());
    CHECK(msg_p->httpVersMajor() == 1);
    CHECK(msg_p->httpVersMinor() == 1);
    CHECK(msg_p->statusCode() == 201);
    CHECK(! msg_p->hasHeader("CONTENT-LENGTH"));
    CHECK(msg_p->header("TRANSFER-ENCODING") == "chunked");
    CHECK(msg_p->header("CONNECTION") == "keep-alive");
    CHECK(msg_p->header("PROXY-CONNECTION") == "keep-alive");
    auto b = msg_p->getContentBuffer()->to_string();
    CHECK(msg_p->getContent()->to_string() ==
        (
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890XXXXX") +
        std::string("1234567890YYYYY") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890HGHGH") +
        std::string("1234567890"))
        );

}
TEST_CASE("header-data-same-buffer chunked message ") {

    std::cout << "good chunked message" << std::endl;
    char* str[] = {
        (char*) "HTTP/1.1 201 OK Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Transfer-Encoding: chunked\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n\r\n0a\r\n123",
        (char*) "4567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0f\r\n1234567890XXXXX\r\n",
        (char*) "0f\r\n1234567890YYYYY\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0f\r\n1234567890HGHGH\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0\r\n",
        (char*) "\r\n",
        NULL
    };
    
    Marvin::ConcreteParser parser;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        char* buf = str[i];
        int len = (int)strlen(buf);
        int nparsed = parser.appendBytes((void*) buf, len);
        
        CHECK(nparsed == len);
    }
    Marvin::MessageBase* msg_p = dynamic_cast<Marvin::MessageBase*>(parser.currentMessage());
    CHECK(msg_p->httpVersMajor() == 1);
    CHECK(msg_p->httpVersMinor() == 1);
    CHECK(msg_p->statusCode() == 201);
    CHECK(! msg_p->hasHeader("CONTENT-LENGTH"));
    CHECK(msg_p->header("TRANSFER-ENCODING") == "chunked");
    CHECK(msg_p->header("CONNECTION") == "keep-alive");
    CHECK(msg_p->header("PROXY-CONNECTION") == "keep-alive");
    auto b = msg_p->getContentBuffer()->to_string();
    CHECK(msg_p->getContent()->to_string() ==
        (
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890XXXXX") +
        std::string("1234567890YYYYY") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890HGHGH") +
        std::string("1234567890"))
        );
    //std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;
}
TEST_CASE("extreme fragment-header-data-same-buffer chunked message ") {

    std::cout << "good chunked message" << std::endl;
    char* str[] = {
        (char*) "HTTP/1.1 201 OK Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Transfer-Encoding: chunked\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n\r\n0a\r\n123",
        (char*) "4567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0f\r\n1234567890XXXXX\r\n",
        (char*) "0f\r\n1234567890YYYYY\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0f\r\n1234567890HGHGH\r\n",
        (char*) "0a\r\n1234567890\r\n",
        (char*) "0\r\n",
        (char*) "\r\n",
        NULL
    };
    
    Marvin::ConcreteParser parser;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        char* buf = str[i];
        int len = (int)strlen(buf);
        int nparsed = parser.appendBytes((void*) buf, len);
        
        CHECK(nparsed == len);
    }
    Marvin::MessageBase* msg_p = dynamic_cast<Marvin::MessageBase*>(parser.currentMessage());
    CHECK(msg_p->httpVersMajor() == 1);
    CHECK(msg_p->httpVersMinor() == 1);
    CHECK(msg_p->statusCode() == 201);
    CHECK(! msg_p->hasHeader("CONTENT-LENGTH"));
    CHECK(msg_p->header("TRANSFER-ENCODING") == "chunked");
    CHECK(msg_p->header("CONNECTION") == "keep-alive");
    CHECK(msg_p->header("PROXY-CONNECTION") == "keep-alive");
    auto b = msg_p->getContentBuffer()->to_string();
    CHECK(msg_p->getContent()->to_string() ==
        (
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890XXXXX") +
        std::string("1234567890YYYYY") +
        std::string("1234567890") +
        std::string("1234567890") +
        std::string("1234567890HGHGH") +
        std::string("1234567890"))
        );
    //std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;
}

