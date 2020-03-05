#ifndef test_message_client_hpp
#define test_message_client_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <marvin/boost_stuff.hpp>
#include <pthread.h>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include "error.hpp"
#include "repeating_timer.hpp"
#include <marvin/connection/connection.hpp>
#include <marvin/message/message_reader.hpp>
#include "bb_testcase.hpp"

namespace body_buffering {

    /**
    * Sends a message to a test server and validates the response as directed by
    * body_buffering::Testcase instance
    */
    class TClient;
    typedef std:: shared_ptr<TClient> TClientSPtr;
    typedef std:: unique_ptr<TClient> TClientUPtr;

    typedef std::function<void(boost::system::error_code err)> SysErrorCb;
    typedef std::function<void(Marvin::ErrorType err)> MarvinErrorCb;
    /**
    * This is a client that sends a request as a raw set of buffers without
    * any checking on the method, headers, content length etc. Caller musrt ensure
    * all that stuff.
    * It reads the response as a http message and validates the response
    * based on properties in the Testcase object
    */
    class TClient
    {
        public:
            TClient(boost::asio::io_service& io, std::string port, Testcase tc);

            void exec();
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
        
            void read_message();
            void read_msg_complete_handler();
            void onMessage(Marvin::ErrorType er);
        
            #if 0
            void onBody(Marvin::ErrorType er, BufferChain chunk);
            void onHeaders(Marvin::ErrorType er);
            #endif
            /**
            * Puts a delay before each buffer write
            */
            void wait_before_write();
        
            boost::asio::io_service&    m_io;
            std::string                 m_scheme;
            std::string                 m_server;
            std::string                 m_port;
            Testcase                    m_testcase;
            ISocketSPtr                  m_conn_sptr;
            MessageReaderSPtr           m_rdr;
            int                         m_buffer_index; /// index into the vector of buffers in the test case
            MarvinErrorCb               m_test_cb;
            boost::asio::deadline_timer m_timer;

    };
}
#endif

