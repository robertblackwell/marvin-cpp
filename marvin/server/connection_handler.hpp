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

#include "message_reader_v2.hpp"
#include "message_writer_v2.hpp"
#include "request_handler_base.hpp"
#include "connection_handler.hpp"
#include "connection_interface.hpp"

class ServerConnectionManager;
class ConnectionHandler;
typedef std::shared_ptr<ConnectionHandler> ConnectionHandlerSPtr;

class ConnectionHandler
{
    public:
        ConnectionHandler(
            boost::asio::io_service&                                        io,
            ServerConnectionManager&                                        connectionManager,
            ConnectionInterface*                                            conn,
            RequestHandlerFactory                                           factory
        );
    
        ~ConnectionHandler();
    
        void serve();
        void close();
        long nativeSocketFD();
    private:
    
        void serveAnother();
        void readMessageHandler(Marvin::ErrorType err);
        void requestComplete(Marvin::ErrorType err, bool keepAlive);
        void handlerComplete(Marvin::ErrorType err);
        void handleConnectComplete(bool hijack);

        boost::uuids::uuid                                  _uuid;
        boost::asio::io_service&                            _io;
        ServerConnectionManager&                            _connectionManager;
//        RequestHandlerBase*                                 _requestHandlerPtr;
        std::unique_ptr<RequestHandlerBase>                 _requestHandlerUnPtr;
        RequestHandlerFactory                               _factory;
    
        ConnectionInterfaceSPtr                             _connection;
        MessageReaderV2SPtr                                 _reader;
        MessageWriterV2SPtr                                 _writer;
};

#endif /* ConnectionHandler_hpp */
