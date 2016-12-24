class Socket {
public:
    virtual boost::asio::ip::tcp::socket &getSocketForAsio() = 0;

    static Socket* create(boost::asio::io_service& iIoService, boost::asio::ssl::context *ipSslContext) {
        // Obviously this has to be in a separate source file since it makes reference to subclasses
        if (ipSslContext == nullptr) {
            return new NonSslSocket(iIoService);
        }
       return new SslSocket(iIoService, *ipSslContext);
    }

    size_t _read(void *ipData, size_t iLength) {
        return boost::asio::read(getSocketForAsio(), boost::asio::buffer(ipData, iLength));
    }
    size_t _write(const void *ipData, size_t iLength) {
        return boost::asio::write(getSocketForAsio(), boost::asio::buffer(ipData, iLength));
    }
};

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SslSocket_t;
class SslSocket: public Socket, private SslSocket_t {
public:
    SslSocket(boost::asio::io_service& iIoService, boost::asio::ssl::context &iSslContext) :
        SslSocket_t(iIoService, iSslContext) {
    }

private:
    boost::asio::ip::tcp::socket &getSocketForAsio() {
        return next_layer();
    }
};

class NonSslSocket: public Socket, private Socket_t {
public:
    NonSslSocket(boost::asio::io_service& iIoService) :
            Socket_t(iIoService) {
    }

private:
    boost::asio::ip::tcp::socket &getSocketForAsio() {
        return next_layer();
    }
};

template <typename SocketType>
void doStuffWithOpenSocket(SocketType socket) {
   boost::asio::write(socket, ...);
   boost::asio::read(socket, ...);
   boost::asio::read_until(socket, ...);
   // etc...
}

boost::asio::ip::tcp::socket socket_;
// socket_ opened normally ...
doStuffWithOpenSocket<boost::asio::ip::tcp::socket>(socket_); // works!

boost::asio::ssl::stream<boost::asio::ip::tcp::socket> secureSocket_;
// secureSocket_ opened normally (including handshake) ...
doStuffWithOpenSocket(secureSocket_); // also works, with (different) implicit instantiation!
// shutdown the ssl socket when done ...