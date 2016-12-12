
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

class Connection : public ReadSocketInterface, public WriteSocketInterface
{
    public:
    Connection(boost::asio::io_service& io_service,
            const std::string& scheme,
            const std::string& server,
            const std::string& port
               );
    
    void asyncConnect(ConnectCallbackType cb);
    
    void asyncWrite(FBuffer& fb, AsyncWriteCallbackType cb);
    void asyncWriteStreamBuf(boost::asio::streambuf& sb, AsyncWriteCallback);

    void asyncRead(MBuffer& mb,  AsyncReadCallbackType cb);


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
    
    boost::asio::ip::tcp::resolver  _resolver;
    boost::asio::ip::tcp::socket    _boost_socket;
    ConnectCallbackType             _finalCb;
};

#endif