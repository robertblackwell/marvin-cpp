
#ifndef marvin_connection_socket_interface_hpp
#define marvin_connection_socket_interface_hpp

#include <string>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/callback_typedefs.hpp>
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

    enum ShutdownType {
        ShutdownSend = boost::asio::ip::tcp::socket::shutdown_send,
        ShutdownReceive = boost::asio::ip::tcp::socket::shutdown_receive,
        ShutdownBoth = boost::asio::ip::tcp::socket::shutdown_both,
    };


    virtual long nativeSocketFD() = 0;
    virtual void asyncConnect(ConnectCallbackType cb) = 0;
    virtual void asyncAccept(
        boost::asio::ip::tcp::acceptor& acceptor,
        std::function<void(const boost::system::error_code& err)> cb
    ) = 0;
    virtual void becomeSecureClient(X509_STORE* certificate_store_ptr) = 0;
    virtual void becomeSecureServer(Cert::Identity server_identity) =0;
    virtual void asyncHandshake(std::function<void(const boost::system::error_code& err)> cb)=0;
    
    virtual Cert::Certificate getServerCertificate() =0 ;

    virtual void asyncRead(Marvin::ContigBuffer::SPtr mb, AsyncReadCallback cb) = 0;
    virtual void asyncRead(boost::asio::streambuf& buffer, AsyncReadCallback cb) = 0;
    virtual void asyncRead(boost::asio::mutable_buffer buffer, AsyncReadCallback cb) = 0;
    virtual void asyncRead(void* buf, std::size_t length, AsyncReadCallback cb) = 0;

    virtual void asyncRead(Marvin::ContigBuffer::SPtr buffer, long timeout_ms, AsyncReadCallbackType cb) = 0;
    virtual void asyncRead(void* buffer, std::size_t length, long timeout_ms, AsyncReadCallbackType cb) = 0;
    
    virtual void asyncWrite(std::string& str, AsyncWriteCallbackType cb) = 0;
    
    virtual void asyncWrite(Marvin::BufferChain::SPtr chain_sptr, AsyncWriteCallback) = 0;
    virtual void asyncWrite(Marvin::ContigBuffer& fb, AsyncWriteCallback) = 0;
    
    virtual void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback) = 0;
    virtual void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb) = 0;

    virtual void asyncWrite(void* buffer, std::size_t buffer_length, AsyncWriteCallback) = 0;


    virtual void setReadTimeout(long millisecs) = 0;
    virtual long getReadTimeout() = 0;
    virtual void shutdown(ShutdownType type) = 0;
    // cancel current operation
    virtual void cancel() = 0;
    virtual boost::asio::io_service& getIO() = 0;
    virtual boost::asio::ssl::context& getSslContext() = 0;

    #ifdef MARVIN_ISOCKET_DEFINE_SHUTDOWN
    virtual void shutdownSend() = 0;
    virtual void shutdownReceive() = 0;
    #endif
    virtual void close() = 0;
};

} // namespace
#endif
