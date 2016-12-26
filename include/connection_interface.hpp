
#ifndef connection_interface_hpp
#define connection_interface_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
//#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "read_socket_interface.hpp"
#include "buffer.hpp"
#include "connection_interface.hpp"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;


using ip::tcp;
using system::error_code;


ConnectionInterface* connectionFactory(
            boost::asio::io_service& io_service,
            const std::string& scheme,
            const std::string& server,
            const std::string& port
);
ConnectionInterface* connectionFactory(
            boost::asio::io_service& io_service,
            const std::string& scheme
);

//--------------------------------------------------------------------------------------------------
// Interface to specify the interface to sockets that do SSL/TLS and sockets that do NOT
//--------------------------------------------------------------------------------------------------
class ConnectionInterface;
typedef std::shared_ptr<ConnectionInterface> ConnectionInterfaceSPtr;
typedef std::unique_ptr<ConnectionInterface> ConnectionInterfaceUPtr;

//typedef std::shared_ptr<ConnectionInterface> ConnectionPtr;


class ConnectionInterface : public ReadSocketInterface, public WriteSocketInterface
{
    public:
#ifdef CONN_IF_NOCONTRUCT
    // client socket needs to know who to connect to
    ConnectionInterface(
            boost::asio::io_service& io_service,
            const std::string& scheme,
            const std::string& server,
            const std::string& port
    );
    
    // server socket will be connected via listen/accept
    virtual ConnectionInterface(
        boost::asio::io_service& io_service
    );
    
    
#endif
    virtual ~ConnectionInterface()=0;

    virtual void asyncConnect(ConnectCallbackType cb) = 0;
    virtual void asyncAccept(
        boost::asio::ip::tcp::acceptor& acceptor,
        std::function<void(const boost::system::error_code& err)> cb
    ) = 0;
    
    virtual void asyncWrite(FBuffer& fb, AsyncWriteCallbackType cb) = 0;
    virtual void asyncWriteStreamBuf(boost::asio::streambuf& sb, AsyncWriteCallback) = 0;

    virtual void asyncRead(MBuffer& mb,  AsyncReadCallbackType cb) = 0;
    virtual void shutdown() = 0;
    virtual void close() = 0;
    
    virtual long nativeSocketFD() = 0;
    
    virtual std::string scheme() = 0;
    virtual std::string server() = 0;
    virtual std::string service() = 0;
};
#endif