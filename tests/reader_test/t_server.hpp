#ifndef test_message_server_hpp
#define test_message_server_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include<marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include "error.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include "test_runner.hpp"
#include <marvin/message/message_reader.hpp>

class TServer;
typedef std:: shared_ptr<TServer> TServerSPtr;
typedef std:: unique_ptr<TServer> TServerUPtr;

class TServer
{
    public:
        explicit TServer(boost::asio::io_service& io, Testcase tc);
        ~TServer();
        void listen(long port, std::function<void(MessageReaderSPtr rdr)> cb);
    
    protected:
        void initialize();
        void startAccept();
        void handleAccept(const boost::system::error_code& err);
        void read_full_message_test();
        void read_headers_body_test();

        long                                            m_port;
        boost::asio::io_service&                        m_io;
        boost::asio::ip::tcp::acceptor                  m_acceptor;
        ISocketSPtr                                     m_conn_sptr;
        MessageReaderSPtr                               m_rdr;
        Testcase                                        m_tc;
        TestrunnerSPtr                                  m_runner_sptr;
};

#endif
