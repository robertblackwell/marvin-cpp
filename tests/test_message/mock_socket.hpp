#ifndef marvin_mock_socket_hpp
#define marvin_mock_socket_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include "helper_types.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include <marvin/connection/socket_interface.hpp>

namespace Marvin {
namespace Tests {
namespace Message {

typedef std::function<void(Marvin::ErrorType &er, std::size_t bytes_transfered)> AsyncReadCallback;
#define TIMER_INTERVAL_MS 10

/**
 * This class simulates an async/non-blocking sockt connection
 */
class MockReadSocket;

typedef std::shared_ptr<MockReadSocket> MockReadSocketSPtr;

class MockReadSocket : public ISocket
{
public:
    MockReadSocket(boost::asio::io_service &io, DataSource &data_source);

    void startRead();

    void asyncRead(Marvin::ContigBuffer::SPtr mb, long timeout_ms, AsyncReadCallback cb) override;

    void asyncRead(void *buffer, std::size_t buffer_length, long timeout_ms, AsyncReadCallback cb) override;

    void asyncRead(Marvin::ContigBuffer::SPtr mb, AsyncReadCallback cb) override;

    void asyncRead(void *buffer, std::size_t buffer_length, AsyncReadCallback cb) override;

    void asyncRead(boost::asio::mutable_buffer mutablebuffer, AsyncReadCallback cb) override;

    void asyncRead(boost::asio::streambuf &streambuffer, AsyncReadCallback cb) override;

    long nativeSocketFD() override;

    void asyncWrite(std::string &str, Marvin::AsyncWriteCallbackType cb) override;

    void asyncWrite(Marvin::ContigBuffer &fb, Marvin::AsyncWriteCallback) override;

    void asyncWrite(Marvin::BufferChain::SPtr chain_sptr, Marvin::AsyncWriteCallback) override;

    void asyncWrite(boost::asio::const_buffer buf, Marvin::AsyncWriteCallback cb) override;

    void asyncWrite(boost::asio::streambuf &sb, Marvin::AsyncWriteCallback) override;

    void asyncWrite(void *buffer, std::size_t buffer_length, Marvin::AsyncWriteCallback) override;

    void asyncConnect(Marvin::ConnectCallbackType cb) override;

    void asyncAccept(
            boost::asio::ip::tcp::acceptor &acceptor,
            std::function<void(const boost::system::error_code &err)> cb
    ) override;

    void becomeSecureClient(X509_STORE *certificate_store_ptr) override;

    void becomeSecureServer(Cert::Identity server_identity) override;

    void asyncHandshake(std::function<void(const boost::system::error_code &err)>) override;

    Cert::Certificate getServerCertificate() override;

    void setReadTimeout(long interval) override;

    long getReadTimeout() override;

    void cancel() override;

    void shutdown(ISocket::ShutdownType type) override;

    void close() override;

    boost::asio::io_service &getIO() override;

    boost::asio::ssl::context &getSslContext() override;


private:
    SingleTimer *m_single_timer;
    RepeatingTimer *m_repeating_timer;
    boost::asio::io_service &m_io;
    boost::asio::ssl::context m_ssl_context;
    DataSource m_data_source;
    // the test case - vector of strings
    std::vector<std::string> rawData;
    // a read buffer malloc'd by the constructor
    char *m_rdBuf;
};
} // namespace Message
} //namespace Tests
} // namespace Marvin


#endif
