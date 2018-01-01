//
//  ConnectionHandler.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/12/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include "http_header.hpp"
#include "connection_handler.hpp"
#include "server_connection_manager.hpp"
#include "http_server.hpp"
#include "server_context.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

ConnectionHandler::ConnectionHandler(
    boost::asio::io_service&    io,
    ServerConnectionManager&    connectionManager,
    ISocket*                    conn,
    RequestHandlerFactory       factory
):  m_io(io),
    m_connectionManager(connectionManager),
    m_uuid(boost::uuids::random_generator()()),
    m_factory(factory)
{
    LogTorTrace();
    /**
    * The connection and the request handler persist acrosss all messages served
    * by a connection handler. This is required to ensure that our MITM proxy
    * can handle keep-alive
    */
    m_connection = std::shared_ptr<ISocket>(conn);
    m_requestHandlerUnPtr = std::unique_ptr<RequestHandlerBase>(m_factory(m_io));
    m_server_context.server_ptr = HTTPServer::get_instance();
    m_server_context.connection_handler_ptr = this;
    m_server_context.server_connection_manager_ptr = &connectionManager;
    m_server_context.connection_ptr = conn;
    LogDebug("");

}


ConnectionHandler::~ConnectionHandler()
{
    LogTorTrace();
    m_requestHandlerUnPtr = nullptr;
    m_connection = nullptr;
    m_reader = nullptr;
    m_writer = nullptr;
}

/*!
*   Utility method returns the underlying FD for this connection.
*   Used only for debug racing purposes
*/
long ConnectionHandler::nativeSocketFD()
{
    return m_connection->nativeSocketFD();
}
std::string ConnectionHandler::uuid()
{
    return boost::uuids::to_string(m_uuid);
}
/*!
* Come here when a the CONNECT handler calls its "done" callback
*/
void ConnectionHandler::p_handle_connect_complete(bool hijacked)
{
    // do not want the connction closed unless !hijacked
    LogInfo(" fd:", nativeSocketFD());
    
    if( ! hijacked ) {
        assert(false); // requires a decision about how to manage closing connections
        m_connection->close();
    }
    m_connectionManager.deregister(this); // should be maybe called deregister
}
/*!
* Called when a request/response cycle is complete and starts a read
* to initiate another request/response cycle or to wait for the client to
* close the connection. Do both these things
*/
void ConnectionHandler::p_request_complete(Marvin::ErrorType err, bool keepAlive)
{
    /*!
    * start serving the next request/response cycle
    */
    if(!err){
            if (keepAlive) {
                this->p_serve_another();
            } else {
                this->p_serve_another();   // this is a hack - wait for the cline to close the connection
                                        // we will get a zero bytes read or error
//                _connection->shutdown(); // let the next read take place - that will complete the shutdown
            }
    }else{
        LogDebug(err.message());
        assert(false); /// we are not handling errors correctly - we dont deregister the connection handler
        m_connection->close(); //TODO - this is wrong I think
    }
}

/*!
* Come here when the latest request/response cycle is complete and
* the client has indicated no more requests probably by closing the
* connection and causing an error on read. In anycase
* start the processing of releasing objects associated with this connection handler
*/
void ConnectionHandler::p_handler_complete(Marvin::ErrorType err)
{
    #if 0
    LogInfo(" fd:", nativeSocketFD(), " err: ", err.message());
    if(!err){
        assert(false);
        m_connection->close(); /// this is wrong or at least undecided
    }
    #endif
    
    // TODO - this does not close the sockst  - change to make that happen
    m_connectionManager.deregister(this); // should be maybe called deregister
    
}
/*!
* Come here after a request message has been successfully read,or on a read error
* For a successful read this is the first step in the request/response cycle.
* For an error this is often the result of client closing the connection after
* one or a series of request response cycles. Either way an error starts the process of
* releasing all objects associated with a connection and request handler
*/
void ConnectionHandler::p_read_message_handler(Marvin::ErrorType err)
{
    LogInfo(" fd:", nativeSocketFD());
    LogInfo("", Marvin::make_error_description(err));
//    LogError("error value: ", err.value(),
//        " category: ", err.category().name(),
//        " msg: ", err.category().message(err.value()));
    std::string uuid_str = boost::uuids::to_string(m_uuid);
    if( err ){
        LogError("error value: ", err.value(),
            " category: ", err.category().name(),
            " msg: ", err.category().message(err.value()));
            //
            // On read error do not call the handler - simply abort the request
            //
            this->p_handler_complete(err);
    } else{
        if(m_reader->method() == HttpMethod::CONNECT ){
            LogWarn("CONNECT request");
             m_requestHandlerUnPtr->handleConnect(m_server_context, m_reader, m_connection, [this](Marvin::ErrorType& err, bool keepAlive){
                this->p_request_complete(err, false);
             });
        } else {
            LogTrace(traceMessage(*m_reader));
            
            m_requestHandlerUnPtr->handleRequest(m_server_context, m_reader, m_writer, [this](Marvin::ErrorType& err, bool keepAlive){
                LogInfo("");
                this->p_serve_another();
//                this->requestComplete(err, keepAlive);
            } );
        }
    }
    LogInfo(" fd:", nativeSocketFD());
}
/*!
* Come here to start the read of a request message, ahdnhence start a request/response cycle
*/
void ConnectionHandler::serve()
{
    LogInfo(" fd:", nativeSocketFD());
//    std::cout << "connection_handler::serve " << std::hex << (long) this << std::endl;
//    ISocket* cptr = _connection.get();
    
//    _requestHandlerUnPtr = std::unique_ptr<RequestHandlerBase>(_factory(_io));
    m_reader = std::shared_ptr<MessageReader>(new MessageReader(m_io, m_connection));
    m_writer = std::shared_ptr<MessageWriter>(new MessageWriter(m_io, m_connection));

    auto rmh = std::bind(&ConnectionHandler::p_read_message_handler, this, std::placeholders::_1 );
    m_reader->readMessage(rmh);
    
}
/*!
* Serve the next request/response cycle - deliberately DO NOT create new MessageReader
* and MessageWriter objects - this is a effort to keep the same connection/socket open
* for the next request
*/
void ConnectionHandler::p_serve_another()
{
    LogInfo(" fd:", nativeSocketFD());
    /// get a new request object
//    ISocket* cptr = _connection.get();
//    std::cout << "connection_handler::serveAnother " << std::hex << (long) this << std::endl;

//    _requestHandlerUnPtr = std::unique_ptr<RequestHandlerBase>(_factory(_io));
    m_reader = std::shared_ptr<MessageReader>(new MessageReader(m_io, m_connection));
    m_writer = std::shared_ptr<MessageWriter>(new MessageWriter(m_io, m_connection));
//    _writer->setWriteSock(cptr);
    
//    _requestHandlerPtr  = new TRequestHandler(_io);

//    _requestHandlerUnPtr = std::unique_ptr<RequestHandlerBase>(_requestHandlerPtr);

    auto rmh = std::bind(&ConnectionHandler::p_read_message_handler, this, std::placeholders::_1 );
    m_reader->readMessage(rmh);
}
