
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)
#include <gtest/gtest.h>
#include "tsc_req_handler.hpp"
#include "server_runner.hpp"
#include "bf_fix_multiple.hpp"
ServerRunner BFMultiple::destination_server_runner;
void BFMultiple::SetUpTestCase(){
    std::cout << __FUNCTION__ << std::endl;
    destination_server_runner.setup(9991);
}
void BFMultiple::TearDownTestCase(){
    std::cout << __FUNCTION__ << std::endl;
    sleep(1);
    destination_server_runner.teardown();
}
void BFMultiple::SetUp(){
    std::cout << __FUNCTION__ << std::endl;
}
void BFMultiple::TearDown(){
    std::cout << __FUNCTION__ << std::endl;

}

