#ifndef tsc_post_hpp
#define tsc_post_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <marvin/include/boost_stuff.hpp>
#include <catch2/catch.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <marvin/message/marvin_uri.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/client/client.hpp>
#include "bf_testcase.hpp"

namespace body_format {
    /**
    * This function runs a post request based on a testcase and checks the response is as expected.
    */
    std::shared_ptr<Client> post_body_testcase(body_format::Testcase testcase, boost::asio::io_service& io);
    class PostTest
    {
        public:
            PostTest(boost::asio::io_service& io, Testcase testcase);
            void handler(Marvin::ErrorType& er, MessageReaderSPtr rdr);
            void exec();
        protected:
            boost::asio::io_service&        m_io;
            Marvin::Http::MessageBaseSPtr   m_msg;
            Marvin::UriSPtr                 m_uri_sptr;
            ClientSPtr                      m_client;
            Testcase                        m_testcase;
    };
}

#endif /* test_client_h */
