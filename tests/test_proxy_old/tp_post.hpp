#ifndef proxy_post_hpp
#define proxy_post_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <marvin/include/boost_stuff.hpp>
#include <catch2/catch.hpp>
#include <marvin/external_src/rb_logger.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/client/client.hpp>
#include "tp_testcase.hpp"

namespace tp {
    /**
    * This function runs a post request based on a testcase and checks the response is as expected.
    */
    std::shared_ptr<Client> post_body_testcase(boost::asio::io_service& io, tp::TestcaseSPtr testcaseSPtr );
    class PostTest
    {
        public:
            PostTest(boost::asio::io_service& io, TestcaseSPtr testcaseSPtr);
            void handler(Marvin::ErrorType& er, MessageReaderSPtr rdr);
            void exec();
        protected:
            boost::asio::io_service&        m_io;
            Marvin::Http::MessageBaseSPtr   m_msg_sptr;
            ClientSPtr                      m_client_sptr;
            TestcaseSPtr                    m_testcase_sptr;
            std::string                     m_scheme;
            std::string                     m_proxy_host;
            std::string                     m_proxy_port;
    };
}

#endif /* test_client_h */
