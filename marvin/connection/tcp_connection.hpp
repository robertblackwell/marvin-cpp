
#ifndef tcp_connection_hpp
#define tcp_connection_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>

//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/function.hpp>

#include "boost_stuff.hpp"

#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "read_socket_interface.hpp"
#include "buffer.hpp"
#include "connection_interface.hpp"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;


using ip::tcp;
using system::error_code;


class TCPConnection;
//typedef std::shared_ptr<TCPConnection> TCPConnectionSPtr;
//typedef std::unique_ptr<TCPConnection> TCPConnectionUPtr;

using TCPConnectionSPtr = std::shared_ptr<TCPConnection>;
using TCPConnectionUPtr = std::unique_ptr<TCPConnection>;
//--------------------------------------------------------------------------------------------------
// NON SSL/TLS Connection
//--------------------------------------------------------------------------------------------------
class TCPConnection : public ConnectionInterface
{
    public:
    /**
    * Configuration - static methods are provided for configuriing the
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
    TCPConnection(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
               );
    /**
    ** server socket will be connected via listen/accept
    */
    TCPConnection(
        boost::asio::io_service& io_service
    );
    
    ~TCPConnection();
    /**
    * A Note on timeouts, the use of a boost::asio::strand by this class, and
    * concurrent calls to the async io methods defined below.
    *
    * 1. Only one async I/O operation should be outstanding at any time.
    * 2. Internally this class applies timeouts to the operations:
    *       -   asyncConnect
    *       -   asyncRead
    *       -   asyncWrite in all its forms
    *   to accomodate this an instance of boost::asio::strand is used to ensure
    *   that internal io handlers coordinate correctly and do not run concurrently.
    *
    *   HOwever all callbacks provided to asyncConnect, asyncRead, asyncWrite are scheduled (using _io.post())on
    *   the instance of io_service being used by the TCPConnection instance and ARE NOT
    *   running on the internal strand. These callbacks should NOT be wrapped by another strand as
    *   that could cause deadlocks.
    *
    *   Notice that asyncAccept was NOT included in the list of io ops subject to timeout.
    *
    *       -   asyncAccept is intended to be used by a server and could be outstanding for a very long time,
    *           and hence a timeout seems inappropriate
    *       -   because of no timeout there is no need for internal handlers associated with asyncAccept
    *           to be run on the internal asio::strand.
    *       -   it is likely that asyncAccept callback will be wrapped by a strand within a server as part
    *           of a servers management of its own concurrency issues.
    */
    void asyncConnect(ConnectCallbackType cb);
    void asyncAccept(boost::asio::ip::tcp::acceptor& acceptor, std::function<void(const boost::system::error_code& err)> cb);
    
    void asyncWrite(MBuffer& buffer, AsyncWriteCallbackType cb);
    void asyncWrite(std::string& str, AsyncWriteCallbackType cb);
    void asyncWrite(BufferChainSPtr buf_chain_sptr, AsyncWriteCallback cb);
    void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb);
    void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback);

    void asyncRead(MBuffer& mb,  AsyncReadCallbackType cb);
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

    void handle_resolve(
        const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator
    );
    
    void handle_connect(
        const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator
    );

    void async_write(void* data, std::size_t size, AsyncWriteCallback cb);

    void completeWithError(Marvin::ErrorType& ec);
    void completeWithSuccess();
    
    void post_accept_cb(std::function<void(boost::system::error_code& err)> cb, Marvin::ErrorType err);
    void post_connect_cb(ConnectCallbackType  cb, Marvin::ErrorType err, ConnectionInterface* conn);
    void post_read_cb(AsyncReadCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    void post_write_cb(AsyncWriteCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    
    void resolveCompleteWithSuccess();
    void connectionCompleteWithSuccess();

    void io_post(std::function<void()> noParam_cb);
    
    void handle_timeout(const boost::system::error_code& err);
    void cancel_timeout();
    void set_timeout(long interval_millisecs);


    std::string                     m_scheme;
    std::string                     m_server;
    std::string                     m_port;
    boost::asio::io_service&        m_io;
    boost::asio::strand             m_strand;
    boost::asio::ip::tcp::resolver  m_resolver;
    boost::asio::ip::tcp::socket    m_boost_socket;
    ConnectCallbackType             m_connect_cb;
    bool                            m_closed_already;
    boost::asio::deadline_timer     m_timer;
    long                            m_connect_timeout_interval_ms;
    long                            m_read_timeout_interval_ms;
    long                            m_write_timeout_interval_ms;
};


#endif
