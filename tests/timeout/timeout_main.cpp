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
#include <gtest/gtest.h>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "timeout.hpp"
#define USE_STRANDx

#include "async_compound.cpp"
#include "async_object.cpp"

/// test timing out an async operation
TEST(Timeout, composedOpOTO2)
{
    
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
        ASSERT_TRUE(result == "result 1");
        ASSERT_TRUE(err == asio_op_aborted);
    });
    io.run();
}
/// test timing out an async operation
TEST(Timeout, composedOpOTO1)
{
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
        ASSERT_TRUE(result == "");
        ASSERT_TRUE(err == asio_op_aborted);
    });
    io.run();
}


TEST(Timeout, composedOpOOK2)
{
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
        ASSERT_TRUE(result == "result 1result 2");
        ASSERT_TRUE(!err);
    });
    io.run();
}


#if 0
/// test timing out an async operation
TEST(Timeout, timedout)
{
    boost::asio::io_service io;
    AsyncObject obj(io);
    /// op interval is 2 timeout interval is 1 - op will be timed out
    obj.async_operation(2, 1, [](const boost::system::error_code err){
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
        ASSERT_TRUE(err == asio_op_aborted);
    });
    io.run();
}
/// test an async operation complting successfully and cancelling a timeout
TEST(Timeout, expired)
{
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
        ASSERT_TRUE(!err);
    });
    
    io.run();
}
/// test an async op that should fail but iit gets tiimed out
TEST(Timeout, FailedOpTimedout)
{
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
        ASSERT_TRUE(err == asio_op_aborted);
    });
    io.run();
}
/// test an async operation that fails with a broken pipe error before a timeout expires
TEST(Timeout, failedOpTimeoutExpired)
{
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
        ASSERT_TRUE(err == asio_op_broken_pipe);
    });
    
    io.run();
}
#endif

int main(int argc, char * argv[])
{
    RBLogging::setEnabled(false);

    char* _argv[2] = {argv[0], (char*)"--gtest_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
