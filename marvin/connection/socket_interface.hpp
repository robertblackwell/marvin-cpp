
#ifndef marvin_socket_interface_hpp
#define marvin_socket_interface_hpp
/**
* \defgroup SocketIO
*/
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <marvin/boost_stuff.hpp>
#include <cert/cert_identity.hpp>
#include <cert/cert_certificate.hpp>
#include <marvin/callback_typedefs.hpp>
#include <marvin/error/marvin_error.hpp>
#include <marvin/buffer/buffer.hpp>

using namespace boost;
using namespace boost::system;
using namespace boost::asio;

/// \ingroup SocketIO
using AsyncReadCallback = std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)>;
/// \ingroup SocketIO
using AsyncWriteCallback = std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)>;
class ISocket;

/// \ingroup SocketIO
using ISocketSPtr = std::shared_ptr<ISocket>;
/// \ingroup SocketIO
using ISocketUPtr = std::unique_ptr<ISocket>;

/**
* \ingroup SocketIO
* \brief Interface defines all the methods read, write, connect, accept, shutdown, close common to tcp and ssl sockets
*/
class ISocket //: public IReadSocket, public IWriteSocket
{
    public:
    using SPtr = std::shared_ptr<ISocket>;
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

    virtual void asyncRead(Marvin::MBufferSPtr mb, AsyncReadCallback cb) = 0;
    virtual void asyncWrite(std::string& str, AsyncWriteCallbackType cb) = 0;
    virtual void asyncWrite(Marvin::MBuffer& fb, AsyncWriteCallback) = 0;
    virtual void asyncWrite(Marvin::BufferChainSPtr chain_sptr, AsyncWriteCallback) = 0;
    virtual void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb) = 0;
    virtual void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback) = 0;

    virtual void setReadTimeout(long millisecs) = 0;
    virtual void shutdown() = 0;
    virtual void close() = 0;
    virtual ~ISocket() = 0;
};
inline ISocket::~ISocket() {}
#endif
