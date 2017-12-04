#ifndef request_hpp
#define request_hpp


#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "message_writer_v2.hpp"
#include "message_reader_v2.hpp"
#include "connection_interface.hpp"
#include "url.hpp"

using boost::asio::ip::tcp;
class Request;
typedef std::shared_ptr<Request> RequestSPtr;
typedef std::unique_ptr<Request> RequestUPtr;

class Request : public MessageWriterV2
{
public:
    Request(boost::asio::io_service& io);
    
    Request(const Request& other) = delete;
    Request& operator=(const Request&) = delete;
    
    ~Request();
    
    void go(std::function<void(Marvin::ErrorType& err)> cb);
    // void goHeaders() - coming to a universe near you
    
    MessageReaderV2&  getResponse();
    
    void setUrl(std::string url);
    
    void end();
    
    friend std::string traceRequest(Request& request);
    
protected:
    void asyncGetWriteSocket(ConnectCallbackType connectCb);
    void haveConnection(Marvin::ErrorType& err, ConnectionInterface* conn);
    void fullWriteHandler(Marvin::ErrorType& err);
    void readComplete(Marvin::ErrorType& err);
    void defaultHeaders();

    boost::asio::io_service&                        _io;
    MessageReaderV2SPtr                             _rdr;
    
    ConnectionInterfaceSPtr                         _connection;
    ReadSocketInterfaceSPtr                         _readSock;
    
    std::function<void(Marvin::ErrorType& err)>     _goCb;
    
    bool        _oneTripOnly;
    
    std::string _service;   //used by boost for resolve and connnect http/https or a port number
    std::string _server;    // as used in boost resolve/connect WITHOUT port number
    
    std::string _scheme;    // http or https
    std::string _host;      // as used in the headers localhost:9991 for example
    std::string _port;
    std::string _path;
    Url::Query  _query;
    std::string _queryStr;
};
#endif
