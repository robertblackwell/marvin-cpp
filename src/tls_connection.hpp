
#ifndef tls_connection_hpp
#define tls_connection_hpp

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
#include "read_socket_interface.hpp"
#include "buffer.hpp"
#include "connection_interface.hpp"

//using namespace boost;
//using namespace boost::system;
//using namespace boost::asio;
//
//
//using ip::tcp;
//using system::error_code;
//using boost::asio::ip::tcp;
////namespace ssl = boost::asio::ssl;
typedef boost::asio::ssl::stream<tcp::socket> SslSocketType;

//--------------------------------------------------------------------------------------------------
// SSL/TLS Connection
//--------------------------------------------------------------------------------------------------
class TLSConnection : public ConnectionInterface
{
    public:
    // client socket needs to know who to connect to
    TLSConnection(
            boost::asio::io_service& io_service,
            const std::string& scheme,
            const std::string& server,
            const std::string& port
               );
    // server socket will be connected via listen/accept
    TLSConnection(
        boost::asio::io_service& io_service
    );
    
    ~TLSConnection();
    
    void asyncConnect(ConnectCallbackType cb);
    void asyncAccept(boost::asio::ip::tcp::acceptor& acceptor, std::function<void(const boost::system::error_code& err)> cb);
    
    void asyncWrite(FBuffer& fb, AsyncWriteCallbackType cb);
    void asyncWriteStreamBuf(boost::asio::streambuf& sb, AsyncWriteCallback);

    void asyncRead(MBuffer& mb,  AsyncReadCallbackType cb);
    void shutdown();
    void close();
    
    long nativeSocketFD();
    
    std::string scheme();
    std::string server();
    std::string service();
    
private:
    void handleResolve(
        const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator
    );
    
    void handleConnect(
        const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator
    );
    void handleConnectHandshake(const boost::system::error_code& error);


    void completeWithError(Marvin::ErrorType& ec);
    void completeWithSuccess();
    bool verifyCertificate(bool preverified, boost::asio::ssl::verify_context& ctx);


    std::string                     _scheme;
    std::string                     _server;
    std::string                     _port;
    io_service&                     _io;
    tcp::resolver                   _resolver;
    
    std::unique_ptr<tcp::socket>                    _boostSocketUPtr;
    std::unique_ptr<SslSocketType>                  _boostSslSocketUPtr;
    std::unique_ptr<ssl::context>                   _sslContextUPtr;

    ConnectCallbackType             _finalCb;

};

#endif