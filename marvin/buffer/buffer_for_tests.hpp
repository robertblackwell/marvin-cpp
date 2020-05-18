#ifndef marvin_buffer_for_tests_hpp
#define marvin_buffer_for_tests_hpp

#include <marvin/buffer/buffer_allocator.hpp>
#include <marvin/buffer/contig_buffer.hpp>
#include <marvin/buffer/contig_buffer_factory.hpp>
#include <marvin/buffer/buffer_chain.hpp>
namespace Marvin {
/**
 * These functons provides a conveniently accessible strategy and factory for testing purposes
 * using the old interface that does not require an explicit strategy. This saved me
 * hundereds of edits of the unit test code
 */
inline ContigBufferFactoryT &getTestFactory()
{
    static BufferMallocator strategy;
    static ContigBufferFactoryT factory(strategy);
    return factory;
}

inline BufferChain::SPtr makeBufferChainSPtr()
{
    return std::make_shared<BufferChain>(getTestFactory());
}
inline BufferChain::SPtr makeBufferChainSPtr(ContigBufferFactoryT& factory)
{
    BufferChain::SPtr sp = std::make_shared<BufferChain>(getTestFactory());
    return sp;
}
inline BufferChain::SPtr makeBufferChainSPtr(BufferChain&& other)
{
    BufferChain::SPtr sp = std::make_shared<BufferChain>(std::move(other));
    return sp;
}
inline BufferChain::SPtr makeBufferChainSPtr(std::string& s)
{
    BufferChain::SPtr sp = std::make_shared<BufferChain>(getTestFactory());
    sp->push_back(getTestFactory().makeSPtr(s));
    return sp;
}
inline BufferChain::SPtr makeBufferChainSPtr(ContigBuffer& cb)
{
    BufferChain::SPtr sp = std::make_shared<BufferChain>(getTestFactory());
    sp->push_back(getTestFactory().makeSPtr(cb));
    return sp;
}
inline BufferChain::SPtr makeBufferChainSPtr(ContigBuffer::SPtr cb_sptr)
{
    BufferChain::SPtr sp = std::make_shared<BufferChain>(getTestFactory());
    sp->push_back(cb_sptr);
    return sp;
}

inline ContigBuffer::SPtr makeContigBufferSPtr()
{
    return getTestFactory().makeSPtr();
}
inline ContigBuffer::SPtr makeContigBufferSPtr(std::size_t capacity)
{
    return getTestFactory().makeSPtr((capacity));
}
inline ContigBuffer::SPtr makeContigBufferSPtr(std::string s)
{
    ContigBuffer::SPtr mbp = getTestFactory().makeSPtr(s);
//    mbp->append((void*) s.c_str(), s.size());
    return mbp;
}
inline ContigBuffer::SPtr makeContigBufferSPtr(void* mem, std::size_t size)
{
    ContigBuffer::SPtr mbp = getTestFactory().makeSPtr(mem, size);
//    mbp->append(mem, size);
    return mbp;
}
inline ContigBuffer::SPtr makeContigBufferSPtr(ContigBuffer& mb)
{
    ContigBuffer::SPtr mbp = getTestFactory().makeSPtr(mb);
//    mbp->append(mb.data(), mb.size());
    return mbp;
}



} // namespace
#endif
