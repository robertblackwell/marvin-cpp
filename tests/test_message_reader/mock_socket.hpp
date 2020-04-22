#ifndef marvin_mock_socket_hpp
#define marvin_mock_socket_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include <marvin/connection/socket_interface.hpp>

typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncReadCallback;
#define TIMER_INTERVAL_MS 10

/**
 * This class simulates an async/non-blocking sockt connection
 */
class MockReadSocket;
typedef std::shared_ptr<MockReadSocket> MockReadSocketSPtr;

class MockReadSocket : public Marvin::ISocket
{
public:
    MockReadSocket(boost::asio::io_service& io, Testcase tc); 
    void startRead();

    void asyncRead(Marvin::MBufferSPtr mb, long timeout_ms, AsyncReadCallback cb);
    
    void asyncRead(Marvin::MBufferSPtr mb, AsyncReadCallback cb);
    long nativeSocketFD();

    void asyncWrite(std::string& str, Marvin::AsyncWriteCallbackType cb);
    void asyncWrite(Marvin::MBuffer& fb, Marvin::AsyncWriteCallback);
    void asyncWrite(Marvin::BufferChainSPtr chain_sptr, Marvin::AsyncWriteCallback);
    void asyncWrite(boost::asio::const_buffer buf, Marvin::AsyncWriteCallback cb);
    void asyncWrite(boost::asio::streambuf& sb, Marvin::AsyncWriteCallback);
    void asyncConnect(Marvin::ConnectCallbackType cb);
    void asyncAccept(
        boost::asio::ip::tcp::acceptor& acceptor,
        std::function<void(const boost::system::error_code& err)> cb
    );
    void becomeSecureClient(X509_STORE* certificate_store_ptr);
    void becomeSecureServer(Cert::Identity server_identity);
    void asyncHandshake(std::function<void(const boost::system::error_code& err)>);
    Cert::Certificate getServerCertificate();

    void setReadTimeout(long interval);
    long getReadTimeout();
    void cancel();
    void shutdown(ISocket::ShutdownType type);
    void close();
    boost::asio::io_service& getIO();
    boost::asio::ssl::context& getSslContext();

    
private:
    SingleTimer*                m_single_timer;
    RepeatingTimer*             m_repeating_timer;
    boost::asio::io_service&    m_io;
    boost::asio::ssl::context   m_ssl_context;
    Testcase                    m_tcObj;
    int                         m_tcLineIndex;
    // the test case - vector of strings
    std::vector<std::string> rawData;
    
    // a read buffer malloc'd by the constructor
    char*       m_rdBuf;
    
    //index into the array strings representing the test case
    int         m_index;
    
    // the index into the list of test cases
    int         m_tc;
};



#endif
