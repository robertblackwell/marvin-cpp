//
//  main.m
//  test_buffers
//
//  Created by ROBERT BLACKWELL on 11/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//
#include <iostream>
#include "buffer.hpp"
#include "rb_logger.hpp"
#include <gtest/gtest.h>

#pragma mark - testcase_mbuffer
TEST(mbuffer, 01)
{
    std::vector<std::string> v {
        "1234567890",
        "",
        "1234567890",
        "1234567890",
        "1234567890",
        "1234567890"
    };
    
    MBuffer* mb = new MBuffer(1000);
    for( std::string& s: v) {
        mb->append((void*)s.c_str(), s.size());
    }
    
    ASSERT_TRUE(mb->size() == 50);
//    std::cout << *mb << std::endl;
    
    auto constb = mb_as_const_buffer(*mb);
    ASSERT_TRUE(boost::asio::buffer_size(constb) == mb->size());
    ASSERT_TRUE(boost::asio::buffer_cast<const void*>(constb) == mb->data());
    
    auto mut_b = mb_as_mutable_buffer(*mb);
    ASSERT_TRUE(boost::asio::buffer_size(mut_b) == mb->size());
    ASSERT_TRUE(boost::asio::buffer_cast<void*>(mut_b) == mb->data());

    delete mb;
}

