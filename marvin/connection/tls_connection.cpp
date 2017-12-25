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

//#include <boost/asio.hpp>
//#include <boost/asio/ssl.hpp>
//#include <boost/bind.hpp>
//#include <boost/function.hpp>

#include "boost_stuff.hpp"

#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)
#include "i_socket.hpp"
#include "tls_connection.hpp"
#include <cassert>

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::asio::ssl;

/**
 * Constructor
 *  @param {io_service} io_service  -   to use for running
 *  @param {string}     scheme      -   "http" or "https"
 *  @param {string}     server      -   domain string
 */
TLSConnection::TLSConnection(
            boost::asio::io_service& io_service,
            const std::string& scheme,
            const std::string& server,
            const std::string& port
            )
            :
            _io(io_service),
            _resolver(io_service),
            _scheme(scheme),
            _server(server),
            _port(port)
{
    LogTorTrace();
//        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);

    _sslContextUPtr = std::unique_ptr<ssl::context>(new ssl::context(boost::asio::ssl::context::tlsv12));
    _sslContextUPtr->set_default_verify_paths();
    _sslContextUPtr->set_options(
        asio::ssl::context::default_workarounds
        | asio::ssl::context::no_sslv2
        | asio::ssl::context::single_dh_use
    );
/* needed for server only
    _sslContextUPtr->use_certificate_chain_file("server.pem");
    _sslContextUPtr->use_private_key_file("server.pem", asio::ssl::context::pem);
    _sslContextUPtr->use_tmp_dh_file("dh2048.pem");
*/
    
    typedef ssl::stream<tcp::socket> SslSocket;
    
    _boostSslSocketUPtr = std::unique_ptr<SslSocket>(new  SslSocket(_io, *_sslContextUPtr));
    /*
    * client only
    */
    _boostSslSocketUPtr->set_verify_mode(boost::asio::ssl::verify_peer | ssl::verify_fail_if_no_peer_cert);
    _boostSslSocketUPtr->set_verify_callback(boost::bind(&TLSConnection::verifyCertificate, this, _1, _2));

//ssl::rfc2818_verification("host.name")
//context_.set_options(
//        asio::ssl::context::default_workarounds
//        | asio::ssl::context::no_sslv2
//        | asio::ssl::context::single_dh_use);
}
//----------------------------------------------------------------------------
TLSConnection::TLSConnection(
    boost::asio::io_service& io_service
    ):   _io(io_service),
         _resolver(io_service)

{
    LogTorTrace();
}
//----------------------------------------------------------------------------
TLSConnection::~TLSConnection()
{
    LogTorTrace();
}

std::string TLSConnection::scheme(){return _scheme;}
std::string TLSConnection::server(){return _server;}
std::string TLSConnection::service(){return _port;}

//----------------------------------------------------------------------------
bool TLSConnection::verifyCertificate(bool preverified, boost::asio::ssl::verify_context& ctx)
{
    LogDebug("");
    return preverified;
}
//----------------------------------------------------------------------------
void TLSConnection::close()
{
    LogDebug(" fd: ", nativeSocketFD());
    _boostSocketUPtr->cancel();
    _boostSocketUPtr->close();
}
void TLSConnection::shutdown()
{
}
//----------------------------------------------------------------------------
long TLSConnection::nativeSocketFD()
{
//    auto x1 = _boostSslSocketUPtr.get();
//    auto x3 = _boostSslSocketUPtr->native_handle();
//    return 4*4*4;
    return (long)_boostSslSocketUPtr->native_handle();
}
//----------------------------------------------------------------------------
void TLSConnection::asyncAccept(
    boost::asio::ip::tcp::acceptor&                     acceptor,
    std::function<void(const boost::system::error_code& err)> cb
)
{
    LogDebug("");
    acceptor.async_accept(_boostSslSocketUPtr->lowest_layer(), [this, cb](const boost::system::error_code& err){
        // after accept
        LogDebug("");
        if( ! err ){
            _boostSslSocketUPtr->async_handshake(boost::asio::ssl::stream_base::server,
                                                [this,cb](const boost::system::error_code& err){
                // after handshake
                if(! err ){
                    cb(err);
                }else{
                    LogDebug(" handshake error ", err.value(), err.category().name(), err.category().message(err.value()));
                    cb(err);
                }
            });
        }else{
            LogDebug(" accept error ", err.value(), err.category().name(), err.category().message(err.value()));
            cb(err);
        }
    });
}

//----------------------------------------------------------------------------
// This is the first of the Connect sequence of calbacks
//
// Next :: handleConnect
//----------------------------------------------------------------------------
void TLSConnection::asyncConnect(ConnectCallbackType final_cb)
{
    tcp::resolver::query query(this->_server, _port);
    _finalCb = final_cb; // save the final callback
    
    _resolver.async_resolve(query, [this](const boost::system::error_code& ec,
                                          tcp::resolver::iterator endpoint_iterator){
        LogDebug("resolve OK","so now connect");
        if( ec ){
            Marvin::ErrorType me = ec;
            LogError("error_value", ec.value(), " message: ", ec.message());
            completeWithError(me);
        } else {
            tcp::endpoint endpoint = *endpoint_iterator;
            auto connect_cb = bind(&TLSConnection::handleConnect, this, _1, ++endpoint_iterator);
            _boostSslSocketUPtr->lowest_layer().async_connect(endpoint, connect_cb);
            LogDebug("leaving");
        }
    });
}

//----------------------------------------------------------------------------
// I think this is obsolete
//----------------------------------------------------------------------------
void TLSConnection::handleResolve(
                    const error_code& err,
                    tcp::resolver::iterator endpoint_iterator)
{
    LogDebug("entry");
    if (err)
    {
        LogError("resolve FAILED","Error: ",err.message());
        Marvin::ErrorType me = err;
        completeWithError(me);
        return;
    }else{
        LogDebug("resolve OK","so now connect");
        tcp::endpoint endpoint = *endpoint_iterator;
        auto connect_cb = bind(&TLSConnection::handleConnect, this, _1, ++endpoint_iterator);
        _boostSslSocketUPtr->lowest_layer().async_connect(endpoint, connect_cb);
        LogDebug("leaving");
    }
}

//----------------------------------------------------------------------------
// Next :: handleConnect            - to try next endpoint
//      :: handleConnectHandshake   - if resolved done, start handshake
//----------------------------------------------------------------------------
void TLSConnection::handleConnect(
                    const boost::system::error_code& err,
                    tcp::resolver::iterator endpoint_iterator)
{
    LogDebug("entry");
    if (!err)
    {
        LogDebug("connect OK");
        _boostSslSocketUPtr->lowest_layer().non_blocking(true);
//        completeWithSuccess();
        _boostSslSocketUPtr->async_handshake(
            boost::asio::ssl::stream_base::client,
            boost::bind(&TLSConnection::handleConnectHandshake, this, boost::asio::placeholders::error)
        );
    }
    else if (endpoint_iterator != tcp::resolver::iterator())
    {
        LogDebug("try next iterator");
        _boostSslSocketUPtr->lowest_layer().close();
        tcp::endpoint endpoint = *endpoint_iterator;
        auto handler = boost::bind(&TLSConnection::handleConnect, this, _1, ++endpoint_iterator);
        _boostSslSocketUPtr->lowest_layer().async_connect(endpoint, handler);
    }
    else
    {
        LogError("resolve FAILED","Error: ",err.message());
        Marvin::ErrorType me = err;
        completeWithError(me);
    }
    LogDebug("leaving");
}
//----------------------------------------------------------------------------
// next :: completeWithSuccess or completeWithError
//----------------------------------------------------------------------------
void TLSConnection::handleConnectHandshake(const boost::system::error_code& error)
{
    if (!error) {
        completeWithSuccess();
    }else{
        LogError("resolve FAILED","Error: ",error.message());
        Marvin::ErrorType me = error;
        completeWithError(me);
    }
}


//----------------------------------------------------------------------------
void TLSConnection::completeWithError(Marvin::ErrorType& ec)
{
    _finalCb(ec, nullptr);
}
//----------------------------------------------------------------------------
void TLSConnection::completeWithSuccess()
{
    Marvin::ErrorType err = Marvin::make_error_ok();
    _finalCb(err, this);
}
//----------------------------------------------------------------------------
/**
 * read
 */
//----------------------------------------------------------------------------
void TLSConnection::asyncRead(MBuffer& buffer, AsyncReadCallbackType cb)
{
    // if a read is active - throw an exception
    //    async_read(this->socket_, buf, )
    //
    // start a boost async_read, on callback pass the data to the http parser
    //
    auto b = boost::asio::buffer(buffer.data(), buffer.capacity());
//    auto opn = _boostSslSocketUPtr->lowest_layer().is_open();
    _boostSslSocketUPtr->async_read_some(b, [this, cb, &buffer](const Marvin::ErrorType& err, std::size_t bytes_transfered){
        Marvin::ErrorType m_err = err;
        buffer.setSize(bytes_transfered);
        cb(m_err, bytes_transfered);
    });
    
}
/**
 * write
 */
//----------------------------------------------------------------------------
void TLSConnection::asyncWrite(MBuffer& buffer, AsyncWriteCallbackType cb)
{
}
//----------------------------------------------------------------------------
void TLSConnection::asyncWrite(FBuffer& buffer, AsyncWriteCallbackType cb)
{
    LogDebug("buffer size: ");
//    auto wcb2 = boost::bind(&TLSConnection::handle_write_request,this,_1, _2);
    /// use the boost function that ONLY returns when the write is DONE
    assert(false);
//    boost::asio::async_write(_boost_socket, boost::asio::buffer(x, 0), cb);
}
//----------------------------------------------------------------------------
void TLSConnection::asyncWriteStreamBuf(boost::asio::streambuf& sb, AsyncWriteCallback cb)
{
    LogDebug("");
    boost::asio::async_write(
        (*_boostSslSocketUPtr),
        sb,
        [this, cb](
            const Marvin::ErrorType& err,
            std::size_t bytes_transfered
            )
        {
        LogDebug("");
        if( !err ){
            Marvin::ErrorType m_err = Marvin::make_error_ok();
            cb(m_err, bytes_transfered);
        }else{
            Marvin::ErrorType m_err = err;
            cb(m_err, bytes_transfered);
        }
    });
}




