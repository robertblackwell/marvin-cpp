//
// This file makes multiple parallel requests to the same host and checks that
// each reply corresponds to the request. That is ensures no "mixing up".
//
// Note the requests go to a local private host that has been programmed
// specifically for this test
//
// Primarily this is a test of the Request object and uses Request in it's
// simplest for - that is sending an entire http message in one hit and with
// no body.
//
// Consider running this test with different Connection management strategies
// to see whether we can save on "async_connect" calls and maybe even get some
// pipe-lining
//
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

#include "boost_stuff.hpp"

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "http_header.hpp"
#include "client.hpp"

#undef VERBOSE
/*
* Now include test cases
*/
#include "pipeline.hpp"
#include "multiple.hpp"
#include "roundtrip.hpp"

int main(int argc, char* argv[]){
    RBLogging::setEnabled(false);
//    testcase_pipeline();

    char* _argv[2] = {argv[0], (char*)"--catch_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    int result = Catch::Session().run( argc, argv );

    return result;
}
