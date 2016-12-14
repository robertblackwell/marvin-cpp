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
#include "client_connection.hpp"

using boost::asio::ip::tcp;

class Request : public MessageWriter
{
public:
    Request(boost::asio::io_service& io);
    
    Request(const Request& other) = delete;
    Request& operator=(const Request&) = delete;
    
    ~Request();
    
    void go(std::function<void(Marvin::ErrorType& err)> cb);
    // void goHeaders() - coming to a universe near you
    
    MessageReader&  getResponse();
    
    void setUrl(std::string url);
    
private:
    void asyncGetWriteSocket(ConnectCallbackType connectCb);
    void haveConnection(Marvin::ErrorType& err, ClientConnection* conn);
    void fullWriteHandler(Marvin::ErrorType& err);
    
    boost::asio::io_service&                        _io;
    std::shared_ptr<MessageReader>                  _rdr;
    
    ClientConnection*                               _connection;
    ReadSocketInterface*                            _readSock;
    
    std::function<void(Marvin::ErrorType& err)>     _goCb;
    
    std::string _service;   //used by boost for resolve and connnect http/https or a port number
    std::string _server;    // as used in boost resolve/connect WITHOUT port number
    
    std::string _scheme;    // http or https
    std::string _host;      // as used in the headers localhost:9991 for example
    std::string _port;
    std::string _path;
    std::string _query;
};
#endif
