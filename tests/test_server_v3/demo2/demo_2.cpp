//
// This file demonstrates how to package tests when when there is a need to start
// a server before the tests start amd stop it at the end
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#define DOCTEST_CONFIG_IMPLEMENT

#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include "../timer.hpp"
#include "../v3_handler.hpp"
#include "../server_v3_runner.hpp"


// This is a test - its a class to keep properties in scope during asyn tests
class Test_ATimer 
{
    public:
    Test_ATimer(boost::asio::io_service& io, std::string label, long delay)
    {
        m_label = label;
        m_delay = delay;
        m_t_sptr = std::make_shared<ATimer>(io, label);
        this->arm();
    }
    void arm() 
    {
        m_t_sptr->arm(m_delay, [this]()
        {
            std::cout << "callback " << m_label << std::endl; 
        });
    }
    std::string                 m_label;
    long                        m_delay;
    std::shared_ptr<ATimer>     m_t_sptr;
};

// Test package - this is 2 asyn tests in parallel
TEST_CASE("01")
{
    boost::asio::io_service io;
    Test_ATimer tst1(io, "Test 1.1", 2);
    Test_ATimer tst2(io, "Test 1.2", 2);
    io.run();
}
// test package this is 3 async tests in parallel
TEST_CASE("02")
{
    boost::asio::io_service io;
    Test_ATimer tst1(io, "Test 2.1", 2);
    Test_ATimer tst2(io, "Test 2.2", 2);
    Test_ATimer tst3(io, "Test 2.2", 2);
    io.run();
}
//
// This is an example of how to run a set of asyn tests that require a server
// be running for those tests
//
int main(int argc, char* argv[])
{
    RBLogging::setEnabled(true);
    ServerRunner  server_runner;
    server_runner.setup(9000, [](boost::asio::io_service& io)
    {
        return new Handler(io);
    });
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int result = context.run(); // run
    std::cout << "Final" << std::endl;
    server_runner.terminateServer();
    return result;
}
