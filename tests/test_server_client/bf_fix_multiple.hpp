#ifndef marvin_bf_fix_multiple_hpp
#define marvin_bf_fix_multiple_hpp
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <gtest/gtest.h>

#include "boost_stuff.hpp"
#include "rb_logger.hpp"
#include "tsc_req_handler.hpp"
#include "server_runner.hpp"

class BFMultiple : public ::testing::Test
 {
    public:
    static ServerRunner destination_server_runner;
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp();
    virtual void TearDown();
 };
 #endif

