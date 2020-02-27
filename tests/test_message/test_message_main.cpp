//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <marvin/external_src/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

int main( int argc, char* argv[] )
{
    RBLogging::setEnabled(false);
    printf("%s\n", __FILE__);
    
    char* _argv[2] = {argv[0], (char*)"--catch_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    int result = Catch::Session().run( argc, argv );
    
    printf("%s\n", __FILE__);
    return result;
}

