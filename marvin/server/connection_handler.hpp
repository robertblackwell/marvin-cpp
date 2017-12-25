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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "server_context.hpp"
#include "message_reader.hpp"
#include "message_writer.hpp"
#include "request_handler_base.hpp"
#include "connection_handler.hpp"
#include "i_socket.hpp"

class ServerConnectionManager;
class ConnectionHandler;
typedef std::shared_ptr<ConnectionHandler> ConnectionHandlerSPtr;

class ConnectionHandler
{
    public:
        ConnectionHandler(
            boost::asio::io_service&                                        io,
            ServerConnectionManager&                                        connectionManager,
            ISocket*                                            conn,
            RequestHandlerFactory                                           factory
        );
    
        ~ConnectionHandler();
    
        void serve();
        long nativeSocketFD();
        std::string uuid();
    private:
    
        void serveAnother();
        void readMessageHandler(Marvin::ErrorType err);
        void requestComplete(Marvin::ErrorType err, bool keepAlive);
        void handlerComplete(Marvin::ErrorType err);
        void handleConnectComplete(bool hijack);
        boost::uuids::uuid                     _uuid;
        boost::asio::io_service&               _io;
        ServerConnectionManager&               _connectionManager;
        std::unique_ptr<RequestHandlerBase>    _requestHandlerUnPtr;
        RequestHandlerFactory                  _factory;
    
        ISocketSPtr                _connection;
        MessageReaderSPtr                                 _reader;
        MessageWriterSPtr                                 _writer;
        ServerContext                                     _server_context;
};

#endif /* ConnectionHandler_hpp */
