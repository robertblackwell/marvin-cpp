#ifndef marvin_i_collector_hpp
#define marvin_i_collector_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "boost_stuff.hpp"
#include "rb_logger.hpp"

#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"

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
