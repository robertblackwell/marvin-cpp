
/**
* this file tests a single client performing multiple consecutive requests
* class implements multiple consecutive requests through the same client
*/
#ifndef pipeline_hpp
#define pipeline_hpp

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
* The Pipeline class is the mechanism for managing mutipe async operations.
*/
class Pipeline
{
    public:
    int _counter;
    boost::asio::io_service& _io;
    std::shared_ptr<Client> _client_sptr;
    MessageBaseSPtr _msg_sptr;
    Pipeline(boost::asio::io_service& io);
    void setup();
    void handler(Marvin::ErrorType err, MessageReaderSPtr rdr);
};

#endif
