#include <iostream>

// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/configure_trog.hpp>

using namespace Marvin;
TEST_CASE("append")
{
    ContigBufferSPtr mbsptr = Marvin::ContigBuffer::makeSPtr("thisisastring");
    BufferChainSPtr bc_sptr = Marvin::BufferChain::makeSPtr();
    bc_sptr->push_back(mbsptr);
    bc_sptr->append("1234567890");
    Marvin::BufferChain::AsioConstBufferSeq const_buf_seq =  bc_sptr->asio_buffer_sequence();
    auto x = boost::asio::is_const_buffer_sequence<Marvin::BufferChain::AsioConstBufferSeq>();
    auto s1 = bc_sptr->to_string();
}
TEST_CASE("m_buffer append_realloc")
{
    ContigBuffer mb{0};
    CHECK(mb.capacity() == ContigBuffer::min_buffer_size);
    std::string big_str = "";
    for(int j = 0; j<100; j++) {
        big_str += "0123456789ABCDEF";
    }
    static std::string const tmpchars = "abcdefghijlkn";
    static std::string tmpchars2 = "abcdefghijlkn";
    // calls the append(std::string* ) variant
    mb.append(&big_str);

    // calls the append(std::string const & ) variant
    mb.append(tmpchars);
    mb.append(tmpchars2);

    // calls the append(std::string && ) variant
    mb.append("thisisatemporary");
    mb.append(std::string("thisisanothertemporary"));
    mb.append(std::move(std::string("thisisanothertemporary")));
    CHECK(mb.capacity() > ContigBuffer::min_buffer_size);
    std::cout << __func__ << std::endl;
}
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
    std::string smb = mb->toString();
    std::string expect = "12345678901234567890123456789012345678901234567890";
    CHECK((smb == expect));
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
TEST_CASE("copy constructor")
{
    // demonstrate copy not reference
    BufferChain bc1{};
    bc1.append("1111Thisisthefirstbuffer");
    bc1.append("222Thisisthesecondbuffer");
    BufferChain bc2{bc1};
    ContigBuffer mb1 = bc1.block_at(0);
    std::string s1 = mb1.toString();
    ContigBuffer mb2 = bc2.block_at(0);
    std::string s2 = mb2.toString();
    CHECK((s1 == s2));
    bc2.append("XXXXXXX");
    mb2 = bc2.block_at(0);
    s2 = mb2.toString();
    CHECK((s1 != s2));
}
TEST_CASE("copy constructor")
{
    // demonstrate copy not reference
    BufferChain bc1{};
    bc1.append("1111Thisisthefirstbuffer");
    bc1.append("222Thisisthesecondbuffer");
    BufferChain bc2{bc1};

    // demonstrate bc1 and bc2 have the same value
    ContigBuffer mb1 = bc1.block_at(0);
    std::string s1 = mb1.toString();
    ContigBuffer mb2 = bc2.block_at(0);
    std::string s2 = mb2.toString();
    CHECK(s1 == s2);
    CHECK(bc1.size() == bc2.size());
    CHECK(bc1.blocks() == bc2.blocks());
    CHECK(bc1.to_string() == bc2.to_string());
    // bc1 and bc2 have the same value

    //now demonstrate they are NOT references to the same thing
    bc2.append("XXXXXXX");
    mb1 = bc1.block_at(0);
    s1 = mb1.toString();
    mb2 = bc2.block_at(0);
    s2 = mb2.toString();
    CHECK((s1 != s2));
    CHECK(bc1.size() != bc2.size());
    CHECK(bc1.to_string() != bc2.to_string());
}
TEST_CASE("copy assignment")
{
    // demonstrate copy not reference
    BufferChain bc1{};
    bc1.append("1111Thisisthefirstbuffer");
    bc1.append("222Thisisthesecondbuffer");
    BufferChain bc2{};
    bc2 = bc1;

    // demonstrate bc1 and bc2 have the same value
    ContigBuffer mb1 = bc1.block_at(0);
    std::string s1 = mb1.toString();
    ContigBuffer mb2 = bc2.block_at(0);
    std::string s2 = mb2.toString();
    CHECK(s1 == s2);
    CHECK(bc1.size() == bc2.size());
    CHECK(bc1.blocks() == bc2.blocks());
    CHECK(bc1.to_string() == bc2.to_string());
    // bc1 and bc2 have the same value

    //now demonstrate they are NOT references to the same thing
    bc2.append("XXXXXXX");
    mb1 = bc1.block_at(0);
    s1 = mb1.toString();
    mb2 = bc2.block_at(0);
    s2 = mb2.toString();
    CHECK((s1 != s2));
    CHECK(bc1.size() != bc2.size());
    CHECK(bc1.to_string() != bc2.to_string());
}
TEST_CASE("move")
{
    // demonstrate copy not reference
    BufferChain bc1{};
    bc1.append("1111Thisisthefirstbuffer");
    bc1.append("222Thisisthesecondbuffer");
    // this causes a move
    BufferChain bc2{std::move(bc1)};
    CHECK(bc1.size() == 0);
    CHECK(bc2.size() != 0);
    std::string stmp = "0123456789";
    BufferChainSPtr bc_sptr_2 = BufferChain::makeSPtr(stmp);
    // this causes a copy not a move
    BufferChain bc3 = (*bc_sptr_2);
    auto x = bc3.size();
    auto x2 = bc_sptr_2->size();
    CHECK(x2 == x);
    // this one is a move
    BufferChain b4 = std::move((*bc_sptr_2));
    CHECK((b4.size() != 0));
    CHECK((bc_sptr_2->size() == 0));
}
TEST_CASE("move with pointers 1")
{
    // another move
    std::string stmp = "0123456789";
    BufferChainSPtr bc_sptr_1 = BufferChain::makeSPtr(stmp);
    CHECK((bc_sptr_1->size() != 0));
    BufferChainSPtr bc_sptr_2 = std::make_shared<BufferChain>(std::move(*bc_sptr_1));
    CHECK((bc_sptr_1->size() == 0));
    CHECK((bc_sptr_2->size() != 0));

}
TEST_CASE("move with pointers 2")
{
    // another move
    std::string stmp = "0123456789";
    BufferChainSPtr bc_sptr_1 = BufferChain::makeSPtr(stmp);
    CHECK((bc_sptr_1->size() != 0));
    BufferChainSPtr bc_sptr_2 = std::make_shared<BufferChain>();
    *bc_sptr_2 = std::move(*bc_sptr_1);
    CHECK((bc_sptr_1->size() == 0));
    CHECK((bc_sptr_2->size() != 0));

}
TEST_CASE("move with makeSPtr()")
{
    // another move
    std::string stmp = "0123456789";
    BufferChainSPtr bc_sptr_1 = BufferChain::makeSPtr(stmp);
    CHECK((bc_sptr_1->size() != 0));
    BufferChainSPtr bc_sptr_2 = BufferChain::makeSPtr(std::move(*bc_sptr_1));
    CHECK((bc_sptr_1->size() == 0));
    CHECK((bc_sptr_2->size() != 0));

}