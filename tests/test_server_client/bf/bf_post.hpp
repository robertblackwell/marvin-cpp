#ifndef tsc_post_hpp
#define tsc_post_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include "boost_stuff.hpp"
#include <catch/catch.hpp>
#include "rb_logger.hpp"
#include "marvin_uri.hpp"
#include "buffer.hpp"
#include "client.hpp"
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
            boost::asio::io_service&    m_io;
            MessageBaseSPtr             m_msg;
            Marvin::UriSPtr             m_uri_sptr;
            ClientSPtr                  m_client;
            Testcase                    m_testcase;
    };
}

#endif /* test_client_h */
