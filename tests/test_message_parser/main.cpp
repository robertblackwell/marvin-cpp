
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/asio.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

/**
* A simple implementation of a concrete working http parser. Required to test the abstract Parser class.
 */
class ConcreteParser: public Marvin::Http::Parser {
public:
    Marvin::Http::MessageBase* m_message;
    
    Marvin::Http::MessageInterface* currentMessage() {
        return m_message;
    };
    void OnParseBegin() {
        m_message = new Marvin::Http::MessageBase();
    }
    void OnHeadersComplete(Marvin::Http::MessageInterface* msg, void* body_start_ptr, std::size_t remainder) {

    }
    void OnMessageComplete(Marvin::Http::MessageInterface* msg) {
    }
    void OnParseError() {

    }
    void OnBodyData(void* buf, int len) {

    }
    void OnChunkBegin(int chunkLength) {

    }
    void OnChunkData(void* buf, int len) {
        Marvin::MBufferSPtr mb_sptr = Marvin::MBuffer::makeSPtr(buf, len);
        Marvin::BufferChainSPtr chain_sptr = m_message->getContentBuffer();
        if (chain_sptr == nullptr) {
            chain_sptr = Marvin::BufferChain::makeSPtr(mb_sptr);
            m_message->setContentBuffer(chain_sptr);
        } else {
            chain_sptr->push_back(mb_sptr);
        }
    }
    void OnChunkEnd() {

    }
    bool messageComplete() {
        return isFinishedMessage();
    }

};

TEST_CASE("simple good message ") {
    
    char* str[] = {
    (char*) "GET /apath HTTP/1.1\r\nHost: ahost\r\n",
    (char*) "Connection: keep-alive\r\nProxy-Connection: keep-alive\r\n",
    (char*) "Content-Length: 10\r\n\r\n",
    (char*) "9123456789",
    NULL
    };
    
    ConcreteParser parser;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        char* buf = str[i];
        int len = (int)strlen(buf);
        int nparsed = parser.appendBytes((void*) buf, len);
        CHECK(nparsed == len);
    }
    Marvin::Http::MessageBase* msg_p = dynamic_cast<Marvin::Http::MessageBase*>(parser.currentMessage());
    CHECK(msg_p->httpVersMajor() == 1);
    CHECK(msg_p->httpVersMinor() == 1);
    CHECK(msg_p->getMethodAsString() == "GET");
    CHECK(msg_p->header("CONTENT-LENGTH") == "10");
    CHECK(msg_p->header("CONNECTION") == "keep-alive");
    CHECK(msg_p->header("PROXY-CONNECTION") == "keep-alive");
    auto b = msg_p->getContentBuffer()->to_string();
    CHECK(msg_p->getContent()->to_string() == "9123456789");
}

#if 1
TEST_CASE("test streaming - two or more messages back to back") {
    // to test this our concrete Parser will need to acquire some extra capability
    // specifically the ability to tell someone that the message has arrived
    char* str[] = {
        (char*) "HTTP/1.1 200 OK 11Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n",
        (char*) "Content-length: 10\r\n\r\n",
        (char*) "1234567890",
        (char*) "HTTP/1.1 201 OK 22Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n",
        (char*) "Content-length: 11\r\n",
        (char*) "\r\n",
        (char*) "ABCDEFGHIJK",
        NULL
    };
    ConcreteParser parser;
    std::vector<Marvin::Http::MessageInterface*> messages;
    
    for(int i = 0; str[i] != NULL  ;i++)
    {
        char* buf = str[i];
        int len = (int)strlen(buf);
        int nparsed = parser.appendBytes((void*) buf, len);
        std::cout << "nparsed: " << nparsed  << "len: " << len << std::endl;
        if (parser.isFinishedMessage()) {
            messages.push_back(parser.currentMessage());
            parser.setUpParserCallbacks();
            parser.setUpNextMessage();
        }
        CHECK(nparsed == len);
    }
    Marvin::Http::MessageBase* m0 = dynamic_cast<Marvin::Http::MessageBase*>(messages[0]);
    CHECK(m0->httpVersMajor() == 1);
    CHECK(m0->httpVersMinor() == 1);
    CHECK(m0->statusCode() == 200);
    CHECK(m0->header("CONTENT-LENGTH") == "10");
    CHECK(m0->header("CONNECTION") == "keep-alive");
    CHECK(m0->header("PROXY-CONNECTION") == "keep-alive");
    auto b0 = m0->getContentBuffer()->to_string();
    CHECK(m0->getContent()->to_string() == "1234567890");
    Marvin::Http::MessageBase* m1 = dynamic_cast<Marvin::Http::MessageBase*>(messages[1]);
    CHECK(m1->httpVersMajor() == 1);
    CHECK(m1->httpVersMinor() == 1);
    CHECK(m1->statusCode() == 201);
    CHECK(m1->header("CONTENT-LENGTH") == "11");
    CHECK(m1->header("CONNECTION") == "keep-alive");
    CHECK(m1->header("PROXY-CONNECTION") == "keep-alive");
    auto b1 = m1->getContentBuffer()->to_string();
    CHECK(m1->getContent()->to_string() == "ABCDEFGHIJK");
    
}
#endif
#if 1

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
    
    ConcreteParser parser;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        char* buf = str[i];
        int len = (int)strlen(buf);
        int nparsed = parser.appendBytes((void*) buf, len);
        
        CHECK(nparsed == len);
    }
    Marvin::Http::MessageBase* msg_p = dynamic_cast<Marvin::Http::MessageBase*>(parser.currentMessage());
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
#endif


// #pragma mark - main
// int main(int argc, char * argv[]) {
//     RBLogging::setEnabled(false);
//     char* _argv[2] = {argv[0], (char*)"--catch_filter=*.*"}; // change the filter to restrict the tests that are executed
//     int _argc = 2;
//     int result = Catch::Session().run( argc, argv );

//     return result;
// }
