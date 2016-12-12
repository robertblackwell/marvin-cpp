
#ifndef HTTP_SERVER_CONNECTION_HPP
#define HTTP_SERVER_CONNECTION_HPP

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

class ServerConnectionManager;

/// Represents a single ServerConnection from a client.
class ServerConnection
  : public std::enable_shared_from_this<ServerConnection>
{
public:
  ServerConnection(const ServerConnection&) = delete;
  ServerConnection& operator=(const ServerConnection&) = delete;

  /// Construct a ServerConnection with the given socket.
    explicit ServerConnection(
                        std::shared_ptr<boost::asio::ip::tcp::socket> socket_sptr,
                        ServerConnectionManager& manager,
                        RequestHandlerInterface& handler);

    /// Start the first asynchronous operation for the ServerConnection.
    void start();

    /// Stop all asynchronous operations associated with the ServerConnection.
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

    /// Socket for the ServerConnection.
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_sptr_;

    /// The manager for this ServerConnection.
    ServerConnectionManager& connection_manager_;

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

typedef std::shared_ptr<ServerConnection> connection_ptr;


#endif // HTTP_CONNECTION_HPP
