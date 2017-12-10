#ifndef test_message_server_hpp
#define Test_message_server_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>
#include "rb_logger.hpp"
#include "error.hpp"
#include "repeating_timer.hpp"
//#include "http_server.hpp"
//#include "request_handler_base.hpp"
//#include "request.hpp"
//#include "tsc_client.hpp"
//#include "tsc_server.hpp"
#include "bb_testcase.hpp"
#include "test_runner.hpp"
#include "tcp_connection.hpp"
#include "message_reader_v2.hpp"

class TServer;
typedef std:: shared_ptr<TServer> TServerSPtr;
typedef std:: unique_ptr<TServer> TServerUPtr;

class TServer
{
    public:
        explicit TServer(boost::asio::io_service& io, Testcase tc);
        ~TServer();
        void listen(long port, std::function<void(MessageReaderV2SPtr rdr)> cb);
    
    protected:
        void initialize();
        void startAccept();
        void handleAccept(const boost::system::error_code& err);
        void read_full_message_test();
        void read_headers_body_test();

        long                                            _port;
        boost::asio::io_service&                         _io;
        boost::asio::ip::tcp::acceptor                  _acceptor;
        TCPConnectionSPtr                               _conn_sptr;
        MessageReaderV2SPtr                             _rdr;
        Testcase                                        _tc;
        TestrunnerSPtr                                  _runner_sptr;
};

#endif
