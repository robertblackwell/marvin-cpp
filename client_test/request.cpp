

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "UriParser.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "request.hpp"
#include "client_connection_manager.hpp"

using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::io_service;
using boost::asio::streambuf;

Request::~Request()
{
    LogDebug("");
}

Request::Request(boost::asio::io_service& io): _io(io), MessageWriter(io, true)
{
    _writeSock = nullptr;
}

MessageReader& Request::getResponse()
{
    return *_rdr;
}
void Request::setUrl(std::string url)
{
    http::url parsed = http::ParseHttpUrl(url);
    std::string _scheme = parsed.protocol;
    std::string _host = parsed.host;
    std::string _port = (parsed.port == 0)? "" : ":"+std::to_string(parsed.port);
    std::string _path = parsed.path;
    std::string _query = parsed.search;
    std::ostringstream host_s, path_s;
    host_s << _host << _port ;
    path_s << _path << "?" << _query;
    std::string host = host_s.str();
    _uri = path_s.str();
    setHeader("Host", host);
//    _uri = url;
}

void Request::asyncGetWriteSocket(ConnectCallbackType connectCb)
{
    ClientConnectionManager* cm = ClientConnectionManager::getInstance(_io);
    if( _writeSock == nullptr){
        std::string scheme("http:");
        std::string server("whiteacorn");
        std::string port("80");
        cm->asyncGetClientConnection(scheme, server, port, [this, connectCb](Marvin::ErrorType& ec, ClientConnection* conn){
            LogDebug("");
            if( ec ){
                Marvin::ErrorType m_er = ec;
                connectCb(m_er, nullptr);
            } else {
                Marvin::ErrorType m_er = Marvin::make_error_ok();
                connectCb(m_er, conn);
            }
        });
        
    }else{
        
    }
}

void Request::fullWriteHandler(Marvin::ErrorType& err)
{
    LogDebug("");
    std::cout << "Request::fullWriteHandler:: " << std:: hex << this << std::endl;
    _rdr->readMessage([this](Marvin::ErrorType& err){
        ClientConnectionManager* cm = ClientConnectionManager::getInstance(_io);
        cm->releaseClientConnection(_connection);
        _connection = nullptr;
        auto pf = std::bind(this->_goCb, err);
        _io.post(pf);
    
    });
}
//
// we are connected start read and write
//
void Request::haveConnection(Marvin::ErrorType& err, ClientConnection* conn)
{
    if( !err ){
    std::cout << "Request::haveConnection:: " << std:: hex << this << " conn: " << conn <<  std::endl;
    
        // give the MessageWrite base class instance a write socket
        this->_connection = conn;
        this->setWriteSock(conn);
        // create a MessageReader with a read socket
        this->_rdr = std::shared_ptr<MessageReader>(new MessageReader(conn, _io));
        // set up read also

        auto cf = std::bind(&Request::fullWriteHandler, this, std::placeholders::_1);
        this->asyncWrite(cf);
    }else{
        LogError("");
    }
}
//
// Get connected
//
void Request::go(std::function<void(Marvin::ErrorType& err)> cb)
{
    _goCb = cb;
    auto cf = std::bind(&Request::haveConnection, this, std::placeholders::_1, std::placeholders::_2);
    asyncGetWriteSocket(cf);
}
