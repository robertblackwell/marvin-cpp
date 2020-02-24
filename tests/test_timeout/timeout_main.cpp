//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include "boost_stuff.hpp"
//#define CATCH_CONFIG_RUNNER
//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
//#include "utest.hpp"
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "timeout.hpp"
#define USE_STRANDx

#include "async_compound.cpp"
#include "async_object.cpp"

/// test timing out an async operation
//TEST_CASE("TimeoutcomposedOpOTO2")
#if 0
TEST_CASE("TimeoutcomposedOpOTO2", "[]")
{
    printf("TimeoutcomposedOpOTO2\n");
    boost::asio::io_service io;
    AsyncComposedOp obj(
        io,
        1, "result 1", // timeout, result string operation 1
        3, "result 2", // timeout, result string operation 2
        3, // timeout on operation 1
        1 // timeout operation 2
        );
    /// op interval is 2 timeout interval is 1 - op will be timed out
    obj.async_composed_op([](std::string result, const boost::system::error_code err) {
        boost::system::error_code ec = err;
        auto asio_op_aborted = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);
#if 0
        std::string s = Marvin::make_error_description(ec);
        std::cout << "main line handler " << s << " result: "<< result<< std::endl;
        std::cout << err.message() << std::endl;
        std::cout << ec.message() << std::endl;
        std::cout << asio_op_aborted.message() << std::endl;
        std::cout << Marvin::make_error_description(ec) << std::endl;
        std::cout << Marvin::make_error_description(asio_op_aborted) << std::endl;
#endif
        REQUIRE(result == "result 1");
        REQUIRE(err == asio_op_aborted);
    });
    io.run();
}
#endif
/// test timing out an async operation
//TEST_CASE("TimeoutcomposedOpOTO1")
#if 0
TEST_CASE("TimeoutcomposedOpOTO1", "[]")
{
    printf("TimeoutcomposedOpOTO1\n");
    boost::asio::io_service io;
    AsyncComposedOp obj(
        io,
        3, "result 1", // timeout, result string operation 1
        1, "result 2", // timeout, result string operation 2
        1, // timeout on operation 1
        12 // timeout operation 2
        );
    /// op interval is 2 timeout interval is 1 - op will be timed out
    obj.async_composed_op([](std::string result, const boost::system::error_code err) {
        boost::system::error_code ec = err;
        auto asio_op_aborted = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);
#if 0
        std::string s = Marvin::make_error_description(ec);
        std::cout << "main line handler " << s << " result: "<< result<< std::endl;
        std::cout << err.message() << std::endl;
        std::cout << ec.message() << std::endl;
        std::cout << asio_op_aborted.message() << std::endl;
        std::cout << Marvin::make_error_description(ec) << std::endl;
        std::cout << Marvin::make_error_description(asio_op_aborted) << std::endl;
#endif
        REQUIRE(result == "");
        REQUIRE(err == asio_op_aborted);
    });
    io.run();
}
#endif

//TEST_CASE("TimeoutcomposedOpOOK2")
#if 0
TEST_CASE("TimeoutcomposedOpOOK2","[]")
{
    printf("TimeoutcomposedOpOOK2\n");
    boost::asio::io_service io;
    AsyncComposedOp obj(
        io,
        1, "result 1", // timeout, result string operation 1
        1, "result 2", // timeout, result string operation 2
        10, // timeout on operation 1
        12 // timeout operation 2
        );
    /// op interval is 2 timeout interval is 1 - op will be timed out
    obj.async_composed_op([](std::string result, const boost::system::error_code err) {
        boost::system::error_code ec = err;
        auto asio_op_aborted = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);
#if 0
        std::string s = Marvin::make_error_description(ec);
        std::cout << "main line handler " << s << " result: "<< result<< std::endl;
        std::cout << err.message() << std::endl;
        std::cout << ec.message() << std::endl;
        std::cout << asio_op_aborted.message() << std::endl;
        std::cout << Marvin::make_error_description(ec) << std::endl;
        std::cout << Marvin::make_error_description(asio_op_aborted) << std::endl;
#endif
        REQUIRE(result == "result 1result 2");
        REQUIRE(!err);
    });
    io.run();
}
#endif

#if 0
/// test timing out an async operation
//TEST_CASE("Timeouttimedout")
TEST_CASE("Timeouttimedout", "[]")
{
    printf("Timeouttimedout\n");
    boost::asio::io_service io;
    std::shared_ptr<AsyncObject> obj_sptr = std::make_shared<AsyncObject>(io);
    auto cb = [&obj_sptr]() {
        obj_sptr = nullptr;
    };
    /// op interval is 2 timeout interval is 1 - op will be timed out
    obj_sptr->async_operation(2, 1, [cb](const boost::system::error_code err){
        boost::system::error_code ec = err;
        auto asio_op_aborted = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);
#if 0
        std::string s = Marvin::make_error_description(ec);
        std::cout << "main line handler " << s << std::endl;
        std::cout << err.message() << std::endl;
        std::cout << ec.message() << std::endl;
        std::cout << asio_op_aborted.message() << std::endl;
        std::cout << Marvin::make_error_description(ec) << std::endl;
        std::cout << Marvin::make_error_description(asio_op_aborted) << std::endl;
#endif
        REQUIRE(err == asio_op_aborted);
        cb();
    });
    io.run();
}
#endif
#if 1
/// test timing out an async operation
//TEST_CASE("Timeouttimedout")
TEST_CASE("Timeout_success", "[]")
{
    printf("Timeout_success\n");
    boost::asio::io_service io;
    std::shared_ptr<AsyncObject> obj_sptr = std::make_shared<AsyncObject>(io);
    auto cb = [&obj_sptr]() {
        obj_sptr = nullptr;
    };
    /// op interval is 2 timeout interval is 1 - op will be timed out
    obj_sptr->async_operation(1, 2, [cb](const boost::system::error_code err){
        boost::system::error_code ec = err;
        auto asio_op_ok = Marvin::make_error_ok();
#if 0
        std::string s = Marvin::make_error_description(ec);
        std::cout << "main line handler " << s << std::endl;
        std::cout << err.message() << std::endl;
        std::cout << ec.message() << std::endl;
        std::cout << asio_op_aborted.message() << std::endl;
        std::cout << Marvin::make_error_description(ec) << std::endl;
        std::cout << Marvin::make_error_description(asio_op_aborted) << std::endl;
#endif
        REQUIRE(!err);
        cb();
    });
    io.run();
}
#endif


#if 0
/// test timing out an async operation
//TEST_CASE("Timeouttimedout")
TEST_CASE("Timeout_donothing", "[]")
{
    printf("Timeout_donothing\n");
    boost::asio::io_service io;
    std::shared_ptr<AsyncObject> obj_sptr = std::make_shared<AsyncObject>(io);
    io.run();
    obj_sptr = nullptr;
    std::cout << "after io.run()" << std::endl;
}


#endif
#if 0
/// test an async operation completing successfully and cancelling a timeout
//TEST_CASE("Timeoutexpired")
TEST_CASE("Timeoutexpired","[]")
{
    printf("Timeoutexpired\n");
   boost::asio::io_service io;
    AsyncObject obj(io);
    /// op interval 1 sec timeout 2 secs op will complete successfully
    obj.async_operation(1, 2, [](const boost::system::error_code err){
        boost::system::error_code ec = err;
#if 0
        std::string s = Marvin::make_error_description(ec);
        std::cout << "main line handler " << s << std::endl;
        auto asio_op_aborted = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);
        std::cout << err.message() << std::endl;
        std::cout << ec.message() << std::endl;
        std::cout << asio_op_aborted.message() << std::endl;
        std::cout << Marvin::make_error_description(ec) << std::endl;
        std::cout << Marvin::make_error_description(asio_op_aborted) << std::endl;
#endif
        REQUIRE(!err);
    });
    io.run();
}
#endif
#if 0
/// test an async op that should fail but iit gets tiimed out
//TEST_CASE("TimeoutFailedOpTimedout")
TEST_CASE("TimeoutFailedOpTimedout","[]")
{
    printf("TimeoutFailedOpTimedout\n");
    boost::asio::io_service io;
    AsyncObject obj(io);
    /// op interval is 2 timeout interval is 1 - op will be timed out
    obj.async_fail_operation(2, 1, [](const boost::system::error_code err){
        boost::system::error_code ec = err;
        auto asio_op_aborted = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);
#if 0
        std::string s = Marvin::make_error_description(ec);
        std::cout << "main line handler " << s << std::endl;
        std::cout << err.message() << std::endl;
        std::cout << ec.message() << std::endl;
        std::cout << asio_op_aborted.message() << std::endl;
        std::cout << Marvin::make_error_description(ec) << std::endl;
        std::cout << Marvin::make_error_description(asio_op_aborted) << std::endl;
#endif
        REQUIRE(err == asio_op_aborted);
    });
    io.run();
}
#endif
#if 0
/// test an async operation that fails with a broken pipe error before a timeout expires
//TEST_CASE("TimeoutfailedOpTimeoutExpired")
TEST_CASE("TimeoutfailedOpTimeoutExpired","[]")
{
    printf("TimeoutfailedOpTimeoutExpired\n");
    boost::asio::io_service io;
    AsyncObject obj(io);
    /// op interval 1 sec timeout 2 secs op will complete successfully
    obj.async_fail_operation(1, 2, [](const boost::system::error_code err){
        boost::system::error_code ec = err;
        auto asio_op_broken_pipe = boost::asio::error::make_error_code(boost::asio::error::broken_pipe);
#if 0
        std::string s = Marvin::make_error_description(ec);
        std::cout << "main line handler " << s << std::endl;
        std::cout << err.message() << std::endl;
        std::cout << ec.message() << std::endl;
        std::cout << asio_op_aborted.message() << std::endl;
        std::cout << Marvin::make_error_description(ec) << std::endl;
        std::cout << Marvin::make_error_description(asio_op_aborted) << std::endl;
#endif
        REQUIRE(err == asio_op_broken_pipe);
    });
    
    io.run();
}

#endif
#if 0
int main(int argc, char** argv) {
    doctest::Context context;

    // !!! THIS IS JUST AN EXAMPLE SHOWING HOW DEFAULTS/OVERRIDES ARE SET !!!

    // defaults
    context.addFilter("test-case-exclude", "*math*"); // exclude test cases with "math" in their name
    context.setOption("abort-after", 5);              // stop test execution after 5 failed assertions
    context.setOption("order-by", "name");            // sort the test cases by their name

    context.applyCommandLine(argc, argv);

    // overrides
    context.setOption("no-breaks", true);             // don't break in the debugger when assertions fail

    int res = context.run(); // run

    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests
    
    int client_stuff_return_code = 0;
    // your program - if the testing framework is integrated in your production code
    
    return res + client_stuff_return_code; // the result from doctest is propagated here as well
}
#else
int main(int argc, char * argv[])
{
    RBLogging::setEnabled(false);

    char* _argv[2] = {argv[0], (char*)"-r junit"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    printf("Timeout\n");
    int result = Catch::Session().run( argc, argv );
    printf("Timeout\n");
    return result;
}
#endif
