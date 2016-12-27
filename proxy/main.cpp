//
// The main entry point for Marvin - a mitm proxy for http/https 
//


#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "forwarding_handler.hpp"

int main(int argc, const char * argv[])
{
    LogError("This is an error", std::string(""));
    LogTrace(std::string("hello"));
    LogTrace(std::string("hello"), std::string("this"), std::string("is"), std::string("a"), std::string("test"));
    try
    {
        Server<ForwardingHandler> server(9991);
        server.listen();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    
    return 0;
}
