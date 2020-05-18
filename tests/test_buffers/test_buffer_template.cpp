//
//  main.m
//  test_buffers
//
//  Created by ROBERT BLACKWELL on 11/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//
#include <iostream>
#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer_for_tests.hpp>

using namespace Marvin;
// this models a buffer object parameterized with a stragegy
// class S an instance of which will be passed to each BufObj at construction
template<typename S>
class BufObj
{
public:
    std::string m_str;
    S* m_s;
    BufObj(S* s):m_s(s) {}
    BufObj(S* s, std::string str): m_s(s), m_str(str){}
    std::string get_string() const
    {
        return m_s->func(m_str);
    }
};
//
// A factory object that creates BufObj and passes the comon sttrategy
// object to each new BufObj
//
template<typename S>
class XFactory
{
public:
    S* m_s;
    XFactory(S* s): m_s(s) {}
    std::shared_ptr<BufObj<S>> make(std::string str)
    {
        auto sptr = std::make_shared<BufObj<S>>(m_s, m_s->func(str));
        return sptr;
    }
};
// a mock srategy object that simple wraps the string used
// to create BufObjs
struct XStrategy
{
    XStrategy(){}
    std::string func(std::string instr)
    {
        return "XXXXX--" + instr + "--YYYYY";
    }
};


using ContigHeaderBuffer = Marvin::ContigBuffer;
using ContigBodyBuffer = Marvin::ContigBuffer;
int main()
{
    BufferMallocator buf_mallocator;
    ContigBufferFactoryT header_factory{buf_mallocator};
    BodyAllocator body_allocator;
    ContigBufferFactoryT body_factory{body_allocator};

    ContigHeaderBuffer::SPtr hb = header_factory.makeSPtr("thisis a string");
    ContigHeaderBuffer::SPtr mb = header_factory.makeSPtr("thisisastriing");
    ContigBodyBuffer::SPtr bb = body_factory.makeSPtr("thisisastriing");
    ContigBodyBuffer::SPtr bb2 = body_factory.makeSPtr("thisisastriing");
    std:: cout << __func__ << std::endl;
    std::string s1 = std::string(std::size_t(1000), 'M');
    mb->append(s1);
    hb->append(s1);
    std::string s2 = std::string(std::size_t(32000), 'B');
    bb->append(s2);
    std::string s22 = std::string(std::size_t(33000), 'B');
    bb2->append(s22);
    BufferChain bc{body_factory};
    // holy grail - contig buffers with different strategies but still on the same chain
    // the cost - a buffer chain must have a factory
    // and we have one class hierachy using runtime inheritance BufferStrategyInterface
    bc.push_back(mb);
    bc.push_back(bb);
    std:: cout << __func__ << std::endl;
#if 0
    XStrategy* strategy = new XStrategy();
    using Examp = BufObj<XStrategy>;
    XFactory<XStrategy> factory{strategy};
    auto r = factory.make("thisisthestring");
    std:: cout << __func__ << std::endl;
#endif
}