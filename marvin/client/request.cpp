

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <cassert>
#include "boost_stuff.hpp"
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/function.hpp>
#include "url.hpp"
#include "UriParser.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)
#include "message_reader.hpp"

#include "request.hpp"
#include "tcp_connection.hpp"

using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::io_service;
using boost::asio::streambuf;

std::string traceRequest(Request& request)
{
    std::stringstream ss;
    ss << "Request: ";
    if( request._connection != NULL ){
        ss << "conn: " << std::hex << (long)request._connection;
        ss << std::dec << "FD: " << request._connection->nativeSocketFD();
    }else{
        ss << "conn:" << "NULL";
    }
    return ss.str();
}


Request::~Request()
{
    LogInfo("");
}

Request::Request(boost::asio::io_service& io): _io(io), MessageWriter(io, true)
{
    LogInfo("");
    _writeSock = nullptr;
    _oneTripOnly = true;
}
/*!--------------------------------------------------------------------------------
* getter for the response message inside this request
*--------------------------------------------------------------------------------*/
MessageReader& Request::getResponse()
{
    return *_rdr;
}
/*!--------------------------------------------------------------------------------
* sets the url for the request, parses the url into components and saves them
* in particular deduces
*  -   scheme
*  -   host - both with and without port appended
*  -   port
*  -   path/uri
*  -   query string
*--------------------------------------------------------------------------------*/
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
    setHeader("Host",_host);

    setUri(_path);
}
//--------------------------------------------------------------------------------
// set default values for headers if they have not already been set
// does not know how to handle proxied requests
// for the moment insists on Connection::close
//--------------------------------------------------------------------------------
void Request::defaultHeaders()
{

    if( ! hasHeader("Connection") ) setHeader("Connection", "close");
    if( ! hasHeader("Host") ) setHeader("Host",_host);
    if( ! hasHeader("User-agent") ) setHeader("User-agent", "Marvin-proxy");
    if( ! hasHeader("Date") ){
        time_t rawtime;
        time (&rawtime);
        struct tm * timeinfo;
        char buffer [80];
        timeinfo = localtime (&rawtime);

        strftime (buffer,80,"%c %Z",timeinfo);

        std::string s(buffer);
        setHeader("Date",s);
    }
}
//--------------------------------------------------------------------------------
// go - execute the request
//--------------------------------------------------------------------------------
void Request::go(std::function<void(Marvin::ErrorType& err)> cb)
{
    LogInfo("", (long)this);
    _goCb = cb;
    defaultHeaders();
    
    auto cf = std::bind(&Request::haveConnection, this, std::placeholders::_1, std::placeholders::_2);

//    asyncGetWriteSocket(cf);

//    ConnectionInterface* conn = connectionFactory(_io, _scheme, _server, _port);
    ConnectionInterface* conn = (ConnectionInterface*) new TCPConnection(_io, _scheme, _server, _port);
    //
    // a bunch of logic here about find existing, add to connection table etc
    //
    conn->asyncConnect([this, conn, cb](Marvin::ErrorType& ec, ConnectionInterface* conn){
    std::string er_s = Marvin::make_error_description(ec);
        LogInfo(" conn", (long)conn, " er: ", er_s);
        this->haveConnection(ec, conn);
    });

}

void Request::end()
{
//    _connection = nullptr;
}
//--------------------------------------------------------------------------------
// get a socket from the connection manager for this request
//--------------------------------------------------------------------------------
void Request::asyncGetWriteSocket(ConnectCallbackType connectCb)
{
#ifdef SSSS

//    ConnectionManager* cm = ConnectionManager::getInstance(_io);
    ConnectionPool* cm = ConnectionPool::getInstance(_io);
    if( _writeSock == nullptr){
//        std::string scheme("http:");
//        std::string server("localhost");
//        std::string port("9991");

        std::string scheme = _scheme;
        std::string server = _server;
        std::string port   = _port;
        std::string service = _service;
        
        cm->asyncGetConnection(
                scheme,
                server,
                service,
        [this, connectCb](Marvin::ErrorType& ec, ConnectionInterface* conn){
            if( ec ){
                LogWarn("", (long)this, " connection: ", (long)conn);
                Marvin::ErrorType m_er = ec;
                connectCb(m_er, nullptr);
            } else {
                LogInfo("", (long)this, " connection: ", (long)conn, " FD:",conn->nativeSocketFD());
                Marvin::ErrorType m_er = Marvin::make_error_ok();
                connectCb(m_er, conn);
            }
        });
        
    }else{
        
    }
#endif
}


//--------------------------------------------------------------------------------
// we are connected start read and write
//--------------------------------------------------------------------------------
void Request::haveConnection(Marvin::ErrorType& err, ConnectionInterface* conn)
{
    if( !err ){
    
        // give the MessageWrite base class instance a write socket
        this->_connection = conn;
        this->setWriteSock(conn);

        LogInfo("", (long)this, " connection: ", (long)_connection, " FD:",_connection->nativeSocketFD());

        // create a MessageReader with a read socket
        this->_rdr = std::shared_ptr<MessageReader>(new MessageReader(_io, conn));
        // set up read also
        LogInfo("", traceRequest(*this), traceWriter(*this));
        auto cf = std::bind(&Request::fullWriteHandler, this, std::placeholders::_1);
        LogTrace(traceWriter(*this));
        this->asyncWrite(cf);
    }else{
        LogError("");
        assert(false);
    }
}
//--------------------------------------------------------------------------------
// write is finished - start read
//--------------------------------------------------------------------------------
void Request::fullWriteHandler(Marvin::ErrorType& err)
{
    LogInfo("", (long)this, " connection: ", (long)_connection, " FD:",_connection->nativeSocketFD());
    LogInfo("", traceRequest(*this), traceWriter(*this));
    
    auto rh = std::bind(&Request::readComplete, this, std::placeholders::_1);
    
    _rdr->readMessage([this](Marvin::ErrorType& err){
//        if(err){
//            LogTrace(Marvin::make_error_description(err));
//        }else{
//            LogTrace(traceReader(*_rdr));
//        }
        LogInfo("readMessage callback");
        auto pf = std::bind(this->_goCb, err);
        _io.post(pf);
    
    });
}
//--------------------------------------------------------------------------------
// the round-trip cycle is complete - write messages - followed by read message
//
// now determine whether we need to close the connection.
//--------------------------------------------------------------------------------
void Request::readComplete(Marvin::ErrorType& err)
{
    LogInfo(" oneTripOnly", _oneTripOnly);
    LogInfo("", (long)this, " connection: ", (long)_connection, " FD:",_connection->nativeSocketFD());
    LogInfo("", traceRequest(*this), traceWriter(*this), traceReader(*(this->_rdr)));
    if( _oneTripOnly){
        auto pf = std::bind(this->_goCb, err);
        _io.post(pf);
    }
}
