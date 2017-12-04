#ifndef roundtrip_hpp
#define roundtrip_hpp

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


std::shared_ptr<Client> do_client_connect(std::string code, boost::asio::io_service& io);
std::shared_ptr<Client> one_roundtrip(std::string code, boost::asio::io_service& io);

#endif
