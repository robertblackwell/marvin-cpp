#ifndef multiple_hpp
#define multiple_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include <marvin/boost_stuff.hpp>

#include <marvin/configure_trog.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/client/client.hpp>


/**
* Perform a single get request
*/
std::shared_ptr<Client> do_get_request(std::string code, boost::asio::io_service& io);
#endif
