//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <cassert>

#include <marvin/include/boost_stuff.hpp>

#include <marvin/error/marvin_error.hpp>
#include <marvin/include/callback_typedefs.hpp>
#include <marvin/external_src/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include <marvin/connection/tls_connection.hpp>

using boost::asio::ip::tcp;
using boost::bind;
using boost::function;
using boost::system::error_code;
using boost::asio::io_service;

#pragma mark - timeout interval config
long SSLConnection::s_connect_timeout_interval_ms = 500000;
long SSLConnection::s_read_timeout_interval_ms = 500000;
long SSLConnection::s_write_timeout_interval_ms = 500000;
void SSLConnection::setConfig_connectTimeOut(long millisecs)
{
    s_connect_timeout_interval_ms = millisecs;
}
void SSLConnection::setConfig_readTimeOut(long millisecs)
{
    s_read_timeout_interval_ms  = millisecs;
}
void SSLConnection::setConfig_writeTimeOut(long millisecs)
{
    s_write_timeout_interval_ms = millisecs;
}
#pragma mark - test and paranoia probes
std::map<int, int> SSLConnection::socket_fds_inuse;
void SSLConnection::fd_inuse(int fd)
{
    assert(socket_fds_inuse.find(fd) == socket_fds_inuse.end());
    socket_fds_inuse[fd] = fd;
}
void SSLConnection::fd_free(int fd)
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
SSLConnection::SSLConnection(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port,
            boost::asio::ssl::context ctx
            )
            :
            m_io(io_service),
            m_timeout(m_io),
            m_resolver(m_io),
            m_ssl_ctx(std::move(ctx)),
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
    LogDebug("constructor:: native handle :: ", x);
}


SSLConnection::SSLConnection(
    boost::asio::io_service& io_service,
    boost::asio::ssl::context ctx
    ):
        m_io(io_service),
        m_timeout(m_io),
        m_resolver(m_io), // dont really need this
        m_ssl_ctx(std::move(ctx)),
        m_ssl_socket(m_io, m_ssl_ctx),
        m_lowest_layer_sock(m_ssl_socket.lowest_layer()),
        m_connect_timeout_interval_ms(s_connect_timeout_interval_ms),
        m_read_timeout_interval_ms(s_read_timeout_interval_ms),
        m_write_timeout_interval_ms(s_write_timeout_interval_ms)

{
    LogTorTrace();
}
SSLConnection::~SSLConnection()
{
    LogTorTrace();
    if( ! m_closed_already) {
        LogFDTrace(nativeSocketFD());
        m_ssl_socket.lowest_layer().close();
    }
}
#pragma mark - public interface getters
std::string SSLConnection::scheme(){return m_scheme;}
std::string SSLConnection::server(){return m_server;}
std::string SSLConnection::service(){return m_port;}

long SSLConnection::nativeSocketFD()
{
    return m_lowest_layer_sock.native_handle();
}

#pragma mark - public interface connection management
void SSLConnection::close()
{
    LogFDTrace(nativeSocketFD());
    assert(! m_closed_already);
    m_closed_already = true;
    m_lowest_layer_sock.cancel();
    m_lowest_layer_sock.close();
}
void SSLConnection::shutdown()
{
    assert(! m_closed_already);
    m_lowest_layer_sock.shutdown(boost::asio::socket_base::shutdown_both);
}
void SSLConnection::setReadTimeout(long millisecs)
{
    m_read_timeout_interval_ms = millisecs;
}


#pragma mark - public interface async io operations
void SSLConnection::asyncAccept(
    boost::asio::ip::tcp::acceptor&                     acceptor,
    std::function<void(const boost::system::error_code& err)> cb
)
{
//    _boost_socket.non_blocking(true);
    acceptor.async_accept(m_lowest_layer_sock, [cb, this](const boost::system::error_code& err) {
        LogFDTrace(this->nativeSocketFD());
        p_post_accept_cb(cb, err);
    });
}

void SSLConnection::asyncConnect(ConnectCallbackType connect_cb)
{
    m_connect_cb = connect_cb; // save the connect callback

    tcp::resolver::query query(this->m_server, m_port);
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
    auto handler = (std::bind(&SSLConnection::p_handle_resolve,this, std::placeholders::_1, std::placeholders::_2));
    m_resolver.async_resolve(query, handler);
#endif
}

/**
 * read
 */
void SSLConnection::asyncRead(Marvin::MBufferSPtr buffer, AsyncReadCallbackType cb)
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
    m_ssl_socket.async_read_some(boost::asio::buffer(buffer->data(), buffer->capacity()), handler);
}
/**
 * write
 */
void SSLConnection::asyncWrite(Marvin::MBuffer& buf, AsyncWriteCallbackType cb)
{
    p_async_write((void*)buf.data(), buf.size(), cb);
}
void SSLConnection::asyncWrite(std::string& str, AsyncWriteCallback cb)
{
    p_async_write( (void*)str.c_str(), str.size(), cb);
}

void SSLConnection::asyncWrite(Marvin::BufferChainSPtr buf_chain_sptr, AsyncWriteCallback cb)
{
    /// this took a while to work out - change buffer code at your peril
    LogDebug("");
    auto tmp = buf_chain_sptr->asio_buffer_sequence();
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        p_post_write_cb(cb, err, bytes_transfered);
    });
    boost::asio::async_write((this->m_ssl_socket), tmp, handler);
}
#if 1
void SSLConnection::asyncWrite(boost::asio::const_buffer abuf, AsyncWriteCallback cb)
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
void SSLConnection::asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback cb)
{
    LogDebug("");
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        p_post_write_cb(cb, err, bytes_transfered);
    });
    boost::asio::async_write((this->m_ssl_socket), sb, handler);
}
#endif
#pragma mark - internal methods
void SSLConnection::p_handle_resolve(
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
//        auto handler = m_strand.wrap(bind(&SSLConnection::p_handle_connect, this, _1, ++endpoint_iterator));
        /// a bit clumsy - incrementing the iterator before passing it
        auto handler = m_strand.wrap(bind(&SSLConnection::p_handle_connect, this, _1, ++endpoint_iterator));
        m_boost_socket.async_connect(endpoint, handler);
#endif
        LogDebug("leaving");
    }
}


void SSLConnection::p_handle_connect(
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
        ///
        p_start_handshake();
//        p_post_connect_cb(m_connect_cb, err, this);
    }
    else if (endpoint_iterator != tcp::resolver::iterator())
    {
        LogDebug("try next iterator");
        m_lowest_layer_sock.close(); /// \todo why close ??
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
        auto handler = m_strand.wrap(boost::bind(&SSLConnection::p_handle_connect, this, _1, ++endpoint_iterator));
        m_boost_socket.async_connect(endpoint, handler);
#endif
    }
    else
    {
        LogError("resolve FAILED","Error: ",err.message());
        p_post_connect_cb(m_connect_cb, err, this);
    }
    LogDebug("leaving");
}
void SSLConnection::p_start_handshake()
{
    m_ssl_socket.async_handshake(
        boost::asio::ssl::stream_base::client,
        boost::bind(&SSLConnection::p_handle_handshake, this, boost::asio::placeholders::error)
    );
}
void SSLConnection::p_handle_handshake(const boost::system::error_code& err)
{
    if(! err) {
        p_post_connect_cb(m_connect_cb, err, this);
    } else {
        p_post_connect_cb(m_connect_cb, err, this);
    }
}
/**
* Common internal write method for all single buffer writes
*/
void SSLConnection::p_async_write(void* data, std::size_t size, AsyncWriteCallback cb)
{
    LogDebug("");
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        LogDebug("");
        p_post_write_cb(cb, err, bytes_transfered);
    });

    boost::asio::async_write((this->m_ssl_socket), boost::asio::buffer(data, size), handler);
}
#pragma mark - post result of async ops through various callbacks
#define USE_POST 1

void SSLConnection::completeWithError(Marvin::ErrorType& ec)
{
    #if USE_POST
    auto c = std::bind(m_connect_cb, ec, nullptr);
    m_io.post(c);
    #else
    m_connect_cb(ec, nullptr);
    #endif
}
void SSLConnection::completeWithSuccess()
{
    Marvin::ErrorType err = Marvin::make_error_ok();
    #if USE_POST
    auto c = std::bind(m_connect_cb, err, this);
    m_io.post(c);
    #else
    m_connect_cb(err, this);
    #endif
}
void SSLConnection::p_post_accept_cb(std::function<void(boost::system::error_code& err)> cb, Marvin::ErrorType err)
{
    #if USE_POST
    auto c = std::bind(cb, err);
    m_io.post(c);
    #else
    cb(err);
    #endif
}

void SSLConnection::p_post_connect_cb(ConnectCallbackType  cb, Marvin::ErrorType err, ISocket* conn)
{
    ISocket* tmp = (err) ? nullptr : this;
    #if USE_POST
    auto c = std::bind(cb, err, tmp);
    m_io.post(c);
    #else
    m_connect_cb(err, tmp);
    #endif
}
void SSLConnection::p_post_read_cb(AsyncReadCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered)
{
    #if USE_POST
    auto c = std::bind(cb, err, bytes_transfered);
    m_io.post(c);
    #else
    cb(err, bytes_transfered);
    #endif
}
void SSLConnection::p_post_write_cb(AsyncWriteCallbackType cb, Marvin::ErrorType err, std::size_t  bytes_transfered)
{
    #if USE_POST
    auto c = std::bind(cb, err, bytes_transfered);
    m_io.post(c);
    #else
    cb(err, bytes_transfered);
    #endif
}

