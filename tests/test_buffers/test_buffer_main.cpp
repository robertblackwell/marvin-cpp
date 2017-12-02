//
//  main.m
//  test_buffers
//
//  Created by ROBERT BLACKWELL on 11/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//
#include <iostream>
#include <gtest/gtest.h>
#include "bufferV2.hpp"
#include "rb_logger.hpp"
#if 0
#pragma mark - local classes to access internals of MBuffer, FBuffer and Fragment

class LocalMBuffer : public MBuffer
{
    
};
class LocalFragment : Fragment
{
    
};
class LocalFBuffer : public FBuffer
{
public:
    MBuffer* container()
    {
        return _container;
    }
    std::vector<Fragment>& fragments()
    {
        return _fragments;
    }
};

#pragma mark - testcase_mbuffer
TEST( Buffers, MBuffer)
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
    
    ASSERT_EQ(mb->size(), 50);
//    std::cout << *mb << std::endl;
    
    auto constb = mb_as_const_buffer(*mb);
    ASSERT_EQ(boost::asio::buffer_size(constb), mb->size());
    ASSERT_EQ(boost::asio::buffer_cast<const void*>(constb), mb->data());
    
    auto mut_b = mb_as_mutable_buffer(*mb);
    ASSERT_TRUE(boost::asio::buffer_size(mut_b) == mb->size());
    ASSERT_TRUE(boost::asio::buffer_cast<void*>(mut_b) == mb->data());

    delete mb;
}
#pragma mark - testcase_fbuffer
TEST( Buffer, FBuffer_01)
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
    MBuffer* mb_internal = new MBuffer(1000);
    FBuffer* fb = new FBuffer(mb_internal);
    for( std::string& s: v) {
        
        mb->append((void*)s.c_str(), s.size());
        fb->copyIn((void*)s.c_str(), s.size());
    }
    std::cout << *fb << std::endl;
    ASSERT_TRUE(fb->size() == 50);
    
    auto const_buf_seq = fb_as_const_buffer_sequence(*fb);
    // next statement proves the const_buf_seq is a suitable thing for in boost async io functions
    auto const_buf_seq_size = boost::asio::buffer_size(const_buf_seq);
    ASSERT_TRUE(const_buf_seq_size == fb->size());
    auto mut_buf_seq = fb_as_mutable_buffer_sequence(*fb);
    // next statement proves the mut_buf_seq is a suitable thing for in boost async io functions
    auto mut_buf_seq_size = boost::asio::buffer_size(mut_buf_seq);
    ASSERT_TRUE(mut_buf_seq_size == fb->size());

    delete fb;
    delete mb;

}
#pragma mark - testcase_fbuffer2

TEST( Buffer, FBuffer_02)
{
    std::string data = "1234567890123456789012345678901234567890";
    std::vector<std::string> v {
        "@",
        "1234567890",
        "@@@",
        "",
        "@@@",
        "1234567890",
        "1234567890",
        "@@@@",
        "1234567890",
        "@@@@@",
        "1234567890",
        "@@",
        "1234567890"
    };
    std::vector<int> v_offset = {
        5,3,6,8,2,1
    };
    std::string total = "";
    MBuffer* mb = new MBuffer(1000);
    MBuffer* mb_internal = new MBuffer(1000);
    FBuffer* fb = new FBuffer(mb_internal);
    for( std::string& s: v) {
        total += s;
        auto ptr = mb_internal->nextAvailable();
        auto len = s.size();
//        std::cout << "s: " << s << " ptr: " << std::hex << ptr << " len: " << std::dec << len << std::endl;
//        mb->append((void*)s.c_str(), s.size()); // put all the strings in
        mb_internal->append((void*)s.c_str(), s.size()); // put all the strings in
        if (s[0] == '@') {
            
        } else {
            fb->addFragment((void*) ptr, len);
        }
    }
//    std::cout << *fb << std::endl;
    LocalFBuffer* lfb = (LocalFBuffer*) fb;
    std::vector<std::string> frags;
    for(Fragment fg: lfb->fragments()) {
        char* p = (char*)fg.startPointer();
        std::size_t sz = fg.size();
        std::string frag_str(p, sz);
//        std::cout << frag_str << std::endl;
        frags.push_back(frag_str);
    }
    std::vector<std::string> expected = {
        "1234567890",
        "12345678901234567890",
        "1234567890",
        "1234567890",
        "1234567890"
    };

    auto const_buf_seq = fb_as_const_buffer_sequence(*fb);
    // next statement proves the const_buf_seq is a suitable thing for in boost async io functions
    auto const_buf_seq_size = boost::asio::buffer_size(const_buf_seq);
    ASSERT_TRUE(const_buf_seq_size == fb->size());
    auto mut_buf_seq = fb_as_mutable_buffer_sequence(*fb);
    // next statement proves the mut_buf_seq is a suitable thing for in boost async io functions
    auto mut_buf_seq_size = boost::asio::buffer_size(mut_buf_seq);
    ASSERT_TRUE(mut_buf_seq_size == fb->size());

    ASSERT_TRUE(expected == frags);
    
}
#endif
#pragma mark - main
int main(int argc, char * argv[]) {
    RBLogging::setEnabled(false);
    BufferChain chain;
//    void* p1 = malloc(100);
//    void* p2 = malloc(200);
//    boost::asio::mutable_buffer b1(p1, 100);
//    boost::asio::mutable_buffer b2(p2, 200);
//    auto b3 = b2 + 10;

//    testcase_mbuffer();
//    testcase_fbuffer();
//    testcase_fbuffer2();
    char* _argv[2] = {argv[0], (char*)"--gtest_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    return RUN_ALL_TESTS();

    return 0;
}
