#ifndef mock_connection_hpp
#define mock_connection_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "error.hpp"
#include "tcase.hpp"

typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncReadCallback;

/**
 * This class simulates an async/non-blocking sockt connection
 */
class Connection
{
public:
    Connection(boost::asio::io_service& io, int tc): io_(io), _tc(tc), _tcObjs(Testcases()), _tcObj(_tcObjs.getCase(tc))
    {
        index = 0;
        _tc = tc;
        _rdBuf = (char*)malloc(100000);
        st = new SingleTimer(io_, 500);
    }
    ~Connection(){
        delete st;
        std::cout << __FUNCTION__ << std::endl;
        free((void*)_rdBuf);
    }
    void startRead(){
        
    }
    //
    // New buffer strategy make the upper level provide the buffer. All we do is check the size
    //
    void asyncRead(MBuffer& mb, AsyncReadCallback cb){
        LogDebug("");
        char* buf;
        std::size_t len;
        if( ! _tcObj.finished() )
        {
            // we still have some test data so simulate an io wait and then call the read callback
            std::string s = _tcObj.next();
            buf = (char*)s.c_str();
            len = strlen(buf);
            std::size_t buf_max = mb.capacity();
            if( buf_max < len + 1){
                LogError("Connection:asyncRead error buffer too small");
                throw "Connection:asyncRead error buffer too small";
            }
            void* rawPtr = mb.data();
            memcpy(rawPtr, buf, len);
            ((char*)rawPtr)[len] = (char)0;
            
            char* b = (char*)rawPtr;
            std::string bb = std::string((char*)rawPtr, len);
            
            std::size_t startIndex = bb.find("\r\n");
            std::string x;
            if( startIndex != bb.npos ){
                x = bb.replace(startIndex, std::string("\r\n").length(), "\\r\\n");
            }else{
                x = bb;
            }
            LogDebug("::new buffer: [", x, "] len:", len);
            index++;
            
            // at this point we have moved len bytes into the buffer mb
//            st = new SingleTimer(io_, 500);
            st->start([this, buf, len, cb](const boost::system::error_code& ec)->bool{
                
                auto f = [this, buf, len, cb](Marvin::ErrorType& er, std::size_t bytes){
                    cb(er, bytes);
                };
                
                auto pf = std::bind(f, Marvin::make_error_ok(), (std::size_t)len);
                io_.post(pf);
                return true;
            });
        }
        else
        {
            LogDebug("test case finished");
            //  we have run out of test data so simulate an end of input - read of length zero
            ((char*)mb.data())[0] = (char)0;
            auto pf = std::bind(cb, Marvin::make_error_eom(), (std::size_t) 0);
            io_.post(pf);
            return;
        }
    }

private:
    SingleTimer*                st;
    RepeatingTimer*             rt;
    boost::asio::io_service&    io_;
//    MBufferCallback readCb;
    
    Testcases                   _tcObjs;
    Testcase                    _tcObj;
    int                         _tcLineIndex;
    // the test case - vector of strings
    std::vector<std::string> rawData;
    
    // a read buffer malloc'd by the constructor
    char*       _rdBuf;
    
    //index into the array strings representing the test case
    int         index;
    
    // the index into the list of test cases
    int         _tc;
};



#endif