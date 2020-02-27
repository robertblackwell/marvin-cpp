#ifndef test_message_client_hpp
#define test_message_client_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <marvin/include/boost_stuff.hpp>
#include <marvin/external_src/rb_logger.hpp>
#include "error.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include <marvin/connection/tcp_connection.hpp>
#include <marvin/message/message_reader.hpp>

class TClient;
typedef std:: shared_ptr<TClient> TClientSPtr;
typedef std:: unique_ptr<TClient> TClientUPtr;

typedef std::function<void(boost::system::error_code err)> SysErrorCb;
typedef std::function<void(Marvin::ErrorType err)> MarvinErrorCb;

class TClient
{
    public:
        TClient(boost::asio::io_service& io, std::string scheme, std::string server, std::string port, Testcase tc);
        /**
        * send all test case buffers
        */
        void send_testcase_buffers(SysErrorCb cb);
    
        /**
        * Establish connection
        */
        void connect();
        /**
        * Start the writing on one buffer
        */
        void write_line();
        /**
        * Handle completion of writing one buffer, either start the next buffer or end
        */
        void handle_write_complete(Marvin::ErrorType& err, std::size_t bytes_transfered);
        /**
        * Puts a delay before each buffer write
        */
        void wait_before_write();
    
        boost::asio::io_service&    m_io;
        std::string                 m_scheme;
        std::string                 m_server;
        std::string                 m_port;
        Testcase                    m_testcase;
//        TCPConnectionSPtr           m_conn_sptr;
        ISocketSPtr                 m_conn_sptr;
        int                         m_buffer_index; /// index into the vector of buffers in the test case
        MarvinErrorCb               m_test_cb;
        boost::asio::deadline_timer m_timer;

};
#endif

