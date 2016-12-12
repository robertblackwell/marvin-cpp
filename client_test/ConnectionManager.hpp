//
//  ConnectionManager.hpp
//  asio-mitm
//
//  Created by ROBERT BLACKWELL on 11/20/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef ConnectionManager_hpp
#define ConnectionManager_hpp

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
//typedef boost::function<void (Connection*)>;

typedef boost::function<void (Connection*)> connectionCb_t;

class ConnectionManager
{
public:
    static ConnectionManager* getInstance(boost::asio::io_service& io);
           
    ConnectionManager(boost::asio::io_service& io);
    
    void init(boost::asio::io_service& io);
    
    void asyncGetConnection(
        std::string scheme,
        std::string server,
        std::string port,
        ConnectCallbackType cb
    );
    
    void releaseConnection(Connection* conn);

private:

    void postSuccess(ConnectCallbackType cb, Connection* conn);    
    void postFail(ConnectCallbackType cb, Marvin::ErrorType& ec);
    
    boost::asio::io_service&        io;
    boost::asio::ip::tcp::resolver  resolver_;

};

#endif /* ConnectionManager_hpp */
