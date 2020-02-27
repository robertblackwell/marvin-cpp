//
// The main entry point for Marvin - a mitm proxy for http/https 
//


#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>
#include <marvin/external_src/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/forwarding/forwarding_handler.hpp>
#include <marvin/collector/pipe_collector.hpp>

int main(int argc, const char * argv[])
{
    RBLogging::addTraceFile("http_server");
//    RBLogging::addTraceFile("connection_handler");
    RBLogging::addTraceFile("forwarding_handler");
    RBLogging::addTraceFile("server_connection_manager");
//    RBLogging::addTraceFile("half_tunnel");
//    RBLogging::addTraceFile("tunnel_handler");
    RBLogging::addTraceFile("main");
    LogError("This is an error", std::string(""));
    LogTrace(std::string("hello"));
    LogTrace(std::string("hello"), std::string("this"), std::string("is"), std::string("a"), std::string("test"));
    RBLogging::enableForLevel(LOG_LEVEL_TRACE);

    try
    {
        PipeCollector::configSet_PipePath("/Users/robertblackwell/marvin_collector");

        std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
        std::vector<int> ports{443, 9443};
        ForwardingHandler::configSet_HttpsPorts(ports);
        ForwardingHandler::configSet_HttpsHosts(re);

        HTTPServer* server_ptr = new HTTPServer([](boost::asio::io_service& io){
            PipeCollector* pc = PipeCollector::getInstance(io);
            return new ForwardingHandler(io, pc);
        });

        server_ptr->listen(9991);
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    
    return 0;
}
