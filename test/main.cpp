#define CATCH_CONFIG_MAIN

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include "HTTPMessage.hpp"
#include "HTTPParser.hpp"

#include "test_set_1.cpp"

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


#pragma mark test callbacks

TEST_CASE("test call backs good message ") {
    
    Mock mock = Mock();
    
    std::string str1 =
    std::string("GET /apath HTTP/1.1\r\nHost: ahost\r\n")
    + std::string("Connection: keep-alive\r\nProxy-Connection: keep-alive\r\n")
    + std::string("Content-Length: 10\r\n\r\n")
    + std::string("9123456789");
    
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str1); // Displays this variable on a FAIL
    
    const char* buf = str1.c_str();
    int len = str1.length();
    
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
                           (HTTPParser* p, HTTPMessage* msg, bool& breakFlag)
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
