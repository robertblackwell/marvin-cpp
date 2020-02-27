#ifndef multiple_hpp
#define multiple_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <catch2/catch.hpp>

#include <marvin/include/boost_stuff.hpp>

#include <marvin/external_src/rb_logger.hpp>
#include <marvin/http/http_header.hpp>
#include <marvin/client/client.hpp>


/**
* Perform a single get request
*/
std::shared_ptr<Client> do_get_request(std::string code, boost::asio::io_service& io);
#endif
