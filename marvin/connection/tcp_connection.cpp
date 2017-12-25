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

#include "boost_stuff.hpp"

#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "i_socket.hpp"
#include "tcp_connection.hpp"

using boost::asio::ip::tcp;
using boost::bind;
using boost::function;
using boost::system::error_code;
using boost::asio::io_service;

#pragma mark - timeout interval config
long TCPConnection::s_connect_timeout_interval_ms = 5000;
long TCPConnection::s_read_timeout_interval_ms = 5000;
long TCPConnection::s_write_timeout_interval_ms = 5000;
void TCPConnection::setConfig_connectTimeOut(long millisecs)
{
    s_connect_timeout_interval_ms = millisecs;
}
void TCPConnection::setConfig_readTimeOut(long millisecs)
{
    s_read_timeout_interval_ms  = millisecs;
}
void TCPConnection::setConfig_writeTimeOut(long millisecs)
{
    s_write_timeout_interval_ms = millisecs;
}
#pragma mark - test and paranoia probes
std::map<int, int> TCPConnection::socket_fds_inuse;
void TCPConnection::fd_inuse(int fd)
{
    assert(socket_fds_inuse.find(fd) == socket_fds_inuse.end());
    socket_fds_inuse[fd] = fd;
}
void TCPConnection::fd_free(int fd)
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
TCPConnection::TCPConnection(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
            )
            :
            m_io(io_service),
            m_strand(m_io),
            m_resolver(m_io),
            m_boost_socket(m_io),
            m_scheme(scheme),
            m_server(server),
            m_port(port),
            m_connect_cb(nullptr),
            m_closed_already(false),
            m_timer(m_io),
            m_connect_timeout_interval_ms(s_connect_timeout_interval_ms),
            m_read_timeout_interval_ms(s_read_timeout_interval_ms),
            m_write_timeout_interval_ms(s_write_timeout_interval_ms)
{
    LogTorTrace();
    auto x = nativeSocketFD();
    LogDebug("constructor:: native handle :: ", x);
}


TCPConnection::TCPConnection(
    boost::asio::io_service& io_service
    ):
        m_io(io_service),
        m_strand(m_io),
        m_resolver(m_io), // dont really need this
        m_boost_socket(m_io),
        m_timer(m_io),
        m_connect_timeout_interval_ms(s_connect_timeout_interval_ms),
        m_read_timeout_interval_ms(s_read_timeout_interval_ms),
        m_write_timeout_interval_ms(s_write_timeout_interval_ms)

{
    LogTorTrace();
}
TCPConnection::~TCPConnection()
{
    LogTorTrace();
    if( ! m_closed_already) {
        LogFDTrace(nativeSocketFD());
        m_boost_socket.close();
    }
}
#pragma mark - public interface getters
std::string TCPConnection::scheme(){return m_scheme;}
std::string TCPConnection::server(){return m_server;}
std::string TCPConnection::service(){return m_port;}

long TCPConnection::nativeSocketFD()
{
    return m_boost_socket.native_handle();
}

#pragma mark - public interface connection management
void TCPConnection::close()
{
    LogFDTrace(nativeSocketFD());
    assert(! m_closed_already);
    m_closed_already = true;
    m_boost_socket.cancel();
    m_boost_socket.close();
}
void TCPConnection::shutdown()
{
    assert(! m_closed_already);
    m_boost_socket.shutdown(boost::asio::socket_base::shutdown_both);
}
#pragma mark - public interface async io operations
void TCPConnection::asyncAccept(
    boost::asio::ip::tcp::acceptor&                     acceptor,
    std::function<void(const boost::system::error_code& err)> cb
)
{
//    _boost_socket.non_blocking(true);
    acceptor.async_accept(m_boost_socket, [cb, this](const boost::system::error_code& err) {
        LogFDTrace(this->nativeSocketFD());
        post_accept_cb(cb, err);
    });
}

void TCPConnection::asyncConnect(ConnectCallbackType connect_cb)
{
    m_connect_cb = connect_cb; // save the connect callback

    tcp::resolver::query query(this->m_server, m_port);
    
    auto handler = m_strand.wrap(std::bind(&TCPConnection::handle_resolve,this, std::placeholders::_1, std::placeholders::_2));
    
    m_resolver.async_resolve(query, handler);
}

/**
 * read
 */
void TCPConnection::asyncRead(MBuffer& buffer, AsyncReadCallbackType cb)
{
    auto handler = m_strand.wrap([this, cb, &buffer](const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        Marvin::ErrorType m_err = err;
        buffer.setSize(bytes_transfered);
        post_read_cb(cb, m_err, bytes_transfered);
    });
    m_boost_socket.async_read_some(boost::asio::buffer(buffer.data(), buffer.capacity()), handler);
}
/**
 * write
 */
void TCPConnection::asyncWrite(MBuffer& buf, AsyncWriteCallbackType cb)
{
    async_write((void*)buf.data(), buf.size(), cb);
}
void TCPConnection::asyncWrite(std::string& str, AsyncWriteCallback cb)
{
    async_write( (void*)str.c_str(), str.size(), cb);
}

void TCPConnection::asyncWrite(BufferChainSPtr buf_chain_sptr, AsyncWriteCallback cb)
{
    /// this took a while to work out - change buffer code at your peril
    LogDebug("");
    auto tmp = buf_chain_sptr->asio_buffer_sequence();
    auto handler = m_strand.wrap([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        post_write_cb(cb, err, bytes_transfered);
    });
    boost::asio::async_write((this->m_boost_socket), tmp, handler);
}
#if 1
void TCPConnection::asyncWrite(boost::asio::const_buffer abuf, AsyncWriteCallback cb)
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
            post_write_cb(cb, m_err, bytes_transfered);
//            cb(m_err, bytes_transfered);
        }else{
            Marvin::ErrorType m_err = err;
            post_write_cb(cb, m_err, bytes_transfered);
//            cb(m_err, bytes_transfered);
        }
    });
#endif
}
void TCPConnection::asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback cb)
{
    LogDebug("");
    auto handler = m_strand.wrap([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        post_write_cb(cb, err, bytes_transfered);
    });
    boost::asio::async_write((this->m_boost_socket), sb, handler);
}
#endif
#pragma mark - internal methods
void TCPConnection::handle_resolve(
                    const error_code& err,
                    tcp::resolver::iterator endpoint_iterator)
{
    LogDebug("entry error: ", err.message());

    /// iterator empty ?
    tcp::resolver::iterator end;
    auto iter_empty = (endpoint_iterator == end);
    std::cout << err.message() << std::endl;
    if (iter_empty && (!err)) {
        LogDebug("empry but no error");
        // empty iter but no error (unlikely)
        boost::system::error_code local_err = boost::system::errc::make_error_code(boost::system::errc::host_unreachable);
        post_connect_cb(m_connect_cb, local_err, this);
    } else if (err) {
        // got an error
        post_connect_cb(m_connect_cb, err, this);
    } else {
        // all is good
        tcp::endpoint ep = *endpoint_iterator;
        std::cout << ep.address().to_string() << std::endl;
        // always use the first iterator result
        LogDebug("resolve OK","so now connect");
        tcp::endpoint endpoint = *endpoint_iterator;
        
        auto handler = m_strand.wrap(bind(&TCPConnection::handle_connect, this, _1, ++endpoint_iterator));
        
        m_boost_socket.async_connect(endpoint, handler);
        LogDebug("leaving");
    }
}


void TCPConnection::handle_connect(
                    const boost::system::error_code& err,
                    tcp::resolver::iterator endpoint_iterator)
{
    LogDebug("entry");
    if (!err)
    {
        LogFDTrace(nativeSocketFD());
        m_boost_socket.non_blocking(true);
        post_connect_cb(m_connect_cb, err, this);
    }
    else if (endpoint_iterator != tcp::resolver::iterator())
    {
        LogDebug("try next iterator");
        m_boost_socket.close();
        tcp::endpoint endpoint = *endpoint_iterator;
        auto handler = m_strand.wrap(boost::bind(&TCPConnection::handle_connect, this, _1, ++endpoint_iterator));
        m_boost_socket.async_connect(endpoint, handler);
    }
    else
    {
        LogError("resolve FAILED","Error: ",err.message());
        post_connect_cb(m_connect_cb, err, this);
    }
    LogDebug("leaving");
}
/**
* Common internal write method for all single buffer writes
*/
void TCPConnection::async_write(void* data, std::size_t size, AsyncWriteCallback cb)
{
    LogDebug("");
    auto handler = m_strand.wrap([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        LogDebug("");
        post_write_cb(cb, err, bytes_transfered);
    });

    boost::asio::async_write((this->m_boost_socket), boost::asio::buffer(data, size), handler);
}
#pragma mark - timeout handler

void TCPConnection::handle_timeout(const boost::system::error_code& err)
{
    if( err == boost::asio::error::operation_aborted) {
        // timeout was cancelled - presumably by a successful i/o operation
    } else if ( err) {
        // some other error - close down ?
        m_boost_socket.cancel();
    } else {
        // no error - timeout
        m_boost_socket.cancel();
    }
}
void TCPConnection::cancel_timeout()
{
    m_timer.cancel();
    m_timer.expires_from_now(boost::posix_time::pos_infin);
}
void TCPConnection::set_timeout(long interval_millisecs)
{
    auto handler = m_strand.wrap(std::bind(&TCPConnection::handle_timeout, this, std::placeholders::_1));
    m_timer.expires_from_now(boost::posix_time::milliseconds(interval_millisecs));
    m_timer.async_wait(handler);
}

#pragma mark - post result of async ops through various callbacks
#define USE_POST 1

void TCPConnection::completeWithError(Marvin::ErrorType& ec)
{
    #if USE_POST
    auto c = std::bind(m_connect_cb, ec, nullptr);
    m_io.post(c);
    #else
    m_connect_cb(ec, nullptr);
    #endif
}
void TCPConnection::completeWithSuccess()
{
    Marvin::ErrorType err = Marvin::make_error_ok();
    #if USE_POST
    auto c = std::bind(m_connect_cb, err, this);
    m_io.post(c);
    #else
    m_connect_cb(err, this);
    #endif
}
void TCPConnection::post_accept_cb(std::function<void(boost::system::error_code& err)> cb, Marvin::ErrorType err)
{
    #if USE_POST
    auto c = std::bind(cb, err);
    m_io.post(c);
    #else
    cb(err);
    #endif
}

void TCPConnection::post_connect_cb(ConnectCallbackType  cb, Marvin::ErrorType err, ISocket* conn)
{
    ISocket* tmp = (err) ? nullptr : this;
    #if USE_POST
    auto c = std::bind(cb, err, tmp);
    m_io.post(c);
    #else
    m_connect_cb(err, tmp);
    #endif
}
void TCPConnection::post_read_cb(AsyncReadCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered)
{
    #if USE_POST
    auto c = std::bind(cb, err, bytes_transfered);
    m_io.post(c);
    #else
    cb(err, bytes_transfered);
    #endif
}
void TCPConnection::post_write_cb(AsyncWriteCallbackType cb, Marvin::ErrorType err, std::size_t  bytes_transfered)
{
    #if USE_POST
    auto c = std::bind(cb, err, bytes_transfered);
    m_io.post(c);
    #else
    cb(err, bytes_transfered);
    #endif
}

