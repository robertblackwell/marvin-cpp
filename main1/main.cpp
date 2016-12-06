#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "HTTPMessage.hpp"

#include "Parser.hpp"

#include "repeating_timer.hpp"
#include "mock_error.hpp"
#include "mock_buffer.hpp"
#include "mock_connection.hpp"

#include "response_reader.hpp"
#include <functional>
#include <memory>

void testRepeatTimer(){
    boost::asio::io_service io_service;
    int n = 6;
    RepeatingTimer rt(io_service, 2000);
    rt.start([&n](const boost::system::error_code& ec)->bool{
        std::cout << __FUNCTION__ << " repeating call back n: " << n << std::endl;
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
    Connection                 conn_;
    boost::asio::io_service&    io_;
    std::string                 body;
    MBuffer*                    bodyBuffer;

    
    TestReader(boost::asio::io_service& io) : io_(io), conn_(Connection(io, 6))
    {
//        Connection c(io, 6);
//        conn_ = c;
        rdr_ = new ResponseReader(conn_, io_);
        
        body = std::string("");
    }
    ~TestReader()
    {
        delete rdr_;
        delete bodyBuffer;
    }
    void onChunkData(Error& er, HTTPMessage* msg){
        std::cout << __FUNCTION__ << std::endl;
    }
    void onEnd(Error& er, HTTPMessage* msg){
        std::cout << __FUNCTION__ << std::endl;
    }
    void onBodyData(Error& er, std::size_t bytes_transfered){
        bodyBuffer->setSize(bytes_transfered);
        MBuffer* buf = bodyBuffer;
        std::string tmp((char*)buf->data(), (std::size_t)buf->size());
        body += tmp;

        std::cout << "TestReader::" << __FUNCTION__ <<  buf <<  std::endl;
        std::cout << "TestReader::" << __FUNCTION__ <<  "body: " << body <<  std::endl;

        auto bh = std::bind(&TestReader::onBodyData, this, std::placeholders::_1, std::placeholders::_2);
        bool done = Error::end_of_message()->equalTo(er);
        
        if( done ){
            std::cout << "Test complete body : " << body << std::endl;
        }else{
            bodyBuffer->empty();
            rdr_->readBodyData(*bodyBuffer, bh);
        }
    }
    void onBodyEnd(Error& er){
        std::cout << "TestReader::" << __FUNCTION__ <<  std::endl;        
    }
    void onResponse(Error& er, MessageInterface* msg){
        std::cout << __FUNCTION__ << std::endl;
        rdr_->dumpHeaders(std::cout);
        bodyBuffer = new MBuffer(20000);
        auto bh = std::bind(&TestReader::onBodyData, this, std::placeholders::_1, std::placeholders::_2);
        rdr_->readBodyData(*bodyBuffer, bh);
    }

    void run(){
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
}

int main(){
    testRequestReader(0);
}
