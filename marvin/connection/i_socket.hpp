
#ifndef marvin_i_socket_hpp
#define marvin_i_socket_hpp
/**
* \defgroup SocketIO
*/
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
//#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "i_socket.hpp"
#include "buffer.hpp"
#include "i_socket.hpp"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;


/// \ingroup SocketIO
using AsyncReadCallback = std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)>;
/// \ingroup SocketIO
using AsyncWriteCallback = std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)>;

class IReadSocket;
/// \ingroup SocketIO
 using IReadSocketSPtr = std::shared_ptr<IReadSocket>;

/**
* \ingroup SocketIO
* \brief Interface defines all the read methods common to a tcp and an ssl socket
*/
class IReadSocket{
public:
    virtual void asyncRead(Marvin::MBuffer& mb, AsyncReadCallback cb) = 0;
};

class IWriteSocket;
/// \ingroup SocketIO
using IWriteSocketSPtr = std::shared_ptr<IWriteSocket>;

/**
* \ingroup SocketIO
* \brief Interface defines all the write methods common to a tcp and an ssl socket
*/
class IWriteSocket{
public:
    virtual void asyncWrite(std::string& str, AsyncWriteCallbackType cb) = 0;
    virtual void asyncWrite(Marvin::MBuffer& fb, AsyncWriteCallback) = 0;
    virtual void asyncWrite(Marvin::BufferChainSPtr chain_sptr, AsyncWriteCallback) = 0;
    virtual void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb) = 0;
    virtual void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback) = 0;
};

class ISocket;

/// \ingroup SocketIO
using ISocketSPtr = std::shared_ptr<ISocket>;
/// \ingroup SocketIO
using ISocketUPtr = std::unique_ptr<ISocket>;

/**
* \ingroup SocketIO
* \brief Interface defines all the methods read, write, connect, accept, shutdown, close common to tcp and ssl sockets
*/
class ISocket : public IReadSocket, public IWriteSocket
{
    public:
    using SPtr = std::shared_ptr<ISocket>;
    virtual long nativeSocketFD() = 0;
    virtual void asyncConnect(ConnectCallbackType cb) = 0;
    virtual void asyncAccept(
        boost::asio::ip::tcp::acceptor& acceptor,
        std::function<void(const boost::system::error_code& err)> cb
    ) = 0;   
    virtual void shutdown() = 0;
    virtual void close() = 0;
};
#endif