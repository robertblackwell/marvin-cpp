//
//  ConnectionManager.hpp
//  asio-mitm
//
//  Created by ROBERT BLACKWELL on 11/20/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef client_connection_manager_hpp
#define client_connection_manager_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "callback_typedefs.hpp"

class Connection;

#include <stdio.h>

class ClientConnectionManager
{
public:
    static ClientConnectionManager* getInstance(boost::asio::io_service& io);
           
    ClientConnectionManager(boost::asio::io_service& io);
    
    void init(boost::asio::io_service& io);
    
    void asyncGetClientConnection(
        std::string scheme,
        std::string server,
        std::string port,
        ConnectCallbackType cb
    );
    
    void releaseClientConnection(ClientConnection* conn);

private:

    void postSuccess(ConnectCallbackType cb, ClientConnection* conn);
    void postFail(ConnectCallbackType cb, Marvin::ErrorType& ec);
    
    boost::asio::io_service&        io;
    boost::asio::ip::tcp::resolver  resolver_;

};

#endif /* ConnectionManager_hpp */
