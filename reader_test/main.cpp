#include <iostream>
#include <ostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <functional>
#include <memory>

#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG);

#include "repeating_timer.hpp"
#include "marvin_error.hpp"
#include "buffer.hpp"
#include "mock_read_socket.hpp"

#include "message_reader.hpp"

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

//----------------------------------------------------------------

class TestReader
{
    
public:
    MessageReader*              rdr_;
    MockReadSocket              conn_;
    boost::asio::io_service&    io_;
    std::string                 body;
    std::ostringstream          bodyStream;
    Testcases                   tcObjs;
    Testcase                    tcObj;
    int                         _tcIndex;


    
    TestReader(boost::asio::io_service& io, int tcIndex)
        : io_(io),
        _tcIndex(tcIndex),
        conn_(MockReadSocket(io, tcIndex)),
        tcObjs(Testcases()),
        tcObj(tcObjs.getCase(tcIndex))
    {
        LogDebug("");
        rdr_ = new MessageReader(conn_, io_);
        body = std::string("");
        bodyStream.str(body);
    }
    ~TestReader()
    {
        LogDebug("");
        delete rdr_;
    }
    void onMessage(Marvin::ErrorType& er)
    {
        LogDebug("");
    }
    void onBody(Marvin::ErrorType& er, FBuffer* fBufPtr)
    {
        LogDebug(" entry");
        // are we done - if not hang another read
        auto bh = std::bind(&TestReader::onBody, this, std::placeholders::_1, std::placeholders::_2);
        bool done = (er == Marvin::make_error_eom());
        
        if( done ){
            bodyStream << *fBufPtr;
            delete fBufPtr;
            std::string msgStr = rdr_->MessageBase::str() + bodyStream.str();
            body = bodyStream.str();
            std::string expectedBody = tcObj.result_body();
            
            bool vb = tcObj.verifyBody(body);
            std::map<std::string, std::string> hh = rdr_->MessageBase::getHeaders();
            bool vh = tcObj.verifyHeaders(hh);
            if( vb && vh ){
                LogDebug("Test for test case", _tcIndex ,"succeeded");
            }else{
                LogDebug("Test for test case", _tcIndex ," FAILED");
                if( ! vb )
                    LogDebug("Body failed ", body, expectedBody );
                if( ! vh ){
                    LogDebug("Headers failed");

                }
            }
        }else{
            // do something with fBuf
            //
            // lets accumulate the FBuffer into a body
            //
            std::string xx = bodyStream.str();
            bodyStream << *fBufPtr;
            delete fBufPtr;
            rdr_->readBody(bh);
        }
        LogDebug("exit");
        
    }
    void onHeaders(Marvin::ErrorType& er){
        LogDebug("entry");
        rdr_->dumpHeaders(std::cout);
        bool x = rdr_->_isRequest;
        auto bh = std::bind(&TestReader::onBody, this, std::placeholders::_1, std::placeholders::_2);
        rdr_->readBody(bh);
        LogDebug("exit");
    }
    void runFullMessageRead()
    {
        LogDebug("getting started");
        auto h = std::bind(&TestReader::onMessage, this, std::placeholders::_1);
        rdr_->readMessage(h);
    }
    void runStreamingBodyRead()
    {
        LogDebug("getting started");
        auto h = std::bind(&TestReader::onHeaders, this, std::placeholders::_1);
        rdr_->readHeaders(h);

    }
};
void testFullMessageReader(int testcase)
{
    boost::asio::io_service io_service;
    auto tr = new TestReader(io_service, testcase);
    tr->runFullMessageRead();
    io_service.run();
    delete tr;
    std::cout << "testMessageReader" << std::endl;
}

void testStreamingReader(int testcase)
{
    boost::asio::io_service io_service;
    auto tr = new TestReader(io_service, testcase);
    tr->runStreamingBodyRead();
    io_service.run();
    delete tr;
    std::cout << "testMessageReader" << std::endl;
}

void TestBuffer(){
    MBuffer* mb = new MBuffer(1000);
    char* t1 = (char*)"123456789";
    char* t2 = (char*)"ABCDEF";
    
    mb->append((void*) t1, strlen(t1));
    mb->append((void*) t2, strlen(t2));
    
    
    char* p = (char*) mb->data();
    bool x1 = mb->contains(p);
    bool x2 = mb->contains(p + 1000);
    bool x3 = mb->contains(p + 1001);
    bool x4 = mb->contains(p - 1);
    std::cout << "" << std::endl;
    
    FBuffer fb1(new MBuffer(1000));
    fb1.copyIn((void*)t1, strlen(t1));
    fb1.copyIn((void*)t2, strlen(t2));
    
    mb = new MBuffer(1000);
    FBuffer fb(mb);
    fb.addFragment((void*)p, 1);
    fb.addFragment((void*)(p+4), 1);
    fb.addFragment((void*)(p+5), 1);
    
    fb.addFragment((void*)(p+7), 90);
    fb.addFragment((void*)(p+101), 100);
    fb.addFragment((void*)p, 1001);
    
    fb.copyIn((void*)t1, strlen(t1));
    fb.copyIn((void*)t2, strlen(t2));
    std::cout << "" << std::endl;

}

int main(){
//    TestBuffer();
    testFullMessageReader(1);

    testStreamingReader(1);
    testStreamingReader(2);
    testStreamingReader(3);
    testStreamingReader(4);
    testStreamingReader(5);
    testStreamingReader(6);
}
