#ifndef marvin_buffer_for_tests_hpp
#define marvin_buffer_for_tests_hpp

#include <marvin/buffer/buffer_allocator.hpp>
#include <marvin/buffer/contig_buffer_t.hpp>
#include <marvin/buffer/contig_buffer_factory_t.hpp>
#include <marvin/buffer/buffer_chain_t.hpp>
namespace Marvin {
/**
 * These functons provides a conveniently accessible strategy and factory for testing purposes
 * using the old interface that does not require an explicit strategy. This saved me
 * hundereds of edits of the unit test code
 */
ContigBufferFactoryT &getTestFactory()
{
    static BufferMallocator strategy;
    static ContigBufferFactoryT factory(strategy);
    return factory;
}

BufferChainT::SPtr makeBufferChainSPtr()
{
    return std::make_shared<BufferChainT>(getTestFactory());
}
BufferChainT::SPtr makeBufferChainSPtr(ContigBufferFactoryT& factory)
{
    BufferChainT::SPtr sp = std::make_shared<BufferChainT>(getTestFactory());
    return sp;
}
BufferChainT::SPtr makeBufferChainSPtr(BufferChainT&& other)
{
    BufferChainT::SPtr sp = std::make_shared<BufferChainT>(std::move(other));
    return sp;
}
BufferChainT::SPtr makeBufferChainSPtr(std::string& s)
{
    BufferChainT::SPtr sp = std::make_shared<BufferChainT>(getTestFactory());
    sp->push_back(getTestFactory().makeSPtr(s));
    return sp;
}
BufferChainT::SPtr makeBufferChainSPtr(ContigBufferT& cb)
{
    BufferChainT::SPtr sp = std::make_shared<BufferChainT>(getTestFactory());
    sp->push_back(getTestFactory().makeSPtr(cb));
    return sp;
}
BufferChainT::SPtr makeBufferChainSPtr(ContigBufferT::SPtr cb_sptr)
{
    BufferChainT::SPtr sp = std::make_shared<BufferChainT>(getTestFactory());
    sp->push_back(cb_sptr);
    return sp;
}

ContigBufferT::SPtr makeContigBufferTSPtr()
{
    return getTestFactory().makeSPtr();
}
ContigBufferT::SPtr makeContigBufferTSPtr(std::size_t capacity)
{
    std::size_t sz = (capacity > ContigBufferT::min_buffer_size) ? capacity : ContigBufferT::min_buffer_size ;
    return getTestFactory().makeSPtr((sz));
}
ContigBufferT::SPtr makeContigBufferTSPtr(std::string s)
{
    ContigBufferT::SPtr mbp = getTestFactory().makeSPtr(s.size());
    mbp->append((void*) s.c_str(), s.size());
    return mbp;
}
ContigBufferT::SPtr makeContigBufferTSPtr(void* mem, std::size_t size)
{
    ContigBufferT::SPtr mbp = getTestFactory().makeSPtr(size);
    mbp->append(mem, size);
    return mbp;
}
ContigBufferT::SPtr makeContigBufferTSPtr(ContigBufferT& mb)
{
    ContigBufferT::SPtr mbp = getTestFactory().makeSPtr(mb.capacity());
    mbp->append(mb.data(), mb.size());
    return mbp;
}



} // namespace
#endif
