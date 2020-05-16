//
//  main.m
//  test_buffers
//
//  Created by ROBERT BLACKWELL on 11/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//
#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/configure_trog.hpp>

using namespace Marvin;
TEST_CASE("buffer_chain_assignment")
{
    ContigBuffer mb(100);
    boost::asio::mutable_buffer bt = boost::asio::buffer(mb.data(), mb.capacity());
    BufferChain chain1;
    BufferChain chain2;
    std::string stmp = "GH";

    for( int i = 0; i < 10; i++) {
        ContigBufferSPtr mb = std::shared_ptr<ContigBuffer>(new ContigBuffer(i*10 + 10));
        stmp += "GH";
        mb->append((void*) stmp.c_str(), stmp.size());
        chain1.push_back(mb);
    }
    chain2 = chain1;
    auto mm = chain1.amalgamate();
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;
}
TEST_CASE("buffer_chain_makeboostbuffer")
{
    BufferChain chain1;
    BufferChain chain2;
    std::string stmp = "GH";

    for( int i = 0; i < 10; i++) {
        ContigBufferSPtr mb = std::shared_ptr<ContigBuffer>(new ContigBuffer(i*10 + 10));
        stmp += "GH";
        mb->append((void*) stmp.c_str(), stmp.size());
        chain1.push_back(mb);
    }
    auto bb = chain1.asio_buffer_sequence();
    auto asiobuf = boost::asio::buffer(bb);
    std::string  outbound_header_ = "outbound headers";
    std::string  outbound_data_ = "outbound data";
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(outbound_header_));
    buffers.push_back(boost::asio::buffer(outbound_data_));
    chain2 = chain1;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;
}

#pragma mark - testcase_mbuffer
TEST_CASE("mbuffer_01")
{
    std::vector<std::string> v {
        "1234567890",
        "",
        "1234567890",
        "1234567890",
        "1234567890",
        "1234567890"
    };
    
    ContigBuffer* mb = new ContigBuffer(1000);
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
