#ifndef mock_connection_hpp
#define mock_connection_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include "boost_stuff.hpp"
#include "bufferV2.hpp"
#include "repeating_timer.hpp"
#include "error.hpp"
#include "testcase.hpp"
#include "read_socket_interface.hpp"

typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncReadCallback;

/**
 * This class simulates an async/non-blocking sockt connection
 */
class MockReadSocket;
typedef std::shared_ptr<MockReadSocket> MockReadSocketSPtr;

class MockReadSocket : public ReadSocketInterface
{
public:
//    MockReadSocket(boost::asio::io_service& io, int tc); //: io_(io), _tc(tc), _tcObjs(Testcases()), _tcObj(_tcObjs.getCase(tc));
    MockReadSocket(boost::asio::io_service& io, Testcase tc); //: io_(io), _tc(tc), _tcObjs(Testcases()), _tcObj(_tcObjs.getCase(tc));
    ~MockReadSocket();
    void startRead();
    void asyncRead(MBuffer& mb, AsyncReadCallback cb);
    long nativeSocketFD();
    
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
