
#ifndef Connection_hpp
#define Connection_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "read_socket_interface.hpp"
#include "buffer.hpp"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;


using ip::tcp;
using system::error_code;

class ClientConnection : public ReadSocketInterface, public WriteSocketInterface
{
    public:
    // client socket needs to know who to connect to
    ClientConnection(
            boost::asio::io_service& io_service,
            const std::string& scheme,
            const std::string& server,
            const std::string& port
               );
    // server socket will be connected via listen/accept
    ClientConnection(
        boost::asio::io_service& io_service
    );
    
    ~ClientConnection();
    
    void asyncConnect(ConnectCallbackType cb);
    
    void asyncWrite(FBuffer& fb, AsyncWriteCallbackType cb);
    void asyncWriteStreamBuf(boost::asio::streambuf& sb, AsyncWriteCallback);

    void asyncRead(MBuffer& mb,  AsyncReadCallbackType cb);
    void close();
    boost::asio::ip::tcp::socket&   getSocketRef();
    int nativeSocketFD();
    
    std::string scheme();
    std::string server();
    std::string service();
    
private:

    void handle_resolve(
        const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator
    );
    
    void handle_connect(
        const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator
    );

    void completeWithError(Marvin::ErrorType& ec);
    void completeWithSuccess();


    std::string                     _scheme;
    std::string                     _server;
    std::string                     _port;
    boost::asio::io_service&        _io;
    boost::asio::ip::tcp::resolver  _resolver;
    boost::asio::ip::tcp::socket    _boost_socket;
    ConnectCallbackType             _finalCb;
};

#endif