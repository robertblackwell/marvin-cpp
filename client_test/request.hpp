#ifndef request_hpp
#define request_hpp


#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "message_writer.hpp"
#include "message_reader.hpp"
#include "Connection.hpp"

using boost::asio::ip::tcp;

class Request : public MessageWriter
{
public:
    Request(boost::asio::io_service& io);
    ~Request();
    
    void go(std::function<void(Marvin::ErrorType& err)> cb);
    
    void asyncGetConnection(ConnectCallbackType connectCb);
    void asyncGetWriteSocket(ConnectCallbackType connectCb);
    void asyncGetReadSocket(ConnectCallbackType connectCb);
    
    MessageReader&  getResponse();
    
    void setUrl(std::string url);
    
private:
    void haveConnection(Marvin::ErrorType& err, Connection* conn);
    void fullWriteHandler(Marvin::ErrorType& err);
    
    boost::asio::io_service&                        _io;
    MessageReader*                                  _rdr;
    
//    WriteSocketInterface*                           _writeSock;
    ReadSocketInterface*                            _readSock;
//    Connection*                                     _connection;
    std::function<void(Marvin::ErrorType& err)>     _goCb;
};
#endif
