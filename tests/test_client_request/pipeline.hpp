
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
#include <catch2/catch.hpp>

#include <marvin/include/boost_stuff.hpp>

#include <marvin/external_src/rb_logger.hpp>
#include <marvin/http/http_header.hpp>
#include <marvin/client/client.hpp>


/**
* The Pipeline class is the mechanism for managing mutipe async operations.
*/
class Pipeline
{
    public:
    Pipeline(boost::asio::io_service& io, Marvin::Uri& uri);
    void setup();
    void handler(Marvin::ErrorType err, MessageReaderSPtr rdr);

    int                             m_counter;
    int                             m_max_counter;
    boost::asio::io_service&        m_io;
    std::shared_ptr<Client>         m_client_sptr;
    Marvin::Http::MessageBaseSPtr   m_msg_sptr;
    Marvin::Uri                     m_uri;
    std::string                     m_request_url;

};

#endif
