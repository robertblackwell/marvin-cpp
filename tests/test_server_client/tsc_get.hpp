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
#include "bufferV2.hpp"
#include "client.hpp"
#include "tsc_testcase.hpp"

/**
* This function runs a post request based on a testcase and checks the response is as expected
*/
std::shared_ptr<Client> get_testcase(EchoTestcase& testcase, boost::asio::io_service& io);
std::shared_ptr<Client> get_testcase(std::string code, boost::asio::io_service& io);

#endif /* test_client_h */

