//
//  ConnectionHandler.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/12/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

template<class TRequestHandler>
ConnectionHandler<TRequestHandler>::ConnectionHandler(
    boost::asio::io_service&                                        io,
    ServerConnectionManager<ConnectionHandler<TRequestHandler>>&    connectionManager,
    ConnectionInterface*                                            conn):  _io(io), _connectionManager(connectionManager)
{
    _requestHandlerPtr  = new TRequestHandler(_io);
    _requestHandlerUnPtr = std::unique_ptr<TRequestHandler>(_requestHandlerPtr);
    
#ifdef CON_SMARTPOINTER
    _connection = std::shared_ptr<ConnectionInterface>(conn);
#else
    _connection = conn;
#endif
}


template<class TRequestHandler>
ConnectionHandler<TRequestHandler>::~ConnectionHandler()
{
#ifdef CON_SMARTPOINTER
#else
    delete _connection;
#endif
    LogDebug("");
    
}
template<class TRequestHandler>
void ConnectionHandler<TRequestHandler>::close()
{
    LogDebug(" fd:", nativeSocketFD());
//    _connection->close();
}
/*!
*   Utility method returns the underlying FD for this connection.
*   Used only for debug racing purposes
*/
template<class TRequestHandler>
long ConnectionHandler<TRequestHandler>::nativeSocketFD()
{
    return _connection->nativeSocketFD();
}

/*!
* Come here when a the CONNECT handler calls its "done" callback
*/
template<class TRequestHandler>
void ConnectionHandler<TRequestHandler>::handleConnectComplete(Marvin::ErrorType& err, bool hijacked)
{
    // do not want the connction closed unless !hijacked
    LogInfo(" fd:", nativeSocketFD());
    
    if( ! hijacked )
        _connection->close();
    
    _connectionManager.deregister(this); // should be maybe called deregister
}
/*!
* Called when a request/response cycle is complete and starts a read
* to initiate another request/response cycle or to wait for the client to
* close the connection
*/
template<class TRequestHandler>
void ConnectionHandler<TRequestHandler>::requestComplete(Marvin::ErrorType& err, bool keepAlive)
{
    /*!
    * start serving the next request/response cycle
    */
    if(!err){
        try{
//            _connection->shutdown();
            this->serveAnother();
            }
            catch (std::exception& e)
            {
                LogError("exception: ", e.what());
            }
    }else{
        _connection->close();
    }
}

/*!
* Come here when the latest request/response cycle is complete and
* the client has indicated no more requests/
*/
template<class TRequestHandler>
void ConnectionHandler<TRequestHandler>::handlerComplete(Marvin::ErrorType& err)
{
    LogInfo(" fd:", nativeSocketFD());
    if(!err)
        _connection->close();
    //
    // This call will start the process of deleting linked objects. Hence we need to have closed the
    // connection before this because after it we may not have the connection to close
    //
    _connectionManager.deregister(this); // should be maybe called deregister
    
}
/*!
* Come here after a request message has been successfully read,
* this is the first step in the request/response cycle
*/
template<class TRequestHandler>
void ConnectionHandler<TRequestHandler>::readMessageHandler(Marvin::ErrorType& err)
{
    LogInfo(" fd:", nativeSocketFD());
    LogError("error value: ", err.value(),
        " category: ", err.category().name(),
        " msg: ", err.category().message(err.value()));
    if( err ){
        LogError("error value: ", err.value(),
            " category: ", err.category().name(),
            " msg: ", err.category().message(err.value()));
            //
            // On read error do not call the handler - simply abort the request
            //
            this->handlerComplete(err);
    } else{
        if(_reader->method() == HttpMethod::CONNECT ){
            LogWarn("CONNECT request");
             _requestHandlerUnPtr->handleConnect(_reader, _connection, [this](Marvin::ErrorType& err, bool hijack){
                this->handleConnectComplete(err, hijack);
             });
        } else {
            _requestHandlerUnPtr->handleRequest(_reader, _writer, [this](Marvin::ErrorType& err, bool keepAlive){
                LogInfo("");
                this->requestComplete(err, keepAlive);
            } );
        }
    }
    LogInfo(" fd:", nativeSocketFD());
}
/*!
* Come here to start the read of a request message, ahdnhence start a request/response cycle
*/
template<class TRequestHandler>
void ConnectionHandler<TRequestHandler>::serve()
{
    LogInfo(" fd:", nativeSocketFD());
    // set up reader and writer
#ifdef CON_SMARTPOINTER
    ConnectionInterface* cptr = _connection.get();
#else
    ConnectionInterface* cptr = _connection;
#endif
    
#ifdef CH_SMARTPOINTER
    _reader = std::shared_ptr<MessageReader>(new MessageReader(cptr, _io));
    _writer = std::shared_ptr<MessageWriter>(new MessageWriter(_io, false));
#else
    _reader = new MessageReader(_connection, _io);
    _writer = new MessageWriter(_io, false);
#endif
    _writer->setWriteSock(cptr);
    auto rmh = std::bind(&ConnectionHandler::readMessageHandler, this, std::placeholders::_1 );
    _reader->readMessage(rmh);
    
}
/*!
* Serve the enxt request/response cycle - deliberately DO NOT create new MessageReader
* and MessageWriter objects - this is a effort to keep the same connection/socket open
* for the next request
*/
template<class TRequestHandler>
void ConnectionHandler<TRequestHandler>::serveAnother()
{
    LogInfo(" fd:", nativeSocketFD());
    /// get a new request object
    _requestHandlerPtr  = new TRequestHandler(_io);
    _requestHandlerUnPtr = std::unique_ptr<TRequestHandler>(_requestHandlerPtr);

    auto rmh = std::bind(&ConnectionHandler::readMessageHandler, this, std::placeholders::_1 );
    _reader->readMessage(rmh);
}
