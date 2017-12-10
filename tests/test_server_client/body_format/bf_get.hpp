#ifndef tsc_get_hpp
#define tsc_get_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "rb_logger.hpp"
#include "buffer.hpp"
#include "client.hpp"
#include "bf_testcase.hpp"

namespace body_format {
    /**
    * This function runs a post request based on a testcase and checks the response is as expected
    */
    std::shared_ptr<Client> get_testcase(Testcase& testcase, boost::asio::io_service& io);
    std::shared_ptr<Client> get_testcase(std::string code, boost::asio::io_service& io);

    class GetTest
    {
        public:
            GetTest(boost::asio::io_service& io, Testcase& testcase);
            void exec();
        protected:
            boost::asio::io_service&    _io;
            MessageBaseSPtr             _msg;
            ClientSPtr                  _client;
            Testcase&                   _testcase;
    };
}
#endif /* test_client_h */

