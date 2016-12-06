#ifndef mock_connection_hpp
#define mock_connection_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "mock_error.hpp"

typedef std::function<void(Error& er, std::size_t bytes_transfered)> AsyncReadCallback;

/**
 * This class simulates an async/non-blocking sockt connection
 */
class Connection
{
public:
    Connection(boost::asio::io_service& io, int tc): io_(io)
    {
        index = 0;
        _tc = tc;
        _rdBuf = (char*)malloc(100000);
        
        static std::vector<std::vector<std::string>> test_cases {
        
            // 0 header ends on a buffer boundary with no body data
            std::vector<std::string> {
                "HTTP/1.1 200 OK 11Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Content-length: 10\r\n\r\n",
                "1234567890"
            },
        
            // 1 EOH is signalled by a bllank line
            std::vector<std::string>  {
                "HTTP/1.1 200 OK 11Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Content-length: 10\r\n",
                "\r\n",
                "1234567890"
            },
            // 2 EOH comes with some body data
            std::vector<std::string>  {
                "HTTP/1.1 201 OK 22Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Content-length: 10\r\n\r\nAB",
                "CDEFGHIJ"
            },
            // 3 EOH and EOM at the same time
            std::vector<std::string>  {
                "HTTP/1.1 201 OK 22Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Content-length: 10",
                "\r\n\r\nABCDEFGHIJ"
            },
            // 4 Chunked with headers on a boundary
            std::vector<std::string> {
                "HTTP/1.1 201 OK Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Transfer-encoding: chunked\r\n\r\n",
                "0a\r\n1234567890\r\n",
                "0f\r\n1234567890XXXXX\r\n",
                "0a\r\n1234567890\r\n",
                "0f\r\n1234567890HGHGH\r\n",
                "0a\r\n1234567890\r\n",
                "0\r\n",
                "\r\n"
            },
            // 5 Chunked with headers with some body data - chunks not broken
            std::vector<std::string> {
                "HTTP/1.1 201 OK Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Transfer-Encoding: chunked\r\n",
                "\r\n0a\r\n1234567890\r\n",
                "0f\r\n1234567890XXXXX\r\n",
                "0a\r\n1234567890\r\n",
                "0f\r\n1234567890HGHGH\r\n",
                "0a\r\n1234567890\r\n",
                "0\r\n",
                "\r\n"
            },
            // 6 Chunked with headers with some body data - BROKEN chunks
            std::vector<std::string> {
                "HTTP/1.1 201 OK Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Transfer-Encoding: chunked\r\n",
                "\r\n0a\r\n123456",
                "7890\r\n",
                "0f\r\n123456",
                "7890XXXXX\r\n0a\r\n1234567890\r\n",
                "0f\r\n1234567890HGHGH\r\n",
                "0a\r\n1234567890\r\n",
                "0\r\n",
                "\r\n"
            }
        };
//        char** test_cases[7] = {str0, str1, str2, str3, str4, str5, str6};
//        
        rawData = test_cases[_tc];
//        readCb = NULL;
        st = NULL;
        
    }
    ~Connection(){
        std::cout << __FUNCTION__ << std::endl;
        free((void*)_rdBuf);
    }
    void startRead(){
        
    }
    //
    // New buffer strategy make the upper level provide the buffer. All we do is check the size
    //
    void asyncRead(MBuffer& mb, AsyncReadCallback cb){
        int c = (int)rawData.size();
        char* buf;
        std::size_t len;
        if( index < c )
        {
            // we still have some test data so simulate an io wait and then call the read callback
            std::string s = rawData[index];
            buf = (char*)s.c_str();
            len = strlen(buf);
            std::size_t buf_max = mb.capacity();
            if( buf_max < len + 1){
                std::cout << "Connection:asyncRead error buffer too small";
                throw "Connection:asyncRead error buffer too small";
            }
            void* rawPtr = mb.data();
            memcpy(rawPtr, buf, len);
            ((char*)rawPtr)[len] = (char)0;
            
            char* b = (char*)rawPtr;
            std::cout << __FUNCTION__ << "::new buffer: " << b << " len: " << len << std::endl;
            index++;
            
            // at this point we have moved len bytes into the buffer mb
            st = new SingleTimer(io_, 1000);
            st->start([this, buf, len, cb](const boost::system::error_code& ec)->bool{
                auto f = [this, buf, len, cb](Error& er, std::size_t bytes){
                    std::cout << "Connection::asyncRead::start::lambda" << std::endl;
                    cb(er, bytes);
                    std::cout << "Connection::asyncRead::start::lambda" << std::endl;
                };
                auto pf = std::bind(f, Error::ok(), (std::size_t)len);
                io_.post(pf);
                return true;
            });
        }
        else
        {
            //  we have run out of test data so simulate an end of input - read of length zero
            ((char*)mb.data())[0] = (char)0;
            auto pf = std::bind(cb, Error::eom(), (std::size_t) 0);
            io_.post(pf);
            return;
        }
    }

private:
    SingleTimer*    st;
    RepeatingTimer* rt;
    boost::asio::io_service& io_;
//    MBufferCallback readCb;
    
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