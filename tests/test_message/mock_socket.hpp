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

    void async_read(Marvin::ContigBuffer::SPtr mb, long timeout_ms, AsyncReadCallback cb) override;

    void async_read(void *buffer, std::size_t buffer_length, long timeout_ms, AsyncReadCallback cb) override;

    void async_read(Marvin::ContigBuffer::SPtr mb, AsyncReadCallback cb) override;

    void async_read(void *buffer, std::size_t buffer_length, AsyncReadCallback cb) override;

    void async_read(boost::asio::mutable_buffer mutablebuffer, AsyncReadCallback cb) override;

    void async_read(boost::asio::streambuf &streambuffer, AsyncReadCallback cb) override;

    long native_socket_fd() override;

    void async_write(std::string &str, ISocket::WriteHandler cb) override;

    void async_write(Marvin::ContigBuffer &fb, ISocket::WriteHandler cb) override;

    void async_write(Marvin::BufferChain::SPtr chain_sptr, ISocket::WriteHandler cb) override;

    void async_write(boost::asio::const_buffer buf, ISocket::WriteHandler cb) override;

    void async_write(boost::asio::streambuf &sb, ISocket::WriteHandler) override;

    void async_write(void *buffer, std::size_t buffer_length, ISocket::WriteHandler) override;

    void async_connect(ISocket::ConnectHandler cb) override;

    void async_accept(
            boost::asio::ip::tcp::acceptor &acceptor,
            std::function<void(const boost::system::error_code &err)> cb
    ) override;

    void become_secure_client(X509_STORE *certificate_store_ptr) override;

    void become_secure_server(Cert::Identity server_identity) override;

    void async_handshake(std::function<void(const boost::system::error_code &err)>) override;

    Cert::Certificate get_server_certificate() override;

    void set_read_timeout(long interval) override;

    long get_read_timeout() override;

    void cancel() override;

    void shutdown(ISocket::ShutdownType type) override;

    void close() override;

    boost::asio::io_service &get_io_context() override;

    boost::asio::ssl::context &get_ssl_context() override;


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
