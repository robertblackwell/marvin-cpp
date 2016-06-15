
#include <iostream>
#include <iterator>
#include <algorithm>
//#include "catch/catch.hpp"
#include "src/HTTPMessage.hpp"
#include "src/HTTPParser.hpp"

//TEST_CASE("Testing Sample Class") {
//    HTTPMessage msg1 = HTTPMessage::ConnectOKMessage();
//    bool x = msg1.hasHeader(HTTPMessage::CONNECTION);
//    bool y = msg1.hasHeader(HTTPMessage::CONTENT_LENGTH);
//    
//    SECTION("Message headers") {
//        INFO("Using TestStr") // Only appears on a FAIL
//        //CAPTURE(sc.getStr()); // Displays this variable on a FAIL
//        
//        CHECK(x == true);
//        CHECK(y == false);
//        msg1.setHeader(HTTPMessage::CONTENT_LENGTH, "37");
//        
//        y = msg1.hasHeader(HTTPMessage::CONTENT_LENGTH);
//        CHECK(y == true);
//    }
//}

void test1HeaderValues()
{
    std::string teststr = "GET /apath Http/1.1\r\nHost: ahost\r\nConnection: keep-alive\r\nProxy-Connection: keep-alive\r\nContent-Length: 10\r\n\r\n";
    
    std::string teststr2 = "HTTP/1.1 200 OK\r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n";
    std::string teststr3 = "GET /apath HTTP/1.1\r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n";
}



void test_parse_get_with_body()
{
    std::string str1 = "GET / HTTP/1.1\r\nHost: ahost\r\nContent-Length: 10\r\n\r\n9123456789";
    
    const char* buf = str1.c_str();
    int len = str1.length();
    
    HTTPParser parser;
    parser.appendBytes((void*) buf, len);
    
    std::cout << "Parsing ended"  << std::endl;
    
    // need some asserts to ensure it works
}


void test_parse_get()
{
    std::string str3 = "GET /apath HTTP/1.1\r\n\r\n";
    std::string str1 = "GET / HTTP/1.1\r\nHost: ahost\r\nContent-Length: 0\r\n\r\n";

    const char* buf = str1.c_str();
    int len = str1.length();

    HTTPParser parser;
    parser.appendBytes((void*) buf, len);
    
    std::cout << "Parsing ended"  << std::endl;
    
    // need some asserts to ensure it works
}


int main()
{
    std::cout << "developing HTTPMessage" << std::endl;
    
    test_parse_get_with_body();
    return 0;
    
    HTTPMessage msg1 = HTTPMessage::ConnectOKMessage();
    
    msg1.dumpHeaders(std::cout);
    bool x = msg1.hasHeader(HTTPMessage::CONNECTION);
    bool y = msg1.hasHeader(HTTPMessage::CONTENT_LENGTH);
    msg1.setHeader(HTTPMessage::CONTENT_LENGTH, "37");
    
    y = msg1.hasHeader(HTTPMessage::CONTENT_LENGTH);
    
    std::string s1 = msg1.headersAsString();
    std::string ss = msg1.getHeader(HTTPMessage::CONTENT_LENGTH);
    msg1.removeHeader(HTTPMessage::CONTENT_LENGTH);
    y = msg1.hasHeader(HTTPMessage::CONTENT_LENGTH);
    HTTPMessage msg2 = HTTPMessage::ConnectOKMessage();
}
