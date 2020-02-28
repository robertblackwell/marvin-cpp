//
//  main.m
//  test_buffers
//
//  Created by ROBERT BLACKWELL on 11/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//
#include <iostream>
#include <marvin/buffer/buffer.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <catch2/catch.hpp>

using namespace Marvin;

#pragma mark - testcase_mbuffer
TEST_CASE("mbuffer_01","")
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
    
    REQUIRE(mb->size() == 50);
//    std::cout << *mb << std::endl;
    
    auto constb = mb_as_const_buffer(*mb);
    REQUIRE(boost::asio::buffer_size(constb) == mb->size());
    REQUIRE(boost::asio::buffer_cast<const void*>(constb) == mb->data());
    
    auto mut_b = mb_as_mutable_buffer(*mb);
    REQUIRE(boost::asio::buffer_size(mut_b) == mb->size());
    REQUIRE(boost::asio::buffer_cast<void*>(mut_b) == mb->data());

    delete mb;
}

