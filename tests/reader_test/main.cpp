#include <iostream>
#include <ostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include "boost_stuff.hpp"
#include <functional>
#include <memory>

#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG);


#include "repeating_timer.hpp"
#include "marvin_error.hpp"
#include "buffer.hpp"
#include "mock_read_socket.hpp"

#define MESSAGE_READER_V2
#ifndef MESSAGE_READER_V2
#include "message_reader.hpp"
class MyMessageReader : public MessageReader
{
public:
    MyMessageReader(ReadSocketInterface* readSock, boost::asio::io_service& io)
    : MessageReader(readSock, io)
    {
    
    }
    ~MyMessageReader()
    {
    
    }
};
#else
#include "message_reader_v2.hpp"
class MyMessageReader : public MessageReaderV2
{
public:
    MyMessageReader(ReadSocketInterface* readSock, boost::asio::io_service& io)
    : MessageReaderV2(readSock, io)
    {
    
    }
    ~MyMessageReader()
    {
    
    }

};
#endif

#include "test_runner.cpp"

void testRepeatTimer(){
    boost::asio::io_service io_service;
    int n = 6;
    RepeatingTimer rt(io_service, 2000);
    rt.start([&n](const boost::system::error_code& ec)->bool{
        LogDebug(" repeating call back n:", n);
        if( n == 0) return false;
        n--;
        return true;
    });
    io_service.run();
    
}
void stringDiff(std::string s1, std::string s2){
    if( s1.size() != s2.size() )
        std::cout << "s1.size: " << s1.size() << " s2.size(): " << s2.size() << std::endl;
    for(int i = 0; i < s1.size(); i++){
        if( s1.c_str()[i] != s2.c_str()[i] ){
            std::cout << "differ at position " << i << std::endl;
            std::cout << "s1: " << s1.c_str()[i] << " s2: " << s2.c_str()[i] << std::endl;
        }
    }
}


void testFullMessageReader(int testcase)
{
    boost::asio::io_service io_service;
    auto tr = new TestRunner(io_service, testcase);
    tr->run_FullMessageRead();
    io_service.run();
    delete tr;
    std::cout << "testMessageReader" << std::endl;
}

void testStreamingReader(int testcase)
{
    boost::asio::io_service io_service;
    auto tr = new TestRunner(io_service, testcase);
    tr->run_StreamingBodyRead();
    io_service.run();
    delete tr;
    std::cout << "testMessageReader" << std::endl;
}

int main(){
//    TestBuffer();
    RBLogging::setEnabled(false);
#if 0
    testFullMessageReader(1);
    testFullMessageReader(2);
    testFullMessageReader(3);
    testFullMessageReader(4);
    testFullMessageReader(5);
    testFullMessageReader(6);
#else
    testStreamingReader(1);
    testStreamingReader(2);
    testStreamingReader(3);
    testStreamingReader(4);
    testStreamingReader(5);
    testStreamingReader(6);
    testStreamingReader(7);
#endif

}
