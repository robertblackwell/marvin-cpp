
#ifndef marvin_ssl_connection_hpp
#define marvin_ssl_connection_hpp
/**
* \ingroup SocketIO
*/

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include "boost_stuff.hpp"
#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "buffer.hpp"
#include "i_socket.hpp"
#include "timeout.hpp"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;


using ip::tcp;
using system::error_code;

class SSLConnection;

using SSLConnectionSPtr = std::shared_ptr<SSLConnection>;
using SSLConnectionUPtr = std::unique_ptr<SSLConnection>;
/**
* \ingroup SocketIO
* \brief Wraps a boost ssl socket and conforms to ISocket
*
*/
class SSLConnection : public ISocket
{
    public:
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
    SSLConnection(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port,
            boost::asio::ssl::context &ctx)
               );
    /**
    ** server socket will be connected via listen/accept
    */
    SSLConnection(
        boost::asio::io_service& io_service
    );
    
    ~TCPConnection();
    void asyncConnect(ConnectCallbackType cb);
    void asyncAccept(boost::asio::ip::tcp::acceptor& acceptor, std::function<void(const boost::system::error_code& err)> cb);
    
    void asyncWrite(Marvin::MBuffer& buffer, AsyncWriteCallbackType cb);
    void asyncWrite(std::string& str, AsyncWriteCallbackType cb);
    void asyncWrite(Marvin::BufferChainSPtr buf_chain_sptr, AsyncWriteCallback cb);
    void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb);
    void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback);

    void asyncRead(Marvin::MBufferSPtr mb,  AsyncReadCallbackType cb);
    void shutdown();
    void close();
    
    /**
    * Utility getter functions®
    */
    long nativeSocketFD();
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
    
    std::string                     m_scheme;
    std::string                     m_server;
    std::string                     m_port;
    boost::asio::io_service&        m_io;
    boost::asio::strand             m_strand;
    boost::asio::ip::tcp::resolver  m_resolver;
//    boost::asio::ip::tcp::socket    m_boost_socket;

    boost::asio::ssl::context&      m_ssl_ctx;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_ssl_socket;

    ConnectCallbackType             m_connect_cb;
    bool                            m_closed_already;
    Timeout                         m_timeout;
    long                            m_connect_timeout_interval_ms;
    long                            m_read_timeout_interval_ms;
    long                            m_write_timeout_interval_ms;
};


#endif
