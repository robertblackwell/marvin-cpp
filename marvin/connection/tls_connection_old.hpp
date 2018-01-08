
#ifndef marvin_tls_connection_hpp
#define marvin_tls_connection_hpp
/**
* \ingroup SocketIO
*/
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include "boost_stuff.hpp"
#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "i_socket.hpp"
#include "buffer.hpp"
#include "i_socket.hpp"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;


using ip::tcp;
using system::error_code;
using boost::asio::ip::tcp;
//namespace ssl = boost::asio::ssl;
using SslSocketType = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

/**
* \ingroup SocketIO
* \brief Provides a bi-directional TLS connection that carries TCP traffic.
*/
class TLSConnection : public ISocket
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
    
    void asyncWrite(Marvin::MBuffer& fb, AsyncWriteCallbackType cb);
    void asyncWriteStreamBuf(boost::asio::streambuf& sb, AsyncWriteCallback);

    void asyncRead(Marvin::MBuffer& mb,  AsyncReadCallbackType cb);
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


    std::string                     m_scheme;
    std::string                     m_server;
    std::string                     m_port;
    io_service&                     m_io;
    strand                          m_strand;
    tcp::resolver                   m_resolver;
    
    std::unique_ptr<tcp::socket>                    m_boostSocketUPtr;
    std::unique_ptr<SslSocketType>                  m_boostSslSocketUPtr;
    std::unique_ptr<ssl::context>                   m_sslContextUPtr;

    ConnectCallbackType             m_finalCb;

};

#endif
