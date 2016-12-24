
#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <signal.h>
#include <utility>

#include "marvin_error.hpp"
#include "server_connection_manager.hpp"
#include "request_handler_interface.hpp"
#include "connection_interface.hpp"
#include "http_connection.hpp"
#include "tls_connection.hpp"
#include "message_reader.hpp"
#include "message_writer.hpp"
#include "rb_logger.hpp"
#include "connection_handler.hpp"

/// The top-level class of the HTTP server.

/// TRequestHandler must conform to RequestHandlerInterface
template<class TRequestHandler> class Server
{
public:
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit Server();

    void listen();
    
private:
    /// Perform an asynchronous accept operation.
    void startAccept();
    void handleAccept(ConnectionHandler<TRequestHandler>* handler, const boost::system::error_code& err);
    void readMessageHandler(Marvin::ErrorType& err);
    
    void postOnStrand(std::function<void()> fn);
    void waitForStop();
    void doStop(const Marvin::ErrorType& err);

    boost::asio::io_service                         _io;
    boost::asio::strand                             _serverStrand;
    boost::asio::signal_set                         _signals;
    boost::asio::ip::tcp::acceptor                  _acceptor;
    ServerConnectionManager<ConnectionHandler<TRequestHandler>>   _connectionManager;

};
#include "server.ipp"
#endif // HTTP_SERVER_HPP
