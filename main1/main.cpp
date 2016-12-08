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

#include "RBLogger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG);

#include "repeating_timer.hpp"
#include "marvin_error.hpp"
#include "buffer.hpp"
#include "mock_connection.hpp"

#include "response_reader.hpp"

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
//----------------------------------------------------------------

class TestReader
{
    
public:
    ResponseReader*             rdr_;
    Connection                  conn_;
    boost::asio::io_service&    io_;
    std::string                 body;
    std::ostringstream          bodyStream;


    
    TestReader(boost::asio::io_service& io) : io_(io), conn_(Connection(io, 6))
    {
        rdr_ = new ResponseReader(conn_, io_);
        body = std::string("");
        bodyStream.str(body);
    }
    ~TestReader()
    {
        delete rdr_;
    }

    void onBody(Marvin::ErrorType& er, FBuffer* fBufPtr)
    {
        LogDebug(" entry");
        // are we done - if not hang another read
        auto bh = std::bind(&TestReader::onBody, this, std::placeholders::_1, std::placeholders::_2);
        bool done = (er == Marvin::make_error_eom());
        
        if( done ){
            delete fBufPtr;
            body = bodyStream.str();
            LogDebug("EOB Test complete body :",  bodyStream.str());
            //
            // Here should test we have the correct headers and body
            //
        }else{
            // do something with fBuf
            //
            // lets accumulate the FBuffer into a body
            //
            bodyStream << *fBufPtr;
            delete fBufPtr;
            rdr_->readBody(bh);
        }
        LogDebug("exit");
        
    }
    void onResponse(Marvin::ErrorType& er, MessageInterface* msg){
        LogDebug("entry");
        rdr_->dumpHeaders(std::cout);

        auto bh = std::bind(&TestReader::onBody, this, std::placeholders::_1, std::placeholders::_2);
        rdr_->readBody(bh);
        LogDebug("exit");
    }

    void run(){
        LogDebug("getting started");
        auto h = std::bind(&TestReader::onResponse, this, std::placeholders::_1, std::placeholders::_2);
        rdr_->readResponse(h);

    }
};

void testRequestReader(int testcase)
{

    boost::asio::io_service io_service;
    auto tr = std::unique_ptr<TestReader>(new TestReader(io_service));
    tr->run();
    io_service.run();
    std::cout << "testResponseReader" << std::endl;
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
    testRequestReader(0);
}
