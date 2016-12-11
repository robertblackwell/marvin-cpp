
#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

class ConnectionManager;

/// Represents a single connection from a client.
class Connection
  : public std::enable_shared_from_this<Connection>
{
public:
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  /// Construct a connection with the given socket.
    explicit Connection(
                        std::shared_ptr<boost::asio::ip::tcp::socket> socket_sptr,
                        ConnectionManager& manager,
                        RequestHandlerInterface& handler);

    /// Start the first asynchronous operation for the connection.
    void start();

    /// Stop all asynchronous operations associated with the connection.
    void stop();

private:
    
    /// Perform an asynchronous read operation and async handler operation.
    void startRead();
    void onGoodRequest();
    void onBadRequest();
    void onHandlerDone();
    void handleRead();

    /// Perform an asynchronous write operation.
    void startWrite();

    /// Socket for the connection.
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_sptr_;

    /// The manager for this connection.
    ConnectionManager& connection_manager_;

    /// The handler used to process the incoming request.
    RequestHandlerInterface& request_handler_;

    /// Buffer for incoming data.
    std::array<char, 8192> buffer_;

    /// The incoming request.
    request request_;

    /// The parser for the incoming request.
    request_parser request_parser_;

    /// The reply to be sent back to the client.
    reply reply_;
    
};

typedef std::shared_ptr<Connection> connection_ptr;


#endif // HTTP_CONNECTION_HPP
