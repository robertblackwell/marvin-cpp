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
#include "request_handler.hpp"
#include "server_connection_manager.hpp"

class ServerConnectionManager;

//
// If these are defined we use unique_ptr to hold Connection, MessageReader, MessageWriter
//
#define CH_SMARTPOINTER
#define CON_SMARTPOINTER

class ConnectionHandler
{
    public:
        ConnectionHandler(
            boost::asio::io_service&    io,
            ServerConnectionManager&    connectionManager,
            RequestHandlerInterface&    requestHandler,
            Connection*                 conn
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

    
        boost::asio::io_service&         _io;
        Connection*                     _conn;
        ServerConnectionManager&        _connectionManager;
        RequestHandlerInterface&        _requestHandler;
#ifdef CON_SMARTPOINTER
        ConnectionPtr                   _connection;
//        std::unique_ptr<Connection>         _connection;
#else
        Connection*                         _connection;
#endif

#ifdef CH_SMARTPOINTER
        std::unique_ptr<MessageReader>      _reader;
        std::unique_ptr<MessageWriter>      _writer;
#else
        MessageReader*                      _reader;
        MessageWriter*                      _writer;
#endif
};
#endif /* ConnectionHandler_hpp */
