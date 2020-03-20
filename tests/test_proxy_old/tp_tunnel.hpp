#ifndef tsc_tunnel_hpp
#define tsc_tunnel_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <marvin/boost_stuff.hpp>
#include <doctest/doctest.h>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/client/client.hpp>
#include "tp_testcase.hpp"

namespace tp {
    /**
    * This class performs a http (not https) request via a tunnel through a proxy.
    * the request and proxy details are provided in the testCaseSPtr provided to the consctutor,
    * the exec method starts the entire process.
    */
//    std::shared_ptr<Client> post_body_testcase(boost::asio::io_service& io, tp::TestcaseSPtr testcaseSPtr );
    class Tunnel
    {
        public:
            Tunnel(boost::asio::io_service& io, TestcaseSPtr testcaseSPtr);
            void handler(Marvin::ErrorType& er, MessageReaderSPtr rdr);
            void exec();
        protected:
            void p_send_conect();
            void p_connect_handler(Marvin::ErrorType& er, MessageReaderSPtr rdr);
            void p_send_request();
            boost::asio::io_service&        m_io;
            
            Marvin::Http::MessageBaseSPtr   m_connect_msg_sptr;
            Marvin::Http::MessageBaseSPtr   m_msg_sptr;
            ClientSPtr                      m_client_sptr;
            TestcaseSPtr                    m_testcase_sptr;
            std::string                     m_scheme;
            std::string                     m_proxy_server;
            std::string                     m_proxy_port;
    };
}

#endif /* test_client_h */
