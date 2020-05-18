
#include <marvin/connection/socket_interface.hpp>
#include <marvin/connection/connection.hpp>
namespace Marvin {
ISocketSPtr socket_factory(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
){
    ISocketSPtr ptr;
    ptr = std::make_shared<Marvin::Connection>(io_service, scheme, server, port);
    return ptr;
}
ISocketSPtr socket_factory(boost::asio::io_service& io_service)
{
    ISocketSPtr ptr;
    ptr = std::make_shared<Connection>(io_service);
    return ptr;
}
} // namespace
/*
using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;

// Create a context that uses the default paths for
// finding CA certificates.
ssl::context ctx(ssl::context::sslv23);
ctx.set_default_verify_paths();

// Open a socket and connect it to the remote host.
boost::asio::io_context io_context;
ssl_socket sock(io_context, ctx);
tcp::resolver resolver(io_context);
tcp::resolver::query query("host.name", "https");
boost::asio::connect(sock.lowest_layer(), resolver.resolve(query));
sock.lowest_layer().set_option(tcp::no_delay(true));

// Perform SSL handshake and verify the remote host's
// certificate.
sock.set_verify_mode(ssl::verify_peer);
sock.set_verify_callback(ssl::rfc2818_verification("host.name"));
sock.handshake(ssl_socket::client);
*/
//ISocket::~ISocket(){}
