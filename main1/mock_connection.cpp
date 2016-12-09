
#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "RBLogger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "error.hpp"
#include "tcase.hpp"
#include "mock_connection.hpp"

Connection::Connection(boost::asio::io_service& io, int tc): io_(io), _tc(tc), _tcObjs(Testcases()), _tcObj(_tcObjs.getCase(tc))
{
    index = 0;
    _tc = tc;
    _rdBuf = (char*)malloc(100000);
    st = new SingleTimer(io_, 500);
}
Connection::~Connection()
{
    delete st;
    std::cout << __FUNCTION__ << std::endl;
    free((void*)_rdBuf);
}
void Connection::startRead()
{
        
}
//
// New buffer strategy make the upper level provide the buffer. All we do is check the size
//
void Connection::asyncRead(MBuffer& mb, AsyncReadCallback cb)
{
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

