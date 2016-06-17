#pragma mark simple parsing
TEST_CASE("Simple GET") {

    std::string str1 = "GET / HTTP/1.1\r\nHost: ahost\r\nContent-Length: 0\r\n\r\n";
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str1); // Displays this variable on a FAIL
    
    const char* buf = str1.c_str();
    int len = str1.length();
    
    HTTPParser parser;
    int nparsed = parser.appendBytes((void*) buf, len);
    
    CHECK(nparsed == len);
    
    CHECK(parser.last_message->methodAsString() == "GET");
    CHECK(parser.last_message->hasHeader("Host") );
    CHECK(parser.last_message->hasHeader("Content-Length") );
    CHECK(parser.last_message->getHeader("Content-Length") == "0" );
    CHECK(parser.last_message->body->length() == 0);

}

TEST_CASE("Simple GET with body") {
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
    int nparsed = parser.appendBytes((void*) buf, len);

    CHECK(nparsed == len);
    CHECK(parser.last_message->methodAsString() == "GET");
    CHECK(parser.last_message->hasHeader("Host") );
    CHECK(parser.last_message->headers.size() == 4 );
    CHECK(parser.last_message->hasHeader("Content-Length") );
    CHECK(parser.last_message->hasHeader("Connection") );
    CHECK(parser.last_message->getHeader("Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Proxy-Connection") );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );
    CHECK(parser.last_message->getHeader("Content-Length") == "10" );
    CHECK(parser.last_message->body->length() == 10);
}

TEST_CASE("Simple response") {
    
    
    std::string str1 =   std::string("HTTP/1.1 201 OK Reason Phrase\r\n") +
                             std::string("Host: ahost\r\n") +
                             std::string("Connection: keep-alive\r\n") +
                             std::string("Proxy-Connection: keep-alive\r\n") +
                             std::string("Content-Length: 0\r\n") +
                             std::string("\r\n");

    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str1); // Displays this variable on a FAIL
    
    const char* buf = str1.c_str();
    int len = str1.length();
    
    HTTPParser parser;
    int nparsed = parser.appendBytes((void*) buf, len);
    
    CHECK(nparsed == len);

    CHECK(parser.last_message->firstLineAsString() == "HTTP/1.1 201 OK Reason Phrase");
    CHECK(parser.last_message->isRequest() == false );
    CHECK(parser.last_message->status_code == 201 );
    CHECK(parser.last_message->status == "OK Reason Phrase" );
    CHECK(parser.last_message->hasHeader("Host") );
    CHECK(parser.last_message->headers.size() == 4 );
    CHECK(parser.last_message->hasHeader("Content-Length") );
    CHECK(parser.last_message->hasHeader("Connection") );
    CHECK(parser.last_message->getHeader("Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Proxy-Connection") );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Content-Length") );
    CHECK(parser.last_message->getHeader("Content-Length") == "0" );
    CHECK(parser.last_message->body->length() == 0);
    
}

TEST_CASE("Response with data") {
    
    
    std::string str1 =   std::string("HTTP/1.1 201 OK Reason Phrase\r\n") +
    std::string("Host: ahost\r\n") +
    std::string("Connection: keep-alive\r\n") +
    std::string("Proxy-Connection: keep-alive\r\n") +
    std::string("Content-Length: 10\r\n\r\n") +
    std::string("987654321q");
    
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str1); // Displays this variable on a FAIL
    
    const char* buf = str1.c_str();
    int len = str1.length();
    
    HTTPParser parser;
    int nparsed = parser.appendBytes((void*) buf, len);
    
    CHECK(nparsed == len);
    
    CHECK(parser.last_message->firstLineAsString() == "HTTP/1.1 201 OK Reason Phrase");
    CHECK(parser.last_message->isRequest() == false );
    CHECK(parser.last_message->status_code == 201 );
    CHECK(parser.last_message->status == "OK Reason Phrase" );
    CHECK(parser.last_message->hasHeader("Host") );
    CHECK(parser.last_message->headers.size() == 4 );
    CHECK(parser.last_message->hasHeader("Content-Length") );
    CHECK(parser.last_message->hasHeader("Connection") );
    CHECK(parser.last_message->getHeader("Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Proxy-Connection") );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Content-Length") );
    CHECK(parser.last_message->getHeader("Content-Length") == "10" );
    CHECK(parser.last_message->body->length() == 10);
    CHECK(parser.last_message->bodyAsString() == "987654321q" );
    
}

#pragma mark chunked responses

TEST_CASE("CHUNKED data Response ") {
    
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
    CAPTURE(str); // Displays this variable on a FAIL
    
    const char* buf;
    int len;
    
    HTTPParser parser;
    int nparsed;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        buf = str[i];
        len = strlen(buf);
        nparsed = parser.appendBytes((void*) buf, len);
        
        CHECK(nparsed == len);
    }
    CHECK(parser.last_message->firstLineAsString() == "HTTP/1.1 201 OK Reason Phrase");
    CHECK(parser.last_message->isRequest() == false );
    CHECK(parser.last_message->status_code == 201 );
    CHECK(parser.last_message->status == "OK Reason Phrase" );
    CHECK(parser.last_message->hasHeader("Host") );
    CHECK(parser.last_message->headers.size() == 4 );
    CHECK(parser.last_message->hasHeader("Connection") );
    CHECK(parser.last_message->getHeader("Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Proxy-Connection") );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Transfer-Encoding") );
    CHECK(parser.last_message->getHeader("Transfer-Encoding") == "chunked" );
//    CHECK(parser.last_message->bodyAsString() == "987654321q" );
    
}

#pragma mark header set/remove

TEST_CASE("set/remove headers ") {
    
    char* str[] = {
        (char*) "HTTP/1.1 201 OK Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n",
        (char*) "\r\n",
        NULL
    };
    
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str); // Displays this variable on a FAIL
    
    const char* buf;
    int len;
    
    HTTPParser parser;
    int nparsed;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        buf = str[i];
        len = strlen(buf);
        nparsed = parser.appendBytes((void*) buf, len);
        
        CHECK(nparsed == len);
    }
    CHECK(parser.last_message->firstLineAsString() == "HTTP/1.1 201 OK Reason Phrase");
    CHECK(parser.last_message->isRequest() == false );
    CHECK(parser.last_message->status_code == 201 );
    CHECK(parser.last_message->status == "OK Reason Phrase" );
    CHECK(parser.last_message->hasHeader("Host") );
    CHECK(parser.last_message->headers.size() == 3 );
    CHECK(parser.last_message->hasHeader("Connection") );
    CHECK(parser.last_message->getHeader("Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Proxy-Connection") );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );
    
    parser.last_message->removeHeader("Proxy-Connection");
    CHECK(parser.last_message->hasHeader("Proxy-Connection") == false );
    
    parser.last_message->setHeader("Connection", "close");
    CHECK(parser.last_message->getHeader("Connection")=="close" );

    parser.last_message->setHeader("Proxy-Connection","keep-alive");
    CHECK(parser.last_message->hasHeader("Proxy-Connection") == true );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );

    
    //    CHECK(parser.last_message->bodyAsString() == "987654321q" );
    
}

#pragma mark test error handling

TEST_CASE("test error handling ") {
    
    char* str[] = {
        (char*) "HTTP/1.1 201 OK Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n",
        (char*) "\r\n",
        NULL
    };
    
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str); // Displays this variable on a FAIL
    
    const char* buf;
    int len;
    
    HTTPParser parser;
    int nparsed;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        buf = str[i];
        len = strlen(buf);
        nparsed = parser.appendBytes((void*) buf, len);
        
        CHECK(nparsed == len);
    }
    CHECK(parser.last_message->firstLineAsString() == "HTTP/1.1 201 OK Reason Phrase");
    CHECK(parser.last_message->isRequest() == false );
    CHECK(parser.last_message->status_code == 201 );
    CHECK(parser.last_message->status == "OK Reason Phrase" );
    CHECK(parser.last_message->hasHeader("Host") );
    CHECK(parser.last_message->headers.size() == 3 );
    CHECK(parser.last_message->hasHeader("Connection") );
    CHECK(parser.last_message->getHeader("Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Proxy-Connection") );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );
    
    parser.last_message->removeHeader("Proxy-Connection");
    CHECK(parser.last_message->hasHeader("Proxy-Connection") == false );
    
    parser.last_message->setHeader("Connection", "close");
    CHECK(parser.last_message->getHeader("Connection")=="close" );
    
    parser.last_message->setHeader("Proxy-Connection","keep-alive");
    CHECK(parser.last_message->hasHeader("Proxy-Connection") == true );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );
    
    
    //    CHECK(parser.last_message->bodyAsString() == "987654321q" );
    
}

#pragma mark test callbacks

TEST_CASE("test call backs ") {
    
    char* str[] = {
        (char*) "HTTP/1.1 201 OK Reason Phrase\r\n",
        (char*) "Host: ahost\r\n",
        (char*) "Connection: keep-alive\r\n",
        (char*) "Proxy-Connection: keep-alive\r\n",
        (char*) "\r\n",
        NULL
    };
    
    INFO("Using TestStr") // Only appears on a FAIL
    CAPTURE(str); // Displays this variable on a FAIL
    
    const char* buf;
    int len;
    
    HTTPParser parser;
    int nparsed;
    for(int i = 0; str[i] != NULL  ;i++)
    {
        buf = str[i];
        len = strlen(buf);
        nparsed = parser.appendBytes((void*) buf, len);
        
        CHECK(nparsed == len);
    }
    CHECK(parser.last_message->firstLineAsString() == "HTTP/1.1 201 OK Reason Phrase");
    CHECK(parser.last_message->isRequest() == false );
    CHECK(parser.last_message->status_code == 201 );
    CHECK(parser.last_message->status == "OK Reason Phrase" );
    CHECK(parser.last_message->hasHeader("Host") );
    CHECK(parser.last_message->headers.size() == 3 );
    CHECK(parser.last_message->hasHeader("Connection") );
    CHECK(parser.last_message->getHeader("Connection")=="keep-alive" );
    CHECK(parser.last_message->hasHeader("Proxy-Connection") );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );
    
    parser.last_message->removeHeader("Proxy-Connection");
    CHECK(parser.last_message->hasHeader("Proxy-Connection") == false );
    
    parser.last_message->setHeader("Connection", "close");
    CHECK(parser.last_message->getHeader("Connection")=="close" );
    
    parser.last_message->setHeader("Proxy-Connection","keep-alive");
    CHECK(parser.last_message->hasHeader("Proxy-Connection") == true );
    CHECK(parser.last_message->getHeader("Proxy-Connection")=="keep-alive" );
    
    
    //    CHECK(parser.last_message->bodyAsString() == "987654321q" );
    
}
