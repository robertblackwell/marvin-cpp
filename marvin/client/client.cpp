//
//  client.cpp
//  all
//
//  Created by ROBERT BLACKWELL on 11/24/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//


#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <memory>
#include <cassert>
#include "boost_stuff.hpp"
#include "url.hpp"
#include "UriParser.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "message_reader.hpp"

#include "request.hpp"
#include "client.hpp"
#include "tcp_connection.hpp"


using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::io_service;
using boost::asio::streambuf;


Client::Client(boost::asio::io_service& io, std::string scheme, std::string server, std::string port)
: _io(io), _scheme(scheme), _server(server), _port(port)
{

}

Client::Client(boost::asio::io_service& io, std::string url)
: _io(io), _url(url)
{
    setupUrl(url);
}

Client::Client(boost::asio::io_service& io, ConnectionInterface* conn) : _io(io)
{

}


Client::~Client()
{
    LogInfo("");
}

/*!--------------------------------------------------------------------------------
* implement connect
*--------------------------------------------------------------------------------*/
void Client::asyncConnect(ErrorOnlyCallbackType cb)
{
    LogInfo("", (long)this);
//    std::cout << "client asyncConnect " << std::hex << (long) this << std::endl;
    if (_conn_shared_ptr != nullptr ) {
        throw "should not have a connection at this point";
    }
    
    TCPConnection* ptr = new TCPConnection(_io, _scheme, _server, _port);
    
    _conn_shared_ptr = std::shared_ptr<TCPConnection>(ptr);
    auto f = [this, cb](Marvin::ErrorType& ec, ConnectionInterface* c) {
        std::string er_s = Marvin::make_error_description(ec);
        LogInfo(" conn", (long)_conn_shared_ptr.get(), " er: ", er_s);
        cb(ec);
    };
    _conn_shared_ptr->asyncConnect(f);
}


#if 1
//--------------------------------------------------------------------------------
// asyncWrite - the whole request message but first check all the header fields are
// ok
//--------------------------------------------------------------------------------
void Client::asyncWrite(MessageBaseSPtr requestMessage,  std::string& body, ResponseHandlerCallbackType cb)
{
    _body_mbuffer_sptr = m_buffer(body);
    _async_write(requestMessage, cb);
}
void Client::asyncWrite(MessageBaseSPtr requestMessage,  MBufferSPtr body, ResponseHandlerCallbackType cb)
{
    _body_mbuffer_sptr = body;
    _async_write(requestMessage, cb);
}
void Client::asyncWrite(MessageBaseSPtr requestMessage,  BufferChainSPtr chain_sptr, ResponseHandlerCallbackType cb)
{
    _body_mbuffer_sptr = chain_sptr->amalgamate();
    _async_write(requestMessage, cb);
}
void Client::asyncWrite(MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb)
{
    _body_mbuffer_sptr  = m_buffer(""); // no body
    _async_write(requestMessage, cb);
}
void Client::_async_write(MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb)
{
    LogInfo("", (long)this);
    _response_handler = cb;
    _current_request = requestMessage;
    defaultHeaders();
    
    bool already_connected = (_conn_shared_ptr != nullptr);
    
    if ( ! already_connected ) {
        internalConnect();
    }else {
        internalWrite();
    }
}
#endif
void Client::internalConnect()
{
    LogInfo("", (long)this);
    asyncConnect([this](Marvin::ErrorType& ec){
        LogDebug("cb-connect");
        if(!ec) {

#ifndef RDR_WRTR_ONESHOT
            this->_rdr = std::shared_ptr<MessageReaderV2>(new MessageReaderV2(_conn_ptr, _io));
            // get a writer
            TCPConnection& conRef = *_conn_uniq_ptr;
            this->_wrtr = std::shared_ptr<MessageWriterV2>(new MessageWriterV2(_io, conRef));
#endif
            internalWrite();
        } else {
            _response_handler(ec, _rdr);
        }
    });
}
//--------------------------------------------------------------------------------
// come here to do a write of the full message
//--------------------------------------------------------------------------------
void Client::internalWrite()
{
    LogInfo("", (long)this);

#ifdef RDR_WRTR_ONESHOT
    // set up the read of the response
    // create a MessageReader with a read socket
    this->_rdr = std::shared_ptr<MessageReaderV2>(new MessageReaderV2(_io, _conn_shared_ptr));
    // get a writer
    this->_wrtr = std::shared_ptr<MessageWriterV2>(new MessageWriterV2(_io, _conn_shared_ptr));
#endif

    if( _on_headers_handler != nullptr ) {
        this->_rdr->readHeaders([this](Marvin::ErrorType ec){
            if (!ec) {
                this->_on_headers_handler(ec, _rdr);
                if( _on_data_handler != nullptr ) {
                    this->_rdr->readBody([this](Marvin::ErrorType err, BufferChain buf_chain){
                        _on_data_handler(err, buf_chain);
                    });
                }
            } else {
                this->_on_headers_handler(ec, _rdr);
            }
        });

    } else {
        this->_rdr->readMessage([this](Marvin::ErrorType ec){
            if (!ec) {
                this->_response_handler(ec, _rdr);
            } else {
                this->_response_handler(ec, _rdr);
            }
        });
    }
    
    // we are about to write the entire request message
    // so make sure we have the content-length correct
    setContentLength();
    LogInfo("",traceWriterV2(*_wrtr));
    
    assert(_body_mbuffer_sptr != nullptr);
    _wrtr->asyncWrite(_current_request, _body_mbuffer_sptr, [this](Marvin::ErrorType& ec){
        if (!ec) {
            // do nothing - let the read happen
            LogDebug("do nothing");
        } else {
            this->_response_handler(ec, _rdr);
        }
    });
}
void Client::end()
{
//    _connection = nullptr;
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
void Client::setupUrl(std::string url)
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
//            _service = "http";
        }else if(___scheme == "https"){
            _port = "443";
//            _service = "https";
        }else{
            assert(false);
        }
    }else{
        _port = ___port;
//        _service = ___port;
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
//    _host = __host;
    _host_with_port = host_s.str();
    _uri = path_s.str();
    _path = path_s.str();
    _uri = _path;
}

//--------------------------------------------------------------------------------
// set default values for headers if they have not already been set
// does not know how to handle proxied requests
// for the moment insists on Connection::close
//--------------------------------------------------------------------------------
void Client::defaultHeaders()
{

    MessageBaseSPtr msg = _current_request;
    msg->setUri(_path);
    msg->setIsRequest(true);
    msg->setHeader("Host",_server);
    msg->setHeader("User-agent", "Marvin-proxy");
    if( ! msg->hasHeader("Connection") ) msg->setHeader("Connection", "close");
    if( ! msg->hasHeader("Date") ){
        time_t rawtime;
        time (&rawtime);
        struct tm * timeinfo;
        char buffer [80];
        timeinfo = localtime (&rawtime);

        strftime (buffer,80,"%c %Z",timeinfo);

        std::string s(buffer);
        msg->setHeader("Date",s);
    }
}
void Client::setContentLength()
{
    long len = 0;
    MessageBaseSPtr msg = _current_request;
    if( _body_mbuffer_sptr != nullptr ) {
        len = _body_mbuffer_sptr->size();
    }
    msg->setHeader(HttpHeader::Name::ContentLength, std::to_string(len));
}
MessageReaderV2SPtr Client::getResponse()
{
    return _rdr;
}

void Client::setOnHeaders(ResponseHandlerCallbackType cb)
{
    _on_headers_handler = cb;
}

void Client::setOnData(ClientDataHandlerCallbackType cb)
{
    _on_data_handler = cb;
}
