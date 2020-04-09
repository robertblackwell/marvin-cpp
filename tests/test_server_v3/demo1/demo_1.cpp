//
// This file demonstrates how to package tests when when there is a need to start
// a server before the tests start amd stop it at the end
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <memory>
#include <pthread.h>
// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/server_v3/adapter.hpp>
#include <marvin/external_src/trog/trog.hpp>
Trog_SETLEVEL(LOG_LEVEL_WARN)
#include <marvin/server_v3/timer.hpp>
#include "../handle_app.hpp"
#include "../server_v3_runner.hpp"


// This is a test - its a class to keep properties in scope during asyn tests
class Test_ATimer 
{
    public:
    Test_ATimer(boost::asio::io_service& io, std::string label, long delay)
    {
        m_label = label;
        m_delay = delay;
        m_t_sptr = std::make_shared<Marvin::ATimer>(io, label);
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
    std::shared_ptr<Marvin::ATimer>     m_t_sptr;
};

// Test package - this is 2 asyn tests in parallel
void test_01 ()
{
    boost::asio::io_service io;
    Test_ATimer tst1(io, "Test 1.1", 2);
    Test_ATimer tst2(io, "Test 1.2", 2);
    io.run();
}
// test package this is 3 async tests in parallel
void test_02 ()
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
TEST_CASE("0")
{
    //===========================================================================================
    // get a server running
    //===========================================================================================
    ServerRunner  server_runner;
    server_runner.setup(9000, [](boost::asio::io_service& io)
    {
        return std::make_unique<AppHandler>(io);
    });
    //===========================================================================================
    // Now run the tests
    //===========================================================================================
    test_01();
    test_02();
    //===========================================================================================
    // terminate the server
    //===========================================================================================
    std::cout << "Final" << std::endl;
    server_runner.terminateServer();
}
