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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#include "boost_stuff.hpp"
#pragma clang diagnostic pop
#include "url.hpp"
#include "UriParser.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "message_reader.hpp"
#include "socket_factory.hpp"
#include "request.hpp"
#include "client.hpp"
#include "tcp_connection.hpp"


using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::io_service;
using boost::asio::streambuf;
using namespace Marvin;
using namespace Marvin::Http;

Client::Client(
    boost::asio::io_service& io, 
    std::string scheme, 
    std::string server, 
    std::string port
): m_io(io), m_scheme(scheme), m_server(server), m_port(port)
{}
Client::Client(
    boost::asio::io_service& io, 
    Marvin::Uri uri
): m_io(io), m_scheme(uri.scheme()), m_server(uri.server()), m_port(std::to_string(uri.port()))
{

}

//Client::Client(boost::asio::io_service& io, std::string url)
//: _io(io), _url(url)
//{
//    setupUrl(url);
//}

Client::Client(
    boost::asio::io_service& io, 
    ISocketSPtr conn
): m_io(io)
{
    m_conn_shared_ptr = conn;
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
    if (m_conn_shared_ptr != nullptr ) {
        throw "should not have a connection at this point";
    }
#if 0
    TCPConnection* ptr = new TCPConnection(m_io, m_scheme, m_server, m_port);
    m_conn_shared_ptr = std::shared_ptr<TCPConnection>(ptr);
#else
    m_conn_shared_ptr = socketFactory(false, m_io,m_scheme,m_server, m_port);
#endif

    auto f = [this, cb](Marvin::ErrorType& ec, ISocket* c) {
        std::string er_s = Marvin::make_error_description(ec);
        LogInfo(" conn", (long)m_conn_shared_ptr.get(), " er: ", er_s);
        cb(ec);
    };
    m_conn_shared_ptr->asyncConnect(f);
}


#if 1
//--------------------------------------------------------------------------------
// asyncWrite - the whole request message but first check all the header fields are
// ok
//--------------------------------------------------------------------------------
void Client::asyncWrite(MessageBaseSPtr requestMessage,  std::string& body, ResponseHandlerCallbackType cb)
{
    m_body_mbuffer_sptr = Marvin::MBuffer::makeSPtr(body);
    _async_write(requestMessage, cb);
}
void Client::asyncWrite(MessageBaseSPtr requestMessage,  Marvin::MBufferSPtr body, ResponseHandlerCallbackType cb)
{
    m_body_mbuffer_sptr = body;
    _async_write(requestMessage, cb);
}
void Client::asyncWrite(MessageBaseSPtr requestMessage,  Marvin::BufferChainSPtr chain_sptr, ResponseHandlerCallbackType cb)
{
    m_body_mbuffer_sptr = chain_sptr->amalgamate();
    _async_write(requestMessage, cb);
}
void Client::asyncWrite(MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb)
{
    m_body_mbuffer_sptr  = Marvin::MBuffer::makeSPtr(""); // no body
    _async_write(requestMessage, cb);
}
void Client::_async_write(MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb)
{
    LogInfo("", (long)this);
    m_response_handler = cb;
    m_current_request = requestMessage;
    
    bool already_connected = (m_conn_shared_ptr != nullptr);
    
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
            this->_rdr = std::shared_ptr<MessageReader>(new MessageReader(_conn_ptr, _io));
            // get a writer
            TCPConnection& conRef = *_conn_uniq_ptr;
            this->_wrtr = std::shared_ptr<MessageWriter>(new MessageWriter(_io, conRef));
#endif
            internalWrite();
        } else {
            m_response_handler(ec, m_rdr);
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
    this->m_rdr = std::shared_ptr<MessageReader>(new MessageReader(m_io, m_conn_shared_ptr));
    // get a writer
    this->m_wrtr = std::shared_ptr<MessageWriter>(new MessageWriter(m_io, m_conn_shared_ptr));
#endif
    // we are about to write the entire request message
    // so make sure we have the content-length correct
    setContentLength();
    LogInfo("",traceWriter(*m_wrtr));
    
    assert(m_body_mbuffer_sptr != nullptr);
    m_wrtr->asyncWrite(m_current_request, m_body_mbuffer_sptr, [this](Marvin::ErrorType& ec){
        if (!ec) {
            LogDebug("start read");
            this->m_rdr->readMessage([this](Marvin::ErrorType ec){
                if (!ec) {
                    this->m_response_handler(ec, m_rdr);
                } else {
                    this->m_response_handler(ec, m_rdr);
                }
            });
        } else {
            this->m_response_handler(ec, m_rdr);
        }
    });
}
void Client::close()
{
    m_conn_shared_ptr->close();
    m_rdr = nullptr;
    m_wrtr = nullptr;
    m_conn_shared_ptr = nullptr;
}
void Client::end()
{
//    _connection = nullptr;
}

void Client::setContentLength()
{
    long len = 0;
    MessageBaseSPtr msg = m_current_request;
    if( m_body_mbuffer_sptr != nullptr ) {
        len = m_body_mbuffer_sptr->size();
    }
    msg->setHeader(Marvin::Http::Headers::Name::ContentLength, std::to_string(len));
}
MessageReaderSPtr Client::getResponse()
{
    return m_rdr;
}
