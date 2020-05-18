#ifndef marvin_contig_buffer_factory_t_hpp
#define marvin_contig_buffer_factory_t_hpp
#include <stddef.h>
#include <memory>
#include <iostream>
#include <iterator>
#include <boost/asio/buffer.hpp>
#include <marvin/error_handler/error_handler.hpp>
#include <marvin/buffer/buffer_allocator.hpp>
#include <marvin/buffer/contig_buffer.hpp>
namespace Marvin {
/**
 * Factory for constructing shared pointers to ContigBuffer objects.
 * The factory injects a BufferStrategy conforming to ConfigBufferInterface
 * into each ContigBuffer constructed.
 */
class ContigBufferFactoryT
{
protected:
    using S = BufferStrategyInterface;
    S& m_strategy;
    using BufType = typename Marvin::ContigBuffer;
    using BufSPtr = typename Marvin::ContigBuffer::SPtr;
    using BufPtr = typename Marvin::ContigBuffer*;
public:
    ContigBufferFactoryT(S& s): m_strategy(s)
    {}
    BufSPtr makeSPtr()
    {
        return std::make_shared<BufType>(m_strategy);
    }
    BufSPtr makeSPtr(std::size_t capacity)
    {
        return std::make_shared<BufType>(m_strategy, (capacity));
    }
    BufSPtr makeSPtr(std::string s)
    {
        BufSPtr mbp = std::make_shared<BufType>(m_strategy, (s.size()));
        mbp->append((void*) s.c_str(), s.size());
        return mbp;
    }
    BufSPtr makeSPtr(void* mem, std::size_t size)
    {
        BufSPtr mbp = std::make_shared<BufType>(m_strategy, (size));
        mbp->append(mem, size);
        return mbp;
    }
    BufSPtr makeSPtr(BufType& mb)
    {
        BufSPtr mbp = std::make_shared<BufType>(m_strategy, (mb.capacity()));
        mbp->append(mb.data(), mb.size());
        return mbp;
    }
    BufType* makePtr()
    {
        return new BufType(m_strategy);
    }

};

} //namespace Marvin
#endif
