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
/**
* What about multiple access
*/
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
/**
 * Constructor
 *  @param {io_service} io_service  -   to use for running
 *  @param {string}     scheme      -   "http" or "https"
 *  @param {string}     server      -   domain string
 */
#if 0
TCPConnection::TCPConnection(
            boost::asio::io_service& io_service,
            const std::string& scheme,
            const std::string& server,
            const std::string& port
            )
            :
            _io(io_service),
            _strand(_io),
            _resolver(io_service),
            _boost_socket(io_service),
            _scheme(scheme),
            _server(server),
            _port(port)
{
    LogTorTrace();
}
#endif
TCPConnection::TCPConnection(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
            )
            :
            _io(io_service),
            _strand(_io),
            _resolver(io_service),
            _boost_socket(io_service),
            _scheme(scheme),
            _server(server),
            _port(port),
            _closed_already(false),
            _timer(_io)
{
    LogTorTrace();
    auto x = nativeSocketFD();
    LogDebug("constructor:: native handle :: ", x);
}


TCPConnection::TCPConnection(
    boost::asio::io_service& io_service
    ):
        _io(io_service),
        _strand(_io),
        _resolver(io_service), // dont really need this
        _boost_socket(io_service),
        _timer(_io)

{
    LogTorTrace();
}
TCPConnection::~TCPConnection()
{
    LogTorTrace();
    if( ! _closed_already) {
        LogFDTrace(nativeSocketFD());
        _boost_socket.close();
    }
}
std::string TCPConnection::scheme(){return _scheme;}
std::string TCPConnection::server(){return _server;}
std::string TCPConnection::service(){return _port;}

void TCPConnection::close()
{
    LogFDTrace(nativeSocketFD());
    assert(! _closed_already);
    _closed_already = true;
    _boost_socket.cancel();
    _boost_socket.close();
}
void TCPConnection::shutdown()
{
    assert(! _closed_already);
    _boost_socket.shutdown(boost::asio::socket_base::shutdown_both);
}

long TCPConnection::nativeSocketFD()
{
    return _boost_socket.native_handle();
}
void TCPConnection::asyncAccept(
    boost::asio::ip::tcp::acceptor&                     acceptor,
    std::function<void(const boost::system::error_code& err)> cb
)
{
//    _boost_socket.non_blocking(true);
    acceptor.async_accept(_boost_socket, [cb, this](const boost::system::error_code& err) {
        LogFDTrace(this->nativeSocketFD());
        post_accept_cb(cb, err);
    });
}

void TCPConnection::asyncConnect(ConnectCallbackType final_cb)
{
    _finalCb = final_cb; // save the final callback

    tcp::resolver::query query(this->_server, _port);
    
    auto handler = _strand.wrap(std::bind(&TCPConnection::handle_resolve,this, std::placeholders::_1, std::placeholders::_2));
    
    _resolver.async_resolve(query, handler);
}

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
        post_connect_cb(_finalCb, err, this);
    } else if (err) {
        // got an error
        post_connect_cb(_finalCb, err, this);
    } else {
        // all is good
        tcp::endpoint ep = *endpoint_iterator;
        std::cout << ep.address().to_string() << std::endl;
        // always use the first iterator result
        LogDebug("resolve OK","so now connect");
        tcp::endpoint endpoint = *endpoint_iterator;
        
        auto handler = _strand.wrap(bind(&TCPConnection::handle_connect, this, _1, ++endpoint_iterator));
        
        _boost_socket.async_connect(endpoint, handler);
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
        _boost_socket.non_blocking(true);
        post_connect_cb(_finalCb, err, this);
//        completeWithSuccess();
    }
    else if (endpoint_iterator != tcp::resolver::iterator())
    {
        LogDebug("try next iterator");
        _boost_socket.close();
        tcp::endpoint endpoint = *endpoint_iterator;
        auto handler = _strand.wrap(boost::bind(&TCPConnection::handle_connect, this, _1, ++endpoint_iterator));
        _boost_socket.async_connect(endpoint, handler);
    }
    else
    {
        LogError("resolve FAILED","Error: ",err.message());
        Marvin::ErrorType me = err;
        post_connect_cb(_finalCb, err, this);
//        completeWithError(me);
    }
    LogDebug("leaving");
}
/**
 * read
 */
void TCPConnection::asyncRead(MBuffer& buffer, AsyncReadCallbackType cb)
{
    auto handler = _strand.wrap([this, cb, &buffer](const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        Marvin::ErrorType m_err = err;
        buffer.setSize(bytes_transfered);
        post_read_cb(cb, m_err, bytes_transfered);
    });
    _boost_socket.async_read_some(boost::asio::buffer(buffer.data(), buffer.capacity()), handler);
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
    auto handler = _strand.wrap([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        post_write_cb(cb, err, bytes_transfered);
    });
    boost::asio::async_write((this->_boost_socket), tmp, handler);
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
    auto handler = _strand.wrap([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        post_write_cb(cb, err, bytes_transfered);
    });
    boost::asio::async_write((this->_boost_socket), sb, handler);
}
#endif
/**
* Common internal write method for all single buffer writes
*/
void TCPConnection::async_write(void* data, std::size_t size, AsyncWriteCallback cb)
{
    LogDebug("");
    auto handler = _strand.wrap([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        LogDebug("");
        post_write_cb(cb, err, bytes_transfered);
    });

    boost::asio::async_write((this->_boost_socket), boost::asio::buffer(data, size), handler);
}
#pragma mark - timeout handler

void TCPConnection::handle_timeout(const boost::system::error_code& err)
{
    if( err == boost::asio::error::operation_aborted) {
        // timeout was cancelled - presumably by a successful i/o operation
    } else if ( err) {
        // some other error - close down ?
        _boost_socket.cancel();
    } else {
        // no error - timeout
        _boost_socket.cancel();
    }
}
void TCPConnection::cancel_timeout()
{
    _timer.cancel();
    _timer.expires_from_now(boost::posix_time::pos_infin);
}
void TCPConnection::set_timeout(long interval_millisecs)
{
    auto handler = _strand.wrap(std::bind(&TCPConnection::handle_timeout, this, std::placeholders::_1));
    _timer.expires_from_now(boost::posix_time::milliseconds(interval_millisecs));
    _timer.async_wait(handler);
}

#pragma mark - post result of async ops through various callbacks
#define USE_POST 1

void TCPConnection::completeWithError(Marvin::ErrorType& ec)
{
    #if USE_POST
    auto c = std::bind(_finalCb, ec, nullptr);
    _io.post(c);
    #else
    _finalCb(ec, nullptr);
    #endif
}
void TCPConnection::completeWithSuccess()
{
    Marvin::ErrorType err = Marvin::make_error_ok();
    #if USE_POST
    auto c = std::bind(_finalCb, err, this);
    _io.post(c);
    #else
    _finalCb(err, this);
    #endif
}
void TCPConnection::post_accept_cb(std::function<void(boost::system::error_code& err)> cb, Marvin::ErrorType err)
{
    #if USE_POST
    auto c = std::bind(cb, err);
    _io.post(c);
    #else
    cb(err);
    #endif
}

void TCPConnection::post_connect_cb(ConnectCallbackType  cb, Marvin::ErrorType err, ISocket* conn)
{
    ISocket* tmp = (err) ? nullptr : this;
    #if USE_POST
    auto c = std::bind(cb, err, tmp);
    _io.post(c);
    #else
    _finalCb(err, tmp);
    #endif
}
void TCPConnection::post_read_cb(AsyncReadCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered)
{
    #if USE_POST
    auto c = std::bind(cb, err, bytes_transfered);
    _io.post(c);
    #else
    cb(err, bytes_transfered);
    #endif
}
void TCPConnection::post_write_cb(AsyncWriteCallbackType cb, Marvin::ErrorType err, std::size_t  bytes_transfered)
{
    #if USE_POST
    auto c = std::bind(cb, err, bytes_transfered);
    _io.post(c);
    #else
    cb(err, bytes_transfered);
    #endif
}

