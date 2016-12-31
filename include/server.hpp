
#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <signal.h>
#include <utility>

#include "marvin_error.hpp"
#include "server_connection_manager.hpp"
#include "request_handler_base.hpp"
#include "connection_interface.hpp"
#include "http_connection.hpp"
#include "tls_connection.hpp"
#include "message_reader.hpp"
#include "message_writer.hpp"
#include "rb_logger.hpp"
#include "connection_handler.hpp"

/*
** @brief HTTP server class.
** @discussion TRequestHandler is a template argument that must conform to RequestHandlerBase
** for a class that will handle an http request and send the necessary response
*/
template<class TRequestHandler> class Server
{
public:

    static void configSet_NumberOfThreads(int num);

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    /**
    ** @brief Construct the server to listen on the specified TCP address and port.
    ** @param long port defaults to 9991
    */
    explicit Server();
    
    /**
    ** @brief starts the listen process on the servers port, and from there
    ** dispatches instances of TRequestHandler to service the connection
    */
    void listen(long port = 9991);
    
private:

    static int __numberOfThreads;

    /**
    ** @brief just as it says - init the server ready to list
    */
    void initialize();
    
    /**
    ** @brief Initiates an asynchronous accept operation.
    */
    void startAccept();
    
    /**
    ** @brief callback that is invoked on completio of an accept call
    ** @param handler an object responsible for managing the new connection established by the
    **          completed accept call.
    ** @param err a boost errorcide that described any error condition
    */
    void handleAccept(ConnectionHandler<TRequestHandler>* handler, const boost::system::error_code& err);

    /**
    ** @brief encapsulates the process of posting a callback fn to the servcers strand
    */
    void postOnStrand(std::function<void()> fn);
    
    /**
    ** @brief sets up a signal callback
    */
    void waitForStop();
    
    /**
    ** @brief IS the signal callback
    */
    void doStop(const Marvin::ErrorType& err);
    
    int                                             _numberOfThreads;
    long                                            _port;
    boost::asio::io_service                         _io;
    boost::asio::strand                             _serverStrand;
    boost::asio::signal_set                         _signals;
    boost::asio::ip::tcp::acceptor                  _acceptor;
    ServerConnectionManager<ConnectionHandler<TRequestHandler>>   _connectionManager;

};
template <class TRequestHandler>
using Server_Template =  typename Server<TRequestHandler>::Server;

#include "server.ipp"
#endif // HTTP_SERVER_HPP
