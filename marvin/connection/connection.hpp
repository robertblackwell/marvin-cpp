
#ifndef marvin_connection_connection_hpp
#define marvin_connection_connection_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <openssl/x509.h>
#include <cert/cert_certificate.hpp>
#include <cert/cert_identity.hpp>

#include <marvin/boost_stuff.hpp>
#include <marvin/error/marvin_error.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/connection/timeout.hpp>

namespace Marvin {

using namespace ::boost;
using namespace ::boost::system;
using namespace ::boost::asio;
using ip::tcp;
using system::error_code;

class Connection;

using ConnectionSPtr = std::shared_ptr<Connection>;
using ConnectionUPtr = std::unique_ptr<Connection>;
/**
* \defgroup connection Connection
* \brief This module contains classes that provide an interface to boost sockets
* for the purposes of transfereing data.
*/
/**
* \ingroup connection
* This class represents a tcp connection that can operate in:
*
* non secure mode - as a plain tcp socket
* secure_client mode -- as a TLS/SSL socket in client mode
* secure_server mode -- as a TLS/SSL socket in server mode
*
* moreover it can start life in non secure mode and either connect
or accept a connection (and even echange traffic) nonsecure and then 
* become secure by executing a handshake 
*/
class Connection : public ISocket
{
    public:
//    using ConstBoostErrRefHandler = std::function<void(const boost::system::error_code& err)>;
//    using ErrRefAndBytesHandler  = std::function<void(ErrorType& err, std::size_t bytes_transfered)>;
//    using ReadHandler = ErrRefAndBytesHandler ;
//    using WriteHandler = ErrRefAndBytesHandler ;

    enum Mode {
        NOTSECURE,
        SECURE_CLIENT,
        SECURE_SERVER
    };
    /**
    * \note Configuration - static methods are provided for configuriing the
    * time out values to be used by connections. The one set of config values
    * apply to all connections. These should be set (if needed) at startup
    */
    static long s_connect_timeout_interval_ms;
    static long s_read_timeout_interval_ms;
    static long s_write_timeout_interval_ms;
    static void setConfig_connectTimeOut(long millisecs);
    static void setConfig_readTimeOut(long millisecs);
    static void setConfig_writeTimeOut(long millisecs);
    /**
    * static properties and methods as test and monitor probes so that
    * correct allocation/deallocation of file descriptors can be
    * monitored
    */
    static std::map<int, int>   socket_fds_inuse;
    static void fd_inuse(int fd);
    static void fd_free(int fd);

    /**
    * client socket needs to know who to connect to
    * @param io_service io_service& the runloop on which this connection will run
    * @param scheme     std::string - values are "http" or "https"
    * @param server     std::string - a name such as www.google.com, NOTE no port number on the end
    * @param port       std::string
    */
    Connection(
            boost::asio::io_service& io_service,
            std::string scheme,
            std::string server,
            std::string port
               );
    /**
    ** server socket will be connected via listen/accept
    */
    explicit Connection(
        boost::asio::io_service& io_service
    );
    
    ~Connection();
    void async_connect(ConnectHandler cb) override;
    void async_accept(boost::asio::ip::tcp::acceptor& acceptor, std::function<void(const boost::system::error_code& err)> cb) override;
    
    void become_secure_client(X509_STORE* certificate_store_ptr) override;
    void become_secure_server(Cert::Identity server_identity) override;
    void async_handshake(std::function<void(const boost::system::error_code& err)> cb) override;
    Cert::Certificate get_server_certificate() override;

    void async_write(std::string& str, WriteHandler cb) override;
    
    void async_write(Marvin::BufferChain::SPtr buf_chain_sptr, AsyncWriteCallback cb) override;
    void async_write(Marvin::ContigBuffer& buffer, WriteHandler cb) override;
    
    void async_write(boost::asio::streambuf& sb, AsyncWriteCallback) override;
    void async_write(boost::asio::const_buffer buf, AsyncWriteCallback cb) override;

    void async_write(void* buffer, std::size_t buffer_length, AsyncWriteCallback cb) override;

    /// read with default timeout
    void async_read(boost::asio::streambuf& streambuf,  ReadHandler cb) override;
    void async_read(boost::asio::mutable_buffer buffer,  ReadHandler cb) override;
    void async_read(void* buffer, std::size_t buffer_length,  ReadHandler cb) override;
    void async_read(Marvin::ContigBuffer::SPtr mb,  ReadHandler cb) override;
    /// same as async_read except that can set the read timeout specifically for this read
    void async_read(Marvin::ContigBuffer::SPtr buffer, long timeout_ms, ReadHandler cb);
    void async_read(void* buffer, std::size_t buffer_length, long timeout_ms, ReadHandler cb);

    void shutdown(ISocket::ShutdownType type) override;
    void cancel() override;
    void close() override;
    void set_read_timeout(long millisecs) override;
    long get_read_timeout() override;
    /**
    * Utility getter functions®
    */
    boost::asio::io_service& get_io_context();
    boost::asio::ssl::context& get_ssl_context();
    long native_socket_fd() override;
    std::string scheme();
    std::string server();
    std::string service();
    
public:
    void p_handle_resolve(
        const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator
    );
    
    void p_handle_connect(
        const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator
    );
    void p_start_handshake();
    void p_handle_handshake(const boost::system::error_code& err);

    void p_async_write(void* data, std::size_t size, AsyncWriteCallback cb);

    void completeWithError(Marvin::ErrorType& ec);
    void completeWithSuccess();
    
    void p_post_accept_cb(std::function<void(boost::system::error_code& err)> cb, Marvin::ErrorType err);
    void p_post_connect_cb(ConnectHandler  cb, Marvin::ErrorType err, ISocket* conn);
    void p_post_read_cb(ReadHandler cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    void p_post_write_cb(WriteHandler cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    
    void resolveCompleteWithSuccess();
    void connectionCompleteWithSuccess();

    void p_io_post(std::function<void()> noParam_cb);
    
    Mode                            m_mode;
    std::string                     m_scheme;
    std::string                     m_server;
    std::string                     m_port;
    boost::asio::io_service&        m_io;
    boost::asio::ip::tcp::resolver  m_resolver;
//    boost::asio::ip::tcp::socket    m_boost_socket;

    std::shared_ptr<boost::asio::ssl::context>                                  m_ssl_ctx_sptr;
    std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>>    m_ssl_stream_sptr;
    boost::asio::ip::tcp::socket                                                m_tcp_socket;
    
    ConnectHandler                  m_connect_cb;

    X509_STORE*                     m_certificate_store_ptr;
    Cert::Identity                  m_server_identity;
    Cert::Certificate               m_server_certificate;

    bool                            m_closed_already;
    Timeout                         m_timeout;
    long                            m_connect_timeout_interval_ms;
    long                            m_read_timeout_interval_ms;
    long                            m_write_timeout_interval_ms;
};
} // namespace

#endif
