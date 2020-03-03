//
//  ConnectionHandler.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/12/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef marvin_connection_handler_hpp
#define marvin_connection_handler_hpp
/// \ingroup Server
#include <stdio.h>

#include<marvin/boost_stuff.hpp>
#include <marvin/server/server_context.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/server/connection_handler.hpp>
#include <marvin/connection/socket_interface.hpp>

class ServerConnectionManager;
class ConnectionHandler;
/// \ingroup Server
using ConnectionHandlerSPtr = std::shared_ptr<ConnectionHandler>;

/// \ingroup Server
/// \brief An instance of this class is created by the server foor every open client connection;
/// this instance manages the life time and invocation of the request handler that actually services the
/// client request.
class ConnectionHandler
{
    public:
        ConnectionHandler(
            boost::asio::io_service&     io,
            ServerConnectionManager&     connectionManager,
            ISocket*                     conn, // we know for certain that this is a TCPConnection ??
            RequestHandlerFactory        factory
        );
    
        ~ConnectionHandler();
    
        void serve();
        long nativeSocketFD();
        std::string uuid();
    private:
    
        void p_serve_another();
        void p_read_message_handler(Marvin::ErrorType err);
        void p_request_complete(Marvin::ErrorType err, bool keepAlive);
        void p_handler_complete(Marvin::ErrorType err);
        void p_handle_connect_complete(bool hijack);
        
        boost::uuids::uuid                     m_uuid;
        boost::asio::io_service&               m_io;
        ServerConnectionManager&               m_connectionManager;
        std::unique_ptr<RequestHandlerBase>    m_requestHandlerUnPtr;
        RequestHandlerFactory                  m_factory;
    
        ISocketSPtr                            m_connection;
        MessageReaderSPtr                      m_reader;
        MessageWriterSPtr                      m_writer;
        ServerContext                          m_server_context;
};

#endif /* ConnectionHandler_hpp */
