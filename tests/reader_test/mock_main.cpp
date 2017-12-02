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
#include "bufferV2.hpp"
#include "mock_read_socket.hpp"

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
#include "testcase.hpp"
#include "testcase_defs.hpp"
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


void testFullMessageReader(Testcase testcase)
{
    boost::asio::io_service io_service;
    MockReadSocket* msock_ptr = new MockReadSocket(io_service, testcase);
    auto tr = new Testrunner(io_service, msock_ptr, testcase);
    tr->run_FullMessageRead();
    io_service.run();
    delete tr;
}

void testStreamingReader(Testcase testcase)
{
    boost::asio::io_service io_service;
    MockReadSocket* msock_ptr = new MockReadSocket(io_service, testcase);
    auto tr = new Testrunner(io_service, msock_ptr, testcase);
    tr->run_StreamingBodyRead();
    io_service.run();
    delete tr;
}

int main(){
//    TestBuffer();
    RBLogging::setEnabled(false);
    TestcaseDefinitions tcs = makeTestcaseDefinitions_01();
    
#if 0
    testFullMessageReader( tcs.get 1);
    testFullMessageReader(tcs.get_case(2);
    testFullMessageReader(tcs.get_case(3);
    testFullMessageReader(tcs.get_case(4);
    testFullMessageReader(tcs.get_case(5);
    testFullMessageReader(tcs.get_case(6);
#else
    testStreamingReader(tcs.get_case(1));
    testStreamingReader(tcs.get_case(2));
    testStreamingReader(tcs.get_case(3));
    testStreamingReader(tcs.get_case(4));
    testStreamingReader(tcs.get_case(5));
    testStreamingReader(tcs.get_case(6));
    testStreamingReader(tcs.get_case(7));
#endif

}
