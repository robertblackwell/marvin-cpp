#include <ostream>
#include <string>
#include <cassert>

#include <marvin/boost_stuff.hpp>
#include <marvin/error/marvin_error.hpp>
#include <marvin/callback_typedefs.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

#include <marvin/connection/connection.hpp>

using boost::asio::ip::tcp;
using boost::bind;
using boost::function;
using boost::system::error_code;
using boost::asio::io_service;

#pragma mark - timeout interval config
long Connection::s_connect_timeout_interval_ms = 500000;
long Connection::s_read_timeout_interval_ms = 500000;
long Connection::s_write_timeout_interval_ms = 500000;
void Connection::setConfig_connectTimeOut(long millisecs)
{
    s_connect_timeout_interval_ms = millisecs;
}
void Connection::setConfig_readTimeOut(long millisecs)
{
    s_read_timeout_interval_ms  = millisecs;
}
void Connection::setConfig_writeTimeOut(long millisecs)
{
    s_write_timeout_interval_ms = millisecs;
}
#pragma mark - test and paranoia probes
std::map<int, int> Connection::socket_fds_inuse;
void Connection::fd_inuse(int fd)
{
    assert(socket_fds_inuse.find(fd) == socket_fds_inuse.end());
    socket_fds_inuse[fd] = fd;
}
void Connection::fd_free(int fd)
{
    assert(socket_fds_inuse.find(fd) != socket_fds_inuse.end());
    socket_fds_inuse.erase(fd);
}
#pragma mark - CTOR
/**
 * Constructor
 *  @param {io_service} io_service  -   to use for running
 *  @param {string}     scheme      -   "http" or "https"
 *  @param {string}     server      -   domain string
 */
Connection::Connection(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
            )
            :
            m_mode(NOTSECURE),
            m_io(io_service),
            m_timeout(m_io),
            m_resolver(m_io),
//            m_ssl_ctx(std::move(ctx)),
            m_ssl_ctx(boost::asio::ssl::context::method::sslv23),
            m_ssl_socket(m_io, m_ssl_ctx),
            m_lowest_layer_sock(m_ssl_socket.lowest_layer()),
            m_scheme(scheme),
            m_server(server),
            m_port(port),
            m_connect_cb(nullptr),
            m_closed_already(false),
            m_connect_timeout_interval_ms(s_connect_timeout_interval_ms),
            m_read_timeout_interval_ms(s_read_timeout_interval_ms),
            m_write_timeout_interval_ms(s_write_timeout_interval_ms)
{
    LogTorTrace();
    auto x = nativeSocketFD();
    boost::asio::ip::tcp::socket& ll = m_ssl_socket.next_layer();
    LogDebug("constructor:: native handle :: ", x);
}


Connection::Connection(
    boost::asio::io_service& io_service
    ):
        m_mode(NOTSECURE),
        m_io(io_service),
        m_timeout(m_io),
        m_resolver(m_io), // dont really need this
//            m_ssl_ctx(std::move(ctx)),
        m_ssl_ctx(boost::asio::ssl::context::method::sslv23),
        m_ssl_socket(m_io, m_ssl_ctx),
        m_lowest_layer_sock(m_ssl_socket.lowest_layer()),
        m_connect_timeout_interval_ms(s_connect_timeout_interval_ms),
        m_read_timeout_interval_ms(s_read_timeout_interval_ms),
        m_write_timeout_interval_ms(s_write_timeout_interval_ms)

{
    LogTorTrace();
}
Connection::~Connection()
{
    LogTorTrace();
    if( ! m_closed_already) {
        LogFDTrace(nativeSocketFD());
        m_ssl_socket.lowest_layer().close();
    }
}
#pragma mark - public interface getters
std::string Connection::scheme(){return m_scheme;}
std::string Connection::server(){return m_server;}
std::string Connection::service(){return m_port;}

long Connection::nativeSocketFD()
{
    return m_lowest_layer_sock.native_handle();
}

#pragma mark - public interface connection management
void Connection::close()
{
    LogFDTrace(nativeSocketFD());
    assert(! m_closed_already);
    m_closed_already = true;
    m_lowest_layer_sock.cancel();
    m_lowest_layer_sock.close();
}
void Connection::shutdown()
{
    assert(! m_closed_already);
    m_lowest_layer_sock.shutdown(boost::asio::socket_base::shutdown_both);
}
void Connection::setReadTimeout(long millisecs)
{
    m_read_timeout_interval_ms = millisecs;
}

#pragma mark - public interface async io operations
void Connection::asyncAccept(
    boost::asio::ip::tcp::acceptor&                     acceptor,
    std::function<void(const boost::system::error_code& err)> cb
)
{
    acceptor.async_accept(m_lowest_layer_sock, [cb, this](const boost::system::error_code& err) {
        LogFDTrace(this->nativeSocketFD());
        p_post_accept_cb(cb, err);
    });
}

void Connection::asyncConnect(ConnectCallbackType connect_cb)
{
    m_connect_cb = connect_cb; // save the connect callback

    tcp::resolver::query query(this->m_server, this->m_port);
#if 1
    m_timeout.setTimeout(m_connect_timeout_interval_ms, [this](){
        m_lowest_layer_sock.cancel();
    });
    auto h = ([this](const error_code& err, tcp::resolver::iterator endpoint_iterator) {
        m_timeout.cancelTimeout([this, err, endpoint_iterator](){
            p_handle_resolve(err, endpoint_iterator);
        });
    });
    m_resolver.async_resolve(query, h);
#else
    auto handler = (std::bind(&Connection::p_handle_resolve,this, std::placeholders::_1, std::placeholders::_2));
    m_resolver.async_resolve(query, handler);
#endif
}

void Connection::becomeSecureClient(X509_STORE* certificate_store_ptr)
{
    if (m_mode != NOTSECURE) {
        THROW("connection already secured");
    }
    m_mode = Mode::SECURE_CLIENT;
    m_certificate_store_ptr = certificate_store_ptr;
}
void Connection::becomeSecureServer(Cert::Identity server_identity)
{
    if (m_mode != NOTSECURE) {
        THROW("connection already secured");
    }
    m_mode = Mode::SECURE_SERVER;
    m_server_identity = server_identity;
}
void Connection::asyncHandshake(std::function<void(const boost::system::error_code& err)> cb)
{
    m_connect_cb = [this, cb](Marvin::ErrorType& err, ISocket* conn) {
        cb(err);
    };
    p_start_handshake();
}
Cert::Certificate Connection::getServerCertificate()
{
    if (!m_server_certificate) {
        THROW("cannot get server certificate until after successful handshake");
    }
    return m_server_certificate;
}

/**
 * read
 */
void Connection::asyncRead(Marvin::MBufferSPtr buffer, AsyncReadCallbackType cb)
{
    /// a bit of explanation -
    /// -   set a time out with a handler, the handler knows what to do, in this case cancel outstanding
    ///     ops on the socket
    m_timeout.setTimeout(m_read_timeout_interval_ms, [this](){
        m_lowest_layer_sock.cancel();
    });
    auto handler = ([this, cb, buffer](const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        /// when a handler is called the first thing to do is call timeout.cancel()
        /// when timeout object is finshed it will call the CB and then we can conlete
        /// out processing knowing that both the IO and tineout are both done
        m_timeout.cancelTimeout([this, cb, buffer, err, bytes_transfered](){
            Marvin::ErrorType m_err = err;
            buffer->setSize(bytes_transfered);
            p_post_read_cb(cb, m_err, bytes_transfered);
        });
    });
    if (m_mode == NOTSECURE) {
        m_ssl_socket.next_layer().async_read_some(boost::asio::buffer(buffer->data(), buffer->capacity()), handler);
    } else {
        m_ssl_socket.async_read_some(boost::asio::buffer(buffer->data(), buffer->capacity()), handler);
    }
}
/**
 * write
 */
void Connection::asyncWrite(Marvin::MBuffer& buf, AsyncWriteCallbackType cb)
{
    p_async_write((void*)buf.data(), buf.size(), cb);
}
void Connection::asyncWrite(std::string& str, AsyncWriteCallback cb)
{
    p_async_write( (void*)str.c_str(), str.size(), cb);
}

void Connection::asyncWrite(Marvin::BufferChainSPtr buf_chain_sptr, AsyncWriteCallback cb)
{
    /// this took a while to work out - change buffer code at your peril
    LogDebug("");
    auto tmp = buf_chain_sptr->asio_buffer_sequence();
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        p_post_write_cb(cb, err, bytes_transfered);
    });
    if (m_mode == NOTSECURE) {
        boost::asio::async_write((this->m_ssl_socket.next_layer()), tmp, handler);
    } else {
        boost::asio::async_write((this->m_ssl_socket), tmp, handler);
    }
}
#if 1
void Connection::asyncWrite(boost::asio::const_buffer abuf, AsyncWriteCallback cb)
{
#if 0
    LogDebug("");
    boost::asio::async_write(
        (this->_boost_socket),
        abuf,
        [this, cb](
            const Marvin::ErrorType& err,
            std::size_t bytes_transfered
            )
        {
        LogDebug("");
        if( !err ){
            Marvin::ErrorType m_err = Marvin::make_error_ok();
            p_post_write_cb(cb, m_err, bytes_transfered);
//            cb(m_err, bytes_transfered);
        }else{
            Marvin::ErrorType m_err = err;
            p_post_write_cb(cb, m_err, bytes_transfered);
//            cb(m_err, bytes_transfered);
        }
    });
#endif
}
void Connection::asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback cb)
{
    LogDebug("");
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        p_post_write_cb(cb, err, bytes_transfered);
    });
    if (m_mode == NOTSECURE) {
        boost::asio::async_write((this->m_ssl_socket.next_layer()), sb, handler);
    } else {
        boost::asio::async_write((this->m_ssl_socket), sb, handler);
    }
}
#endif
#pragma mark - internal methods
void Connection::p_handle_resolve(
                    const error_code& err,
                    tcp::resolver::iterator endpoint_iterator)
{
    LogDebug("entry error: ", err.message());

    /// iterator empty ?
    tcp::resolver::iterator end;
    auto iter_empty = (endpoint_iterator == end);
    Marvin::ErrorType ec = err;
    LogDebug(std::string(__FUNCTION__) + " " + Marvin::make_error_description(ec) );
    if (iter_empty && (!err)) {
        LogDebug("empry but no error");
        // empty iter but no error (unlikely)
        boost::system::error_code local_err = boost::system::errc::make_error_code(boost::system::errc::host_unreachable);
        p_post_connect_cb(m_connect_cb, local_err, this);
    } else if (err) {
        // got an error
        p_post_connect_cb(m_connect_cb, err, this);
    } else {
        // all is good
        tcp::endpoint ep = *endpoint_iterator;
        LogDebug( ep.address().to_string());
        // always use the first iterator result
        LogDebug("resolve OK","so now connect");
        tcp::endpoint endpoint = *endpoint_iterator;
        
#if 1
        m_timeout.setTimeout(m_connect_timeout_interval_ms, [this](){
            m_lowest_layer_sock.cancel();
        });
        auto next_iter = ++endpoint_iterator;
        auto h = ([this, next_iter](const error_code& err) {
            m_timeout.cancelTimeout([this, err, next_iter](){
                p_handle_connect(err, next_iter);
            });
        });
        m_lowest_layer_sock.async_connect(endpoint, h);
#else
//        auto handler = m_strand.wrap(bind(&Connection::p_handle_connect, this, _1, ++endpoint_iterator));
        /// a bit clumsy - incrementing the iterator before passing it
        auto handler = m_strand.wrap(bind(&Connection::p_handle_connect, this, _1, ++endpoint_iterator));
        m_boost_socket.async_connect(endpoint, handler);
#endif
        LogDebug("leaving");
    }
}


void Connection::p_handle_connect(
                    const boost::system::error_code& err,
                    tcp::resolver::iterator endpoint_iterator)
{
    Marvin::ErrorType  ec = err;
    LogDebug("entry: ", Marvin::make_error_description(ec));
    if (!err)
    {
        LogFDTrace(nativeSocketFD());
        m_lowest_layer_sock.non_blocking(true);
        ///
        /// now must do handshake - not return
        /// TODO - if NOTSECURE
        if (m_mode == NOTSECURE) {
            p_post_connect_cb(m_connect_cb, err, this);
        } else {
            p_start_handshake();
        }
    }
    else if (endpoint_iterator != tcp::resolver::iterator())
    {
        LogDebug("try next iterator");
        m_lowest_layer_sock.close(); /// \todo why close ??
        tcp::endpoint endpoint = *endpoint_iterator;
        m_timeout.setTimeout(m_connect_timeout_interval_ms, [this](){
            m_lowest_layer_sock.cancel();
        });
        auto next_iter = ++endpoint_iterator;
        auto h = ([this, next_iter](const error_code& err) {
            m_timeout.cancelTimeout([this, err, next_iter](){
                p_handle_connect(err, next_iter);
            });
        });
        m_lowest_layer_sock.async_connect(endpoint, h);
    }
    else
    {
        LogError("resolve FAILED","Error: ",err.message());
        p_post_connect_cb(m_connect_cb, err, this);
    }
    LogDebug("leaving");
}
void Connection::p_start_handshake()
{
    m_ssl_socket.async_handshake(
        boost::asio::ssl::stream_base::client,
        boost::bind(&Connection::p_handle_handshake, this, boost::asio::placeholders::error)
    );
}
void Connection::p_handle_handshake(const boost::system::error_code& err)
{
    if(! err) {
        X509* server_cert = SSL_get_peer_certificate(this->m_ssl_socket.native_handle());
        m_server_certificate = Cert::Certificate(server_cert);
        p_post_connect_cb(m_connect_cb, err, this);
    } else {
        p_post_connect_cb(m_connect_cb, err, this);
    }
}
/**
* Common internal write method for all single buffer writes
*/
void Connection::p_async_write(void* data, std::size_t size, AsyncWriteCallback cb)
{
    LogDebug("");
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        LogDebug("");
        p_post_write_cb(cb, err, bytes_transfered);
    });
    if (m_mode == NOTSECURE) {
        boost::asio::async_write((this->m_ssl_socket.next_layer()), boost::asio::buffer(data, size), handler);
    } else {
        boost::asio::async_write((this->m_ssl_socket), boost::asio::buffer(data, size), handler);
    }
}
#pragma mark - post result of async ops through various callbacks
#define USE_POST 1

void Connection::completeWithError(Marvin::ErrorType& ec)
{
    #if USE_POST
    auto c = std::bind(m_connect_cb, ec, nullptr);
    m_io.post(c);
    #else
    m_connect_cb(ec, nullptr);
    #endif
}
void Connection::completeWithSuccess()
{
    Marvin::ErrorType err = Marvin::make_error_ok();
    #if USE_POST
    auto c = std::bind(m_connect_cb, err, this);
    m_io.post(c);
    #else
    m_connect_cb(err, this);
    #endif
}
void Connection::p_post_accept_cb(std::function<void(boost::system::error_code& err)> cb, Marvin::ErrorType err)
{
    #if USE_POST
    auto c = std::bind(cb, err);
    m_io.post(c);
    #else
    cb(err);
    #endif
}
void Connection::p_post_connect_cb(ConnectCallbackType  cb, Marvin::ErrorType err, ISocket* conn)
{
    ISocket* tmp = (err) ? nullptr : this;
    #if USE_POST
    auto c = std::bind(cb, err, tmp);
    m_io.post(c);
    #else
    m_connect_cb(err, tmp);
    #endif
}
void Connection::p_post_read_cb(AsyncReadCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered)
{
    #if USE_POST
    auto c = std::bind(cb, err, bytes_transfered);
    m_io.post(c);
    #else
    cb(err, bytes_transfered);
    #endif
}
void Connection::p_post_write_cb(AsyncWriteCallbackType cb, Marvin::ErrorType err, std::size_t  bytes_transfered)
{
    #if USE_POST
    auto c = std::bind(cb, err, bytes_transfered);
    m_io.post(c);
    #else
    cb(err, bytes_transfered);
    #endif
}

