#ifndef tsc_post_hpp
#define tsc_post_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <gtest/gtest.h>
#include "rb_logger.hpp"
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
            boost::asio::io_service&    _io;
            MessageBaseSPtr             _msg;
            ClientSPtr                  _client;
            Testcase                    _testcase;
    };
}

#endif /* test_client_h */
