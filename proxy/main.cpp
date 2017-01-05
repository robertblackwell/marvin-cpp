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

#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "forwarding_handlerV2.hpp"
#include "pipe_collector.hpp"

int main(int argc, const char * argv[])
{
    LogError("This is an error", std::string(""));
    LogTrace(std::string("hello"));
    LogTrace(std::string("hello"), std::string("this"), std::string("is"), std::string("a"), std::string("test"));
    try
    {
        PipeCollector::configSet_PipePath("/Users/rob/marvin_collect");
        
        std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
        std::vector<int> ports{443, 9443};
        ForwardingHandlerV2<PipeCollector>::configSet_HttpsPorts(ports);
        ForwardingHandlerV2<PipeCollector>::configSet_HttpsHosts(re);
        
        HTTPServer<ForwardingHandlerV2<PipeCollector>> server;
        server.listen(9991);
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    
    return 0;
}
