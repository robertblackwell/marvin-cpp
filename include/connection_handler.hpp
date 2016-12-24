//
//  ConnectionHandler.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/12/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef CONNECTION_HANDLER_HPP
#define CONNECTION_HANDLER_HPP

#include <stdio.h>
#include <boost/asio.hpp>

#include "message_reader.hpp"
#include "message_writer.hpp"
#include "connection_interface.hpp"
#include "server_connection_manager.hpp"


//
// If these are defined we use unique_ptr to hold Connection, MessageReader, MessageWriter
//
#define CH_SMARTPOINTER
#define CON_SMARTPOINTER
/// TRequestHandler must conform to RequestHandlerInterface
template<class TRequestHandler> class ConnectionHandler
{
    public:
        ConnectionHandler(
            boost::asio::io_service&                        io,
            ServerConnectionManager<ConnectionHandler<TRequestHandler>>&     connectionManager,
            ConnectionInterface*                                     conn
        );
    
        ~ConnectionHandler();
    
        void serve();
        void close();
        int nativeSocketFD();
    private:
    
        void readMessageHandler(Marvin::ErrorType& err);
        void requestComplete();
        void handlerComplete();
        void handleConnectComplete(bool hijack);

    
        boost::asio::io_service&                            _io;
        ConnectionInterface*                                _conn;
        ServerConnectionManager<ConnectionHandler>&         _connectionManager;
        TRequestHandler*                                    _requestHandlerPtr;
        std::unique_ptr<TRequestHandler>                    _requestHandlerUnPtr;
    
#ifdef CON_SMARTPOINTER
        ConnectionPtr                                       _connection;
//        std::unique_ptr<Connection>         _connection;
#else
        ConnectionInterface*                         _connection;
#endif

#ifdef CH_SMARTPOINTER
        std::unique_ptr<MessageReader>      _reader;
        std::unique_ptr<MessageWriter>      _writer;
#else
        MessageReader*                      _reader;
        MessageWriter*                      _writer;
#endif
};

#include "connection_handler.ipp"

#endif /* ConnectionHandler_hpp */
