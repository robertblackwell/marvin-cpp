#ifndef multiple_hpp
#define multiple_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "boost_stuff.hpp"

#include "rb_logger.hpp"
#include "http_header.hpp"
#include "client.hpp"


/**
* Perform a single get request
*/
std::shared_ptr<Client> do_get_request(std::string code, boost::asio::io_service& io);
#endif
