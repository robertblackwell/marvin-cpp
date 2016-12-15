

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <cassert>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "url.hpp"
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
    std::string __url(url);
    http::url parsed = http::ParseHttpUrl(url);
    Url u(__url);
    std::string ___scheme = u.scheme();
    //
    // this user staff will go in the Auth header
    //
    std::string ___user = u.user_info();
    std::string ___host = u.host();
    std::string ___port = u.port();
    std::string ___path = u.path();
    Url::Query query = u.query();
    
    // only used by the browser never sendin the http message
    std::string ___fragment = u.fragment();
    
    std::string __scheme = parsed.protocol;
    std::string __host = parsed.host;
    if( ___port == "") {
        if( ___scheme == "http"){
            _port = "80";
            _service = "http";
        }else if(___scheme == "https"){
            _port = "443";
            _service = "https";
        }else{
            assert(false);
        }
    }else{
        _port = ___port;
        _service = ___port;
    }
    
    std::string __path = ___path;
    if( ___path == "" ) { __path = "/"; }
    
    _query = query;
    std::string q_str("");
    if( query.size() > 0 ){
        for(auto const& keyVal: query){
            if( q_str == "")
                q_str += keyVal.key() + "=" + keyVal.val();
            else
                q_str += "&"+keyVal.key() + "=" + keyVal.val();
        }
        q_str = "?" + q_str;
    }
    _queryStr = q_str;
    std::ostringstream host_s, path_s;
    host_s << __host << ":" << _port ;

    path_s << __path << _queryStr;
    _scheme = __scheme;
    _server = __host;
    _host = host_s.str();
    _uri = path_s.str();
    _path = path_s.str();
    setUri(_path);
}

void Request::asyncGetWriteSocket(ConnectCallbackType connectCb)
{
    ClientConnectionManager* cm = ClientConnectionManager::getInstance(_io);
    if( _writeSock == nullptr){
//        std::string scheme("http:");
//        std::string server("localhost");
//        std::string port("9991");

        std::string scheme = _scheme;
        std::string server = _server;
        std::string port   = _port;
        std::string service = _service;
        
        cm->asyncGetClientConnection(
                scheme,
                server,
                service,
        [this, connectCb](Marvin::ErrorType& ec, ClientConnection* conn){
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
