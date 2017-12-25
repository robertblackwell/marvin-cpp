
#ifndef i_socket_hpp
#define i_socket_hpp

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


//--------------------------------------------------------------------------------------------------
// Interface to specify the interface to sockets that do SSL/TLS and sockets that do NOT
//--------------------------------------------------------------------------------------------------
typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncReadCallback;
typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncWriteCallback;

class IReadSocket;
typedef std::shared_ptr<IReadSocket> IReadSocketSPtr;

/**
* \brief Interface defines all the read methods common to a tcp and an ssl socket
*/
class IReadSocket{
public:
    virtual void asyncRead(MBuffer& mb, AsyncReadCallback cb) = 0;
};

/**
* \brief Interface defines all the write methods common to a tcp and an ssl socket
*/
class IWriteSocket;
typedef std::shared_ptr<IWriteSocket> IWriteSocketSPtr;

class IWriteSocket{
public:
    virtual void asyncWrite(std::string& str, AsyncWriteCallbackType cb) = 0;
    virtual void asyncWrite(MBuffer& fb, AsyncWriteCallback) = 0;
    virtual void asyncWrite(BufferChainSPtr chain_sptr, AsyncWriteCallback) = 0;
    virtual void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb) = 0;
    virtual void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback) = 0;
};

/**
* \brief Interface defiines all the methods read, write, connect, accept, shutdown, close common to tcp and ssl sockets
*/
class ISocket;

using ISocketSPtr = std::shared_ptr<ISocket>;
using ISocketUPtr = std::unique_ptr<ISocket>;

class ISocket : public IReadSocket, public IWriteSocket
{
    public:
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
