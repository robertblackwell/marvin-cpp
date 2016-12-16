//
//  ConnectionHandler.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/12/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#
#include "connection_handler.hpp"
#include "request_handler.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

ConnectionHandler::ConnectionHandler(
    boost::asio::io_service&    io,
    ServerConnectionManager&    connectionManager,
    RequestHandlerInterface&    requestHandler,
    ClientConnection*           conn
):  _io(io),
    _connectionManager(connectionManager),
    _requestHandler(requestHandler)
//    _connection(conn)
{

#ifdef CON_SMARTPOINTER
    _connection = std::unique_ptr<ClientConnection>(conn);
#else
    _connection = conn;
#endif
}
ConnectionHandler::~ConnectionHandler()
{
#ifdef CON_SMARTPOINTER
#else
    delete _connection;
#endif
    LogDebug("");
    
}
void ConnectionHandler::close()
{
    LogDebug(" fd:", nativeSocketFD());
//    _connection->close();
}
int ConnectionHandler::nativeSocketFD()
{
    return _connection->nativeSocketFD();
}
void ConnectionHandler::requestComplete()
{
    // a stub for future expansionto have a handler handle
    // multiple requests from the same connection
}
void ConnectionHandler::handlerComplete()
{
    LogDebug(" fd:", nativeSocketFD());
    _connection->close();
    //
    // This call will start the process of deleting linked objects. Hence we need to have closed the
    // connection before this because after it we may not have the connection to close
    //
    _connectionManager.stop(this); // should be maybe called deregister
    
}

void ConnectionHandler::readMessageHandler(Marvin::ErrorType& err)
{
    LogDebug(" fd:", nativeSocketFD());
    if( err ){
        LogDebug("error value: ", err.value(),
            " category: ", err.category().name(),
            " msg: ", err.category().message(err.value()));
            //
            // On read error do not call the handler - simply abort the request
            //
            this->handlerComplete();
    } else{
        _requestHandler.handle_request(_io, *_reader, *_writer, [this](bool good){
            LogDebug("");
            //
            // should check here for Connection::close/keep-alive
            // and if keep-alive call requestComplete to read another request
            //
            this->handlerComplete();
        } );
    }
}
//
// This is a setup - the read is initiated somewhere else
//
void ConnectionHandler::serve()
{
    LogDebug(" fd:", nativeSocketFD());
    // set up reader and writer
#ifdef CON_SMARTPOINTER
    ClientConnection* cptr = _connection.get();
#else
    ClientConnection* cptr = _connection;
#endif
    
#ifdef CH_SMARTPOINTER
    _reader = std::unique_ptr<MessageReader>(new MessageReader(cptr, _io));
    _writer = std::unique_ptr<MessageWriter>(new MessageWriter(_io, false));
#else
    _reader = new MessageReader(_connection, _io);
    _writer = new MessageWriter(_io, false);
#endif
    _writer->setWriteSock(cptr);
    auto rmh = std::bind(&ConnectionHandler::readMessageHandler, this, std::placeholders::_1 );
    _reader->readMessage(rmh);
    
}
