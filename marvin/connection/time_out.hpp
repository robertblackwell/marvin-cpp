
#ifndef tcp_connection_hpp
#define tcp_connection_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
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
// Mixin to add timeout management to other classes
//--------------------------------------------------------------------------------------------------
class TimeOut
{
    TimeOut(boost::asio::io_service& io_service);
    ~TimeOut();
    
protected:

    
    void post_accept_cb(std::function<void(boost::system::error_code& err)> cb, Marvin::ErrorType err);
    void post_connect_cb(ConnectCallbackType  cb, Marvin::ErrorType err, ConnectionInterface* conn);
    void post_read_cb(AsyncReadCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    void post_write_cb(AsyncWriteCallbackType cb, Marvin::ErrorType err, std::size_t bytes_transfered);
    
    void resolveCompleteWithSuccess();
    void connectionCompleteWithSuccess();

    void io_post(std::function<void()> noParam_cb);
    
    void handle_timeout(const boost::system::error_code& err);
    void cancel_timeout();
    void set_timeout(long interval_millisecs);


    std::string                     _scheme;
    std::string                     _server;
    std::string                     _port;
    boost::asio::io_service&        _io;
    boost::asio::strand             _strand;
    boost::asio::ip::tcp::resolver  _resolver;
    boost::asio::ip::tcp::socket    _boost_socket;
    ConnectCallbackType             _finalCb;
    bool                            _closed_already;
    boost::asio::deadline_timer     _timer;
};


#endif
