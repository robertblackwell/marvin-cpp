#ifndef roundtrip_hpp
#define roundtrip_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include <marvin/boost_stuff.hpp>

#include <marvin/external_src/trog/trog.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/client/client.hpp>
#include <marvin/client/request.hpp>


std::shared_ptr<Client> do_client_connect(std::string code, boost::asio::io_service& io);
std::shared_ptr<Client> one_roundtrip(std::string code, boost::asio::io_service& io);

#endif
