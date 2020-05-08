#include <ostream>
#include <string>
#include <cassert>

#include <marvin/boost_stuff.hpp>
#include <marvin/error/marvin_error.hpp>
#include <marvin/callback_typedefs.hpp>
#include <marvin/error_handler/error_handler.hpp>
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn|Trog::LogLevelTrace3|Trog::LogLevelCTorTrace|Trog::LogLevelFDTrace)

#include <marvin/connection/connection.hpp>
namespace Marvin {
using ::boost::asio::ip::tcp;
using ::boost::bind;
using ::boost::function;
using ::boost::system::error_code;
using ::boost::asio::io_service;

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
            m_tcp_socket(m_io),
            m_scheme(scheme),
            m_server(server),
            m_port(port),
            m_connect_cb(nullptr),
            m_closed_already(false),
            m_connect_timeout_interval_ms(s_connect_timeout_interval_ms),
            m_read_timeout_interval_ms(s_read_timeout_interval_ms),
            m_write_timeout_interval_ms(s_write_timeout_interval_ms)
{
   TROG_TRACE_CTOR();
   TROG_TRACE_FD(nativeSocketFD());
   auto x = nativeSocketFD();
   TROG_DEBUG("constructor:: native handle :: ", x);
}


Connection::Connection(
    boost::asio::io_service& io_service
    ):
        m_mode(NOTSECURE),
        m_io(io_service),
        m_timeout(m_io),
        m_resolver(m_io), // dont really need this
//            m_ssl_ctx(std::move(ctx)),
        m_tcp_socket(m_io),
        m_connect_timeout_interval_ms(s_connect_timeout_interval_ms),
        m_read_timeout_interval_ms(s_read_timeout_interval_ms),
        m_write_timeout_interval_ms(s_write_timeout_interval_ms)

{
   TROG_TRACE_CTOR();
   TROG_TRACE_FD(nativeSocketFD());
}
Connection::~Connection()
{
    std::cout << __PRETTY_FUNCTION__ << "FD: " << nativeSocketFD() << " already_closed: " << (int)m_closed_already << std::endl;
    TROG_TRACE_CTOR();
    if( ! m_closed_already) {
        TROG_TRACE3("close fd: ", nativeSocketFD());
        TROG_TRACE_FD(nativeSocketFD());
        m_tcp_socket.close();
    } else {
        TROG_TRACE_FD(nativeSocketFD());
    }
}
std::string Connection::scheme(){return m_scheme;}
std::string Connection::server(){return m_server;}
std::string Connection::service(){return m_port;}

long Connection::nativeSocketFD()
{
    return m_tcp_socket.native_handle();
}
boost::asio::io_service& Connection::getIO()
{
    return m_io;
}
boost::asio::ssl::context& Connection::getSslContext()
{
    return *m_ssl_ctx_sptr;
};

void Connection::close()
{
   TROG_TRACE_FD(nativeSocketFD());
    assert(! m_closed_already);
    m_closed_already = true;
    m_tcp_socket.cancel();
    m_tcp_socket.close();
}
void Connection::shutdown(ISocket::ShutdownType type)
{
    assert(! m_closed_already);
    m_tcp_socket.shutdown(boost::asio::socket_base::shutdown_send);
}
void Connection::cancel()
{
    m_tcp_socket.cancel();
}
void Connection::setReadTimeout(long millisecs)
{
    m_read_timeout_interval_ms = millisecs;
}
long Connection::getReadTimeout()
{
    return m_read_timeout_interval_ms;
}
#pragma mark - public interface async io operations
void Connection::asyncAccept(
    boost::asio::ip::tcp::acceptor&                     acceptor,
    std::function<void(const boost::system::error_code& err)> cb
)
{
    acceptor.async_accept(m_tcp_socket, [cb, this](const boost::system::error_code& err) {
        TROG_WARN("before");
        TROG_TRACE_FD(nativeSocketFD());
        TROG_WARN("after");
        p_post_accept_cb(cb, err);
    });
}

void Connection::asyncConnect(ConnectCallbackType connect_cb)
{
    m_connect_cb = connect_cb; // save the connect callback

    tcp::resolver::query query(this->m_server, this->m_port);
    #ifdef TIMEOUT_ON_RESOLVE
    m_timeout.setTimeout(m_connect_timeout_interval_ms, [this](){
        m_tcp_socket.cancel();
    });
    #endif
    auto h = ([this](const error_code& err, tcp::resolver::iterator endpoint_iterator) {
        #ifdef TIMEOUT_ON_RESOLVE
        m_timeout.cancelTimeout([this, err, endpoint_iterator](){
            p_handle_resolve(err, endpoint_iterator);
        });
        #else
            p_handle_resolve(err, endpoint_iterator);
        #endif
    });
    m_resolver.async_resolve(query, h);
}

void Connection::becomeSecureClient(X509_STORE* certificate_store_ptr)
{
    if (m_mode != NOTSECURE) {
        MARVIN_THROW("connection already secured");
    }
    m_ssl_ctx_sptr = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::method::sslv23);
    m_ssl_ctx_sptr->set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    SSL_CTX* raw_ssl_ctx_ptr = m_ssl_ctx_sptr->native_handle();
    m_mode = Mode::SECURE_CLIENT;
    m_certificate_store_ptr = certificate_store_ptr;
    SSL_CTX_set_cert_store(raw_ssl_ctx_ptr, m_certificate_store_ptr);
    // m_ssl_ctx_sptr->set_verify_mode(boost::asio::ssl::context::verify_peer);
    m_ssl_ctx_sptr->set_verify_mode(boost::asio::ssl::verify_peer);
    /// setup openssl to verify host name
    // now make the ssl stream using the tcp_socket and the ssl_ctx
    m_ssl_stream_sptr = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>>(m_tcp_socket, *m_ssl_ctx_sptr);
    /// set the host/server name for SNI by the server
    /// must be done before setting VERIFY_PARAM
    SSL_set_tlsext_host_name(m_ssl_stream_sptr->native_handle(), m_server.c_str());    
 
    X509_VERIFY_PARAM *param;
    param = X509_VERIFY_PARAM_new();
    X509_VERIFY_PARAM_set_flags(param, X509_V_FLAG_CRL_CHECK);
    X509_VERIFY_PARAM_set1_host(param, m_server.c_str(), m_server.size());
    SSL_CTX_set1_param(m_ssl_ctx_sptr->native_handle(), param);
    X509_VERIFY_PARAM_free(param);
 
 }
void Connection::becomeSecureServer(Cert::Identity server_identity)
{
    if (m_mode != NOTSECURE) {
        MARVIN_THROW("connection already secured");
    }
    m_ssl_ctx_sptr = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::method::sslv23);
    m_mode = Mode::SECURE_SERVER;
    m_server_identity = server_identity;
    m_ssl_ctx_sptr->set_options(
        ssl::context::default_workarounds | ssl::context::no_sslv2
    );
    SSL_CTX* raw_ssl_ctx_ptr = m_ssl_ctx_sptr->native_handle();

    X509* tmp_x509 = server_identity.getX509();
    EVP_PKEY* tmp_evp_pkey = server_identity.getEVP_PKEY();

    SSL_CTX_use_certificate(raw_ssl_ctx_ptr, tmp_x509);
    SSL_CTX_use_PrivateKey(raw_ssl_ctx_ptr, tmp_evp_pkey);

    // X509_free(tmp_x509);
    // EVP_PKEY_free(tmp_evp_pkey);

    m_ssl_stream_sptr = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>>(m_tcp_socket, *m_ssl_ctx_sptr);

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
        MARVIN_THROW("cannot get server certificate until after successful handshake");
    }
    return m_server_certificate;
}
/*
* Read interface 
*/

/** not implemented*/
void Connection::asyncRead(boost::asio::streambuf& streambuffer, AsyncReadCallbackType cb)
{
    MARVIN_THROW("not implemented yet");
}
/** read some into a Marvin MBuffer  and update the size property*/
void Connection::asyncRead(Marvin::MBufferSPtr buffer, AsyncReadCallbackType cb)
{
    asyncRead(buffer, m_read_timeout_interval_ms, cb);
}
/** read some into a boost::asio::mutable buffer*/
void Connection::asyncRead(boost::asio::mutable_buffer buffer, AsyncReadCallbackType cb)
{
    asyncRead(buffer.data(), buffer.size(), cb);
}
/** read some into a buffer .egnth pair */
void Connection::asyncRead(void* buffer, std::size_t length, AsyncReadCallbackType cb)
{
    asyncRead(buffer, length, m_read_timeout_interval_ms, cb);
}
/** read some with explicit timeout into a Marvin MBuffer and update its size property*/
void Connection::asyncRead(Marvin::MBufferSPtr mbuffer, long timeout_ms, AsyncReadCallbackType cb)
{
    asyncRead(mbuffer->data(), mbuffer->capacity(), timeout_ms, 
    [this, mbuffer, cb](const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        Marvin::ErrorType m_err = err;
        mbuffer->setSize(bytes_transfered);
        p_post_read_cb(cb, m_err, bytes_transfered);
    });
}
/** the heavy lifting - read with explicit timeout into a pointer length pair */
void Connection::asyncRead(void* buffer, std::size_t buffer_length, long timeout_ms, AsyncReadCallbackType cb)
{
    /// a bit of explanation -
    /// -   set a time out with a handler, the handler knows what to do, in this case cancel outstanding
    ///     ops on the socket
    m_timeout.setTimeout(timeout_ms, [this](){
        m_tcp_socket.cancel();
    });
    auto handler = ([this, cb, buffer](const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        /// when a handler is called the first thing to do is call timeout.cancel()
        /// when timeout object is finshed it will call the CB and then we can complete
        /// our processing knowing that both the IO and tineout are both done
        m_timeout.cancelTimeout([this, cb, buffer, err, bytes_transfered](){
            Marvin::ErrorType m_err = err;
            p_post_read_cb(cb, m_err, bytes_transfered);
        });
    });
    if (m_mode == NOTSECURE) {
        m_tcp_socket.async_read_some(boost::asio::buffer(buffer, buffer_length), handler);
    } else {
        m_ssl_stream_sptr->async_read_some(boost::asio::buffer(buffer, buffer_length), handler);
    }
}

/**
 * write
 */

void Connection::asyncWrite(Marvin::BufferChainSPtr buf_chain_sptr, AsyncWriteCallback cb)
{
    /// this took a while to work out - change buffer code at your peril
    TROG_DEBUG("");
    auto tmp = buf_chain_sptr->asio_buffer_sequence();
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        p_post_write_cb(cb, err, bytes_transfered);
    });
    if (m_mode == NOTSECURE) {
        boost::asio::async_write((this->m_tcp_socket), tmp, handler);
    } else {
        boost::asio::async_write((*m_ssl_stream_sptr), tmp, handler);
    }
}

void Connection::asyncWrite(Marvin::MBuffer& buf, AsyncWriteCallbackType cb)
{
    p_async_write((void*)buf.data(), buf.size(), cb);
}
void Connection::asyncWrite(std::string& str, AsyncWriteCallback cb)
{
    p_async_write( (void*)str.c_str(), str.size(), cb);
}
void Connection::asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback cb)
{
    TROG_DEBUG("");
    auto handler = ([this, &sb, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        p_post_write_cb(cb, err, bytes_transfered);
    });
    if (m_mode == NOTSECURE) {
        boost::asio::async_write((this->m_tcp_socket), sb, handler);
    } else {
        boost::asio::async_write((*m_ssl_stream_sptr), sb, handler);
    }
}
void Connection::asyncWrite(boost::asio::const_buffer constbuffer, AsyncWriteCallback cb)
{
    TROG_DEBUG("");
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        p_post_write_cb(cb, err, bytes_transfered);
    });
    if (m_mode == NOTSECURE) {
        boost::asio::async_write((this->m_tcp_socket), constbuffer, handler);
    } else {
        boost::asio::async_write((*m_ssl_stream_sptr), constbuffer, handler);
    }
}
void Connection::asyncWrite(void* buffer, std::size_t buffer_length, AsyncWriteCallback cb)
{
    TROG_DEBUG("");
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        p_post_write_cb(cb, err, bytes_transfered);
    });
    if (m_mode == NOTSECURE) {
        boost::asio::async_write((this->m_tcp_socket), boost::asio::buffer(buffer, buffer_length), handler);
    } else {
        boost::asio::async_write((*m_ssl_stream_sptr), boost::asio::buffer(buffer, buffer_length), handler);
    }
}

#pragma mark - internal methods
void Connection::p_handle_resolve(
                    const error_code& err,
                    tcp::resolver::iterator endpoint_iterator)
{
    TROG_DEBUG("entry error: ", err.message());

    /// iterator empty ?
    tcp::resolver::iterator end;
    auto iter_empty = (endpoint_iterator == end);
    Marvin::ErrorType ec = err;
    TROG_DEBUG(std::string(__FUNCTION__) + " " + Marvin::make_error_description(ec) );
    if (iter_empty && (!err)) {
        TROG_DEBUG("empry but no error");
        // empty iter but no error (unlikely)
        boost::system::error_code local_err = boost::system::errc::make_error_code(boost::system::errc::host_unreachable);
        p_post_connect_cb(m_connect_cb, local_err, this);
    } else if (err) {
        // got an error
        p_post_connect_cb(m_connect_cb, err, this);
    } else {
        // all is good
        tcp::endpoint ep = *endpoint_iterator;
        TROG_DEBUG( ep.address().to_string());
        // always use the first iterator result
        TROG_DEBUG("resolve OK","so now connect");
        tcp::endpoint endpoint = *endpoint_iterator;
        
        m_timeout.setTimeout(m_connect_timeout_interval_ms, [this](){
            m_tcp_socket.cancel();
        });
        auto next_iter = ++endpoint_iterator;
        auto h = ([this, next_iter](const error_code& err) {
            m_timeout.cancelTimeout([this, err, next_iter](){
                p_handle_connect(err, next_iter);
            });
        });
        m_tcp_socket.async_connect(endpoint, h);
        TROG_DEBUG("leaving");
    }
}


void Connection::p_handle_connect(
                    const boost::system::error_code& err,
                    tcp::resolver::iterator endpoint_iterator)
{
    Marvin::ErrorType  ec = err;
    TROG_DEBUG("entry: ", Marvin::make_error_description(ec));
    if (!err)
    {
       TROG_TRACE_FD(nativeSocketFD());
        m_tcp_socket.non_blocking(true);
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
        TROG_DEBUG("try next iterator");
        tcp::endpoint endpoint = *endpoint_iterator;
        m_timeout.setTimeout(m_connect_timeout_interval_ms, [this](){
            m_tcp_socket.cancel();
        });
        auto next_iter = ++endpoint_iterator;
        auto h = ([this, next_iter](const error_code& err) {
            m_timeout.cancelTimeout([this, err, next_iter](){
                p_handle_connect(err, next_iter);
            });
        });
        m_tcp_socket.async_connect(endpoint, h);
    }
    else
    {
        TROG_ERROR("resolve FAILED","Error: ",err.message());
        p_post_connect_cb(m_connect_cb, err, this);
    }
    TROG_DEBUG("leaving");
}
void Connection::p_start_handshake()
{
    boost::asio::ssl::stream_base::handshake_type handshake_type;
    if (m_mode == SECURE_CLIENT) {
        handshake_type = boost::asio::ssl::stream_base::client;        
    } else if (m_mode == SECURE_SERVER) {
        handshake_type = boost::asio::ssl::stream_base::server;
    } else {
        MARVIN_THROW("Invalid handshake_type in p_start_handshake");
    }
    m_ssl_stream_sptr->async_handshake(
        handshake_type,
        boost::bind(&Connection::p_handle_handshake, this, boost::asio::placeholders::error)
    );
}
void Connection::p_handle_handshake(const boost::system::error_code& err)
{
    if(! err) {
        if (m_mode == SECURE_CLIENT) {
            X509* server_cert = SSL_get_peer_certificate(this->m_ssl_stream_sptr->native_handle());
            m_server_certificate = Cert::Certificate(server_cert);

            #ifdef MARVIN_HTTPS_TRACE
            STACK_OF(X509*) st = SSL_get_peer_cert_chain(this->m_ssl_stream_sptr->native_handle());
            auto n = sk_X509_num(st);
            TROG_TRACE3("SERVER CERTIFICATE CHAIN:");
            for(int i = 0; i < n; i++) {
                X509* cert = sk_X509_value(st, i);
                TROG_TRACE3("Server Chain index: ", i );
                Cert::Certificate cc{cert};
                std::string ssm1 = cc.getIssuerNameAsOneLine();
                std::string ssm2 = cc.getSubjectNameAsOneLine(); 
                if( i == 0) {
                    std::string ssm3 = cc.getSubjectAlternativeNamesAsString();
                    TROG_TRACE3("    subjectname: ", ssm2);
                    TROG_TRACE3("    certificate issuer     : ", ssm1);
                    TROG_TRACE3("    certificate altnames   : ", ssm3);
                } else {
                    TROG_TRACE3("    subjectname: ", ssm2);
                    TROG_TRACE3("    certificate issuer     : ", ssm1);
                }
            }
            TROG_TRACE3("SERVER CERTIFICATE :");
            Cert::Certificate cc{server_cert};
            std::string ssm1 = cc.getIssuerNameAsOneLine();
            std::string ssm2 = cc.getSubjectNameAsOneLine(); 
            std::string ssm3 = cc.getSubjectAlternativeNamesAsString();
            TROG_TRACE3("subjectname: ", ssm2);
            TROG_TRACE3("issuer     : ", ssm1);
            TROG_TRACE3("altnames   : ", ssm3);
            #endif
        }
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
    TROG_DEBUG("");
    auto handler = ([this, cb]( const Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        TROG_DEBUG("");
        p_post_write_cb(cb, err, bytes_transfered);
    });
    if (m_mode == NOTSECURE) {
        boost::asio::async_write((this->m_tcp_socket), boost::asio::buffer(data, size), handler);
    } else {
        boost::asio::async_write((*m_ssl_stream_sptr), boost::asio::buffer(data, size), handler);
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
} // namespace
