#ifndef marvin_i_collector_hpp
#define marvin_i_collector_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <marvin/include/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>

#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/client/request.hpp>

class ICollector
{
    public:
        /**
        ** Interface method for client code to call collect
        **/
        virtual void collect(std::string scheme, std::string host, MessageReaderSPtr req, Marvin::Http::MessageBaseSPtr resp) = 0;
    
    private:
};

#endif
