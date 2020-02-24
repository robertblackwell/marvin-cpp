#ifndef marvin_mock_socket_hpp
#define marvin_mock_socket_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "boost_stuff.hpp"
#include "buffer.hpp"
#include "repeating_timer.hpp"
#include "error.hpp"
#include "testcase.hpp"
#include "i_socket.hpp"

typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncReadCallback;
#define TIMER_INTERVAL_MS 10

/**
 * This class simulates an async/non-blocking sockt connection
 */
class MockReadSocket;
typedef std::shared_ptr<MockReadSocket> MockReadSocketSPtr;

class MockReadSocket : public ISocket
{
public:
//    MockReadSocket(boost::asio::io_service& io, int tc); //: io_(io), _tc(tc), _tcObjs(Testcases()), _tcObj(_tcObjs.getCase(tc));
    MockReadSocket(boost::asio::io_service& io, Testcase tc); //: io_(io), _tc(tc), _tcObjs(Testcases()), _tcObj(_tcObjs.getCase(tc));
    ~MockReadSocket();
    void startRead();
    void asyncRead(Marvin::MBufferSPtr mb, AsyncReadCallback cb);
    long nativeSocketFD();

    void asyncWrite(std::string& str, AsyncWriteCallbackType cb);
    void asyncWrite(Marvin::MBuffer& fb, AsyncWriteCallback);
    void asyncWrite(Marvin::BufferChainSPtr chain_sptr, AsyncWriteCallback);
    void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb);
    void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback);
    void asyncConnect(ConnectCallbackType cb);
    void asyncAccept(
        boost::asio::ip::tcp::acceptor& acceptor,
        std::function<void(const boost::system::error_code& err)> cb
    );
    void setReadTimeout(long interval);
    void shutdown();
    void close();

    
private:
    SingleTimer*                st;
    RepeatingTimer*             rt;
    boost::asio::io_service&    io_;
    
//    Testcases                   _tcObjs;
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
