
#ifndef tcp_connection_hpp
#define tcp_connection_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>

//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/function.hpp>

#include "boost_stuff.hpp"

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


class TCPConnection;
typedef std::shared_ptr<TCPConnection> TCPConnectionSPtr;
typedef std::unique_ptr<TCPConnection> TCPConnectionUPtr;

//--------------------------------------------------------------------------------------------------
// NON SSL/TLS Connection
//--------------------------------------------------------------------------------------------------
class TCPConnection : public ConnectionInterface
{
    public:
    static std::map<int, int>   socket_fds_inuse;
    static void fd_inuse(int fd);
    static void fd_free(int fd);

    // client socket needs to know who to connect to
#if 0
    TCPConnection(
            boost::asio::io_service& io_service,
            const std::string& scheme,
            const std::string& server,
            const std::string& port
               );
#endif
    TCPConnection(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
               );
    // server socket will be connected via listen/accept
    TCPConnection(
        boost::asio::io_service& io_service
    );
    
    ~TCPConnection();
    
    void asyncConnect(ConnectCallbackType cb);
    void asyncAccept(boost::asio::ip::tcp::acceptor& acceptor, std::function<void(const boost::system::error_code& err)> cb);
    
    void asyncWrite(MBuffer& buffer, AsyncWriteCallbackType cb);
    void asyncWrite(std::string& str, AsyncWriteCallbackType cb);
    void asyncWrite(BufferChainSPtr buf_chain_sptr, AsyncWriteCallback cb);
    void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb);
    void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback);

    void asyncRead(MBuffer& mb,  AsyncReadCallbackType cb);
    void shutdown();
    void close();
    
    long nativeSocketFD();
    
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

    void async_write(void* data, std::size_t size, AsyncWriteCallback cb);

    void completeWithError(Marvin::ErrorType& ec);
    void completeWithSuccess();
    
    void post_accept_cb(std::function<void(boost::system::error_code& err)> cb, Marvin::ErrorType err);
    void post_connect_cb(ConnectCallbackType  cb, Marvin::ErrorType err);
    void post_read_cb(AsyncReadCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    void post_write_cb(AsyncWriteCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    
    void resolveCompleteWithSuccess();
    void connectionCompleteWithSuccess();

    void io_post(std::function<void()> noParam_cb);


    std::string                     _scheme;
    std::string                     _server;
    std::string                     _port;
    boost::asio::io_service&        _io;
    boost::asio::strand             _strand;
    boost::asio::ip::tcp::resolver  _resolver;
    boost::asio::ip::tcp::socket    _boost_socket;
    ConnectCallbackType             _finalCb;
    bool                            _closed_already; 
};


#endif
