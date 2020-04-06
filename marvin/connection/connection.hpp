
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
#include <marvin/callback_typedefs.hpp>
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
    void asyncConnect(ConnectCallbackType cb) override;
    void asyncAccept(boost::asio::ip::tcp::acceptor& acceptor, std::function<void(const boost::system::error_code& err)> cb) override;
    
    void becomeSecureClient(X509_STORE* certificate_store_ptr) override;
    void becomeSecureServer(Cert::Identity server_identity) override;
    void asyncHandshake(std::function<void(const boost::system::error_code& err)> cb) override;
    Cert::Certificate getServerCertificate() override;

    void asyncWrite(Marvin::MBuffer& buffer, AsyncWriteCallbackType cb) override;
    void asyncWrite(std::string& str, AsyncWriteCallbackType cb) override;
    void asyncWrite(Marvin::BufferChainSPtr buf_chain_sptr, AsyncWriteCallback cb) override;
    void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb) override;
    void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback) override;

    /// read with default timeout
    void asyncRead(Marvin::MBufferSPtr mb,  AsyncReadCallbackType cb) override;
    /// same as asyncRead except that can set the read timeout specifically for this read
    void asyncRead(Marvin::MBufferSPtr buffer, long timeout_ms, AsyncReadCallbackType cb);

    void shutdown(ISocket::ShutdownType type) override;
    void cancel() override;
    void close() override;
    void setReadTimeout(long millisecs) override;
    long getReadTimeout() override;
    /**
    * Utility getter functions®
    */
    boost::asio::io_service& getIO();
    long nativeSocketFD() override;
    std::string scheme();
    std::string server();
    std::string service();
    
private:
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
    void p_post_connect_cb(ConnectCallbackType  cb, Marvin::ErrorType err, ISocket* conn);
    void p_post_read_cb(AsyncReadCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    void p_post_write_cb(AsyncWriteCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    
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

    /// our own copy not just a reference
    X509_STORE*                     m_certificate_store_ptr;
    Cert::Identity                  m_server_identity;
    boost::asio::ssl::context       m_ssl_ctx;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_ssl_socket;
    boost::asio::ip::tcp::socket::lowest_layer_type& m_lowest_layer_sock;// = ssl_sock.lowest_layer();
    ConnectCallbackType             m_connect_cb;

    // only defined after client connection completes handshake
    Cert::Certificate               m_server_certificate;

    bool                            m_closed_already;
    Timeout                         m_timeout;
    long                            m_connect_timeout_interval_ms;
    long                            m_read_timeout_interval_ms;
    long                            m_write_timeout_interval_ms;
};
} // namespace

#endif
