#define CATCH_CONFIG_MAIN

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include "HTTPMessage.hpp"
#include "HTTPParser.hpp"

//#include "test_set_1.cpp"
class Connection
{
public:
    Connection()
    {
        index = 0;
        static char* str[] = {
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
            (char*) "Content-length: 10\r\n",
            (char*) "\r\n",
            (char*) "ABCDEFGHIJ",
            NULL
        };
        rawData = str;

    }
    void asyncRead(std::function<void(char* buf, int len)> cb){
        char* buf = rawData[index];
        int len = (int) strlen(buf);
        index++;
        cb(buf, len);
        
    }
private:
    char** rawData;
    int index;
};


class Error
{
public:
    Error(std::string msg){}
    Error& operator=(Error other)
    {
        return *this;
    }
    static Error* success(){
        Error* e = new Error("");
        return e;
    }
};
class BodyBuffer
{
    
};
class ChunkBuffer
{
    
};

typedef std::function<void(Error& err, BodyBuffer& data)>      BodyCbType;
typedef std::function<void(Error& err, ChunkBuffer& chunk)>    ChunkCbType;
typedef std::function<void(Error& err, HTTPMessage* msg)>      HeadersCbType;
typedef std::function<void(Error& err, HTTPMessage* msg)>      ResponseCbType;


/**
 * Instances of this class read an incoming http(s) response message from a stream
 */
class MessageReader
{
public:
    MessageReader(Connection* conn)
    {
        _conn = conn;
    }
    void readResponse(ResponseCbType cb)
    {
        this->responseCb_ = cb;
        Error* e = new Error("Just a test");
        cb(*e, NULL);
         _conn->asyncRead([](char* buf, int len){
             std::cout << __FUNCTION__ << buf << " " << len << std::endl;
         });
        
        /*
         for(int i = 0; str[i] != NULL  ;i++)
         {
         buf = str[i];
         len = (int)strlen(buf);
         nparsed = parser.appendBytes((void*) buf, len);
         std::cout << "nparsed: " << nparsed  << "len: " << len << std::endl;
         bool ip = parser.isPaused();
         if( (nparsed != len) && (parser.finishedHeaders() ) ) {
         parser.unPause();
         int l = len - nparsed;
         int ix = ++nparsed;
         const char* pp = &(buf[ix-1]);
         void* p = (void*) &(buf[ix-1]);
         nparsed = parser.appendBytes(p, l);
         std::cout << "nparsed: " << nparsed  << "l: " << l << std::endl;
         }
         
         CHECK(nparsed == len);
         }

         
         
         this->messageCb = cb;
         async_read from socket up to and including '/r/n/r/n' ((buffer){
            int n = buffer.size();
            int nparsed = parser.appendBytes(buffer);
         //
         // Unless there was an error this should return with nparsed = n - 1
         // as the second \r should have signalled end-of-headers
         // and the OnheadersComplete function will pause the parser 
         // so now we must readjust the buffer to point at the last \r\n
         // cancel the pause
         // parse the last two bytes \r\n
         // and leave the parse ready to handle the body data if there is any
         // if there is no body these last two bytes will have signalled message complete
         //
            if( parser.error ){
                this->onError("parse-error")
            }
         })
         */
    }
    void OnHeadersComplete()
    {
        
    }
    
    void readChunk(ChunkCbType cb)
    {
        this->chunkCb_ = cb;
    }
    void readBodyData(BodyCbType cb)
    {
        this->bodyCb_ = cb;
    }
    void onError(std::string err)
    {
        Error* er = new Error(err);
        this->responseCb_(*er, NULL);
    }
    void onMessage(HTTPMessage* msg)
    {
        Error* er = Error::success();
        
        this->responseCb_(*er, msg);
    }
private:
    Connection* _conn;
    BodyCbType bodyCb_;
    ChunkCbType chunkCb_;
    HeadersCbType headersCb_;
    ResponseCbType responseCb_;
};

class Mock
{
public:
    bool header_flag;
    bool message_flag;
    Mock()
    {
        header_flag = false;
        message_flag = false;
    }
    void mock_call_back(HTTPParser* p)
    {
        std::cout << "got a call back " << std::endl;
    }
};

TEST_CASE("next step"){
    Connection* conn = new Connection();
    MessageReader* rdr = new MessageReader(conn);
    rdr->readResponse([](Error& er, HTTPMessage* msg){
        std::cout << __FUNCTION__ << std::endl;
    });
}

TEST_CASE("test pause and streaming"){
    std::cout << "pause and streaming" << std::endl;
    Mock mock = Mock();

    
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
        (char*) "Content-length: 10\r\n",
        (char*) "\r\n",
        (char*) "ABCDEFGHIJ",
        NULL
    };

    std::string str1("streaming");
    
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str1); // Displays this variable on a FAIL
    
    const char* buf = str1.c_str();
    int len = (int)str1.length();
    
    HTTPParser parser;
    parser.setStreamingOption(true);
    
    using namespace std::placeholders;
    
    //    HTTPParserCallBackFunction cb = mock.mock_call_back;
    
    bool headersCBFlag = false;
    bool messageCBFlag = false;
    
    auto hcb2 = [&mock, &headersCBFlag]( HTTPParser* p, HTTPMessage* msg){
        std::cout << "headers complete CB" << std::endl;
        headersCBFlag = true;
        mock.header_flag  = true;
        p->pause();
    };
    parser.on_HeadersComplete(hcb2);

        
    parser.onBodyDataLambda([ &messageCBFlag, &mock ](HTTPParser* p, HTTPMessage* msg){
                            messageCBFlag = true;
                            mock.message_flag  = true;
    });
    parser.onMessageLambda([ &messageCBFlag, &mock ](HTTPParser* p, HTTPMessage* msg, bool* breakFlag){
                               messageCBFlag = true;
                               mock.message_flag  = true;
   });
//    return;
    int nparsed;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        buf = str[i];
        len = (int)strlen(buf);
        nparsed = parser.appendBytes((void*) buf, len);
        std::cout << "nparsed: " << nparsed  << "len: " << len << std::endl;
        bool ip = parser.isPaused();
        if( (nparsed != len) && (parser.finishedHeaders() ) ) {
            parser.unPause();
            int l = len - nparsed;
            int ix = ++nparsed;
            const char* pp = &(buf[ix-1]);
            void* p = (void*) &(buf[ix-1]);
            nparsed = parser.appendBytes(p, l);
            std::cout << "nparsed: " << nparsed  << "l: " << l << std::endl;
        }
        
        CHECK(nparsed == len);
    }
    
    CHECK(messageCBFlag == true);
    CHECK(headersCBFlag == true);
    CHECK(mock.message_flag == true);
    CHECK(mock.header_flag == true);
  
    
}

TEST_CASE("test call backs good chunked message ") {

    std::cout << "good chunked message" << std::endl;

    Mock mock = Mock();
    
    std::string str1 =
    std::string("GET /apath HTTP/1.1\r\nHost: ahost\r\n")
    + std::string("Connection: keep-alive\r\nProxy-Connection: keep-alive\r\n")
    + std::string("Content-Length: 10\r\n\r\n")
    + std::string("9123456789");
    
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
    
   
    
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str1); // Displays this variable on a FAIL
    
    const char* buf = str1.c_str();
    int len = (int)str1.length();
    
    HTTPParser parser;
    
    using namespace std::placeholders;
    
    //    HTTPParserCallBackFunction cb = mock.mock_call_back;
    
    bool headersCBFlag = false;
    bool messageCBFlag = false;
    auto chunkCb = [&mock](HTTPParser* p, HTTPMessage* msg){
        std::cout << "chunk callback" << std::endl;
    };
    parser.on_ChunkBegin(chunkCb);
    parser.on_ChunkData(chunkCb);
    parser.on_ChunkEnd(chunkCb);
    auto hcb = [&mock]( bool* flag, Mock* mock, HTTPParser* p, HTTPMessage* msg){
        *flag = true;
        mock->header_flag  = true;
    };

    auto hcb2 = [&mock, &headersCBFlag]( HTTPParser* p, HTTPMessage* msg){
        std::cout << "headers complete CB" << std::endl;
        headersCBFlag = true;
        mock.header_flag  = true;
    };
    parser.on_HeadersComplete(hcb2);

    auto hcb3 = [&mock, &messageCBFlag]( HTTPParser* p, HTTPMessage* msg, bool* term){
        std::cout << "message complete cb" << std::endl;
        messageCBFlag = true;
        mock.message_flag  = true;
    };
    parser.on_MessageComplete(hcb3);
    
    int nparsed;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        buf = str[i];
        len = (int)strlen(buf);
        nparsed = parser.appendBytes((void*) buf, len);
        
        CHECK(nparsed == len);
    }
    
    CHECK(messageCBFlag == true);
    CHECK(headersCBFlag == true);
    CHECK(mock.message_flag == true);
    CHECK(mock.header_flag == true);
    
}


#pragma mark test callbacks

TEST_CASE("test call backs good message ") {
    
    std::cout << "callbacks good message content-length" << std::endl;

    Mock mock = Mock();
    
    std::string str1 =
    std::string("GET /apath HTTP/1.1\r\nHost: ahost\r\n")
    + std::string("Connection: keep-alive\r\nProxy-Connection: keep-alive\r\n")
    + std::string("Content-Length: 10\r\n\r\n")
    + std::string("9123456789");
    
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str1); // Displays this variable on a FAIL
    
    const char* buf = str1.c_str();
    int len = (int)str1.length();
    
    HTTPParser parser;
    
    using namespace std::placeholders;
    
    //    HTTPParserCallBackFunction cb = mock.mock_call_back;
    
    bool headersCBFlag = false;
    bool messageCBFlag = false;
    
    parser.onHeadersLambda(
                           [ &headersCBFlag, &mock ]
                           (HTTPParser* p, HTTPMessage* msg)
                           {
                               headersCBFlag = true;
                               mock.header_flag  = true;
                           }
                           );
    
    
    parser.onMessageLambda(
                           [ &messageCBFlag, &mock ]
                           (HTTPParser* p, HTTPMessage* msg, bool* breakFlag)
                           {
                               messageCBFlag = true;
                               mock.message_flag  = true;
                           }
                           );
    
    int nparsed = parser.appendBytes((void*) buf, len);
    
    CHECK(messageCBFlag == true);
    CHECK(headersCBFlag == true);
    CHECK(mock.message_flag == true);
    CHECK(mock.header_flag == true);
    
}

TEST_CASE("test call backs BAD message ") {

    std::cout << "bad message" << std::endl;

    Mock mock = Mock();
    
    std::string str1 =
    std::string("GET /apath HTXTP/1.1\r\nHost: ahost\r\n")
    + std::string("Connection: keep-alive\r\nProxy-Connection: keep-alive\r\n")
    + std::string("Content-Length: 10\r\n\r\n")
    + std::string("9123456789");
    
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str1); // Displays this variable on a FAIL
    
    const char* buf = str1.c_str();
    int len = str1.length();
    
    HTTPParser parser;
    
    bool errorCBFlag = false;

    parser.onParseErrorLambda(
       [ &errorCBFlag ]
       (HTTPParser* p)
       {
           errorCBFlag = true;
       }
   );

    int nparsed = parser.appendBytes((void*) buf, len);

    CHECK(errorCBFlag == true);

    
}
