
#ifndef marvin_connection_socket_interface_hpp
#define marvin_connection_socket_interface_hpp

#include <string>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <cert/cert_identity.hpp>
#include <cert/cert_certificate.hpp>
#include <marvin/error/marvin_error.hpp>

namespace Marvin {

using namespace ::boost;
using namespace ::boost::system;
using namespace ::boost::asio;

using AsyncReadCallback = std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)>;
using AsyncWriteCallback = std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)>;
class ISocket;

using ISocketSPtr = std::shared_ptr<ISocket>;
using ISocketUPtr = std::unique_ptr<ISocket>;



/**
* \ingroup connection
* \brief ISocket interface defines all the methods read, write, connect, accept, shutdown, close 
* that are common to tcp and ssl sockets. This is obsolete as there is only one type of
* connection and it can be both plain and ssl.
*/
class ISocket //: public IReadSocket, public IWriteSocket
{
    public:
    using SPtr = std::shared_ptr<ISocket>;
    using ConstBoostErrRefHandler = std::function<void(const boost::system::error_code& err)>;
    using ErrRefAndBytesHandler  = std::function<void(ErrorType& err, std::size_t bytes_transfered)>;
    using ReadHandler = ErrRefAndBytesHandler ;
    using WriteHandler = ErrRefAndBytesHandler ;
    using ConnectHandler = std::function<void(ErrorType& err, ISocket* socket)>;

    enum ShutdownType {
        ShutdownSend = boost::asio::ip::tcp::socket::shutdown_send,
        ShutdownReceive = boost::asio::ip::tcp::socket::shutdown_receive,
        ShutdownBoth = boost::asio::ip::tcp::socket::shutdown_both,
    };


    virtual long native_socket_fd() = 0;
    virtual void async_connect(ConnectHandler cb) = 0;
    virtual void async_accept(
        boost::asio::ip::tcp::acceptor& acceptor,
        std::function<void(const boost::system::error_code& err)> cb
    ) = 0;
    virtual void become_secure_client(X509_STORE* certificate_store_ptr) = 0;
    virtual void become_secure_server(Cert::Identity server_identity) =0;
    virtual void async_handshake(std::function<void(const boost::system::error_code& err)> cb)=0;
    
    virtual Cert::Certificate get_server_certificate() =0 ;

    virtual void async_read(Marvin::ContigBuffer::SPtr mb, ReadHandler cb) = 0;
    virtual void async_read(boost::asio::streambuf& buffer, ReadHandler cb) = 0;
    virtual void async_read(boost::asio::mutable_buffer buffer, ReadHandler cb) = 0;
    virtual void async_read(void* buf, std::size_t length, ReadHandler cb) = 0;

    virtual void async_read(Marvin::ContigBuffer::SPtr buffer, long timeout_ms, ReadHandler cb) = 0;
    virtual void async_read(void* buffer, std::size_t length, long timeout_ms, ReadHandler cb) = 0;
    
    virtual void async_write(std::string& str, WriteHandler cb) = 0;
    
    virtual void async_write(Marvin::BufferChain::SPtr chain_sptr, WriteHandler) = 0;
    virtual void async_write(Marvin::ContigBuffer& fb, WriteHandler) = 0;
    
    virtual void async_write(boost::asio::streambuf& sb, WriteHandler) = 0;
    virtual void async_write(boost::asio::const_buffer buf, WriteHandler cb) = 0;

    virtual void async_write(void* buffer, std::size_t buffer_length, WriteHandler) = 0;


    virtual void set_read_timeout(long millisecs) = 0;
    virtual long get_read_timeout() = 0;
    virtual void shutdown(ShutdownType type) = 0;
    // cancel current operation
    virtual void cancel() = 0;
    virtual boost::asio::io_service& get_io_context() = 0;
    virtual boost::asio::ssl::context& get_ssl_context() = 0;

    #ifdef MARVIN_ISOCKET_DEFINE_SHUTDOWN
    virtual void shutdownSend() = 0;
    virtual void shutdownReceive() = 0;
    #endif
    virtual void close() = 0;
};

} // namespace
#endif
