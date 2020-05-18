#ifndef marvin_contig_buffer_template_hpp
#define marvin_contig_buffer_template_hpp
#include <stddef.h>
#include <memory>
#include <iostream>
#include <iterator>
#include <boost/asio/buffer.hpp>
#include <marvin/error_handler/error_handler.hpp>
//#include <marvin/buffer/buffer_allocator.hpp>
namespace Marvin {

#pragma mark - ContigBuffer class

/**
*
* \ingroup buffers
* \brief ContigBufferT provides a template class representing a contiguous expanding block of memory.r
*
* ContigBufferT class wraps a contigous buffer and provides manipulation methods.
* Once constructed the ContigBuffer instance "own" the raw memory.
* ContigBufferT destructor releases the raw memory.
* The template parameter is a strategy for how much to allocate initially and realloc
* when expansion is required.
* S should be 
*   -   default constructable
*   -   void* allocate(std::size_t size)
*   -   std::size_t reallocate_size(std::size_t original_size, std::size_t new_size)
*       like standard realloc should copy the content from the original address to the new address
*   -   void* reallocate(void* p, std::size_t)
*   -   void* free(void* p)
*/
//template<typename S=BufferStrategyInterface>
class ContigBufferT
{
protected:
    using S = BufferStrategyInterface;
    S&          m_strategy;
    void*       m_memPtr;     /// points to the start of the memory slab managed by the instance
    char*       m_cPtr;       /// same as memPtr but makes it easier in debugger to see whats in the buffer
    std::size_t m_length;    ///
    std::size_t m_capacity;  /// the capacity of the buffer, the value used for the malloc call
    std::size_t m_size;      /// size of the currently filled portion of the memory slab
public:
    using SPtr = std::shared_ptr<ContigBufferT>;
    static std::size_t min_buffer_size;
#if 0
    static SPtr makeSPtr(std::size_t capacity)
    {
        std::size_t sz = (capacity > ContigBufferT<S>::min_buffer_size) ? capacity : ContigBufferT<S>::min_buffer_size ;
        SPtr mbp = std::make_shared<ContigBufferT>((sz));
        return mbp;
    }
    static SPtr makeSPtr(std::string s)
    {
        SPtr mbp = std::make_shared<ContigBufferT>((s.size()));
        mbp->append((void*) s.c_str(), s.size());
        return mbp;
    }
    static SPtr makeSPtr(void* mem, std::size_t size)
    {
        SPtr mbp = std::make_shared<ContigBufferT>((size));
        mbp->append(mem, size);
        return mbp;
    }
    static SPtr makeSPtr(ContigBufferT& mb)
    {
        SPtr mbp = std::make_shared<ContigBufferT>((mb.capacity()));
        mbp->append(mb.data(), mb.size());
        return mbp;
    }
#endif
    ContigBufferT(S& strategy): m_strategy(strategy)
    {
        std::size_t tmp_cap = m_strategy.min_size();
        m_memPtr = m_strategy.allocate(tmp_cap);
        m_cPtr = (char*) m_memPtr;
        m_length = 0;
        m_size = 0;
        m_capacity = tmp_cap;
    }
    ContigBufferT(S& strategy, const std::size_t cap): m_strategy(strategy)
    {
        std::size_t tmp_cap = std::max(cap, m_strategy.min_size());
        m_memPtr = m_strategy.allocate(tmp_cap);
        m_cPtr = (char*) m_memPtr;
        m_length = 0;
        m_size = 0;
        m_capacity = tmp_cap;
    }
    ContigBufferT(S& strategy, std::string str): ContigBufferT(strategy, str.size())
    {
        this->append((void*)str.c_str(), str.size());
    }
    ContigBufferT(ContigBufferT& other): m_strategy(other.m_strategy)
    {
        m_capacity = other.m_capacity;
        m_memPtr = m_strategy.allocate(m_capacity);
        m_cPtr = (char*) m_memPtr;
        m_size = other.m_size;
        memcpy(m_memPtr, other.m_memPtr, other.m_size);
    }
    ContigBufferT& operator =(ContigBufferT& other)
    {
        if (&other == this) {
            return *this;
        }
        m_strategy = other.m_strategy;
        m_capacity = other.m_capacity;
        m_memPtr = m_strategy.allocate(m_capacity);
        m_cPtr = (char*) m_memPtr;
        m_size = other.m_size;
        memcpy(m_memPtr, other.m_memPtr, other.m_size);
        return *this;
    }
    ContigBufferT(ContigBufferT&& other): m_strategy(other.m_strategy)
    {
        m_strategy = other.m_strategy;
        m_capacity = other.m_capacity;
        m_memPtr = other.m_memPtr;
        m_cPtr = (char*) m_memPtr;
        m_size = other.m_size;
        other = ContigBufferT(m_strategy, m_capacity);
    }
    ContigBufferT& operator =(ContigBufferT&& other)
    {
        if (&other == this) {
            return *this;
        }
        m_strategy = other.m_strategy;
        m_memPtr = other.m_memPtr;
        m_cPtr = other.m_cPtr;
        m_capacity = other.m_capacity;
        m_size = other.m_size;
        other = ContigBufferT(m_strategy, m_capacity);
    }

    ~ContigBufferT()
    {
        if( (m_memPtr != nullptr) && (m_capacity > 0) ){
            m_strategy.deallocate(m_memPtr);
        }
    }
    /**
     * gets a pointer to the start of the memory slab being managed by the instance
     */
    void* data()
    {
        return m_memPtr;
    }
    /**
     * gets the size of used portion of the buffer
    */
    std::size_t size()
    {
        return m_length;
    }
    /**
     * capacity of the buffer - max value of size
    */
    std::size_t capacity()
    {
        return m_capacity;
    }
    /**
     * returns a pointer to the next available unused position in the buffer
    */
    void* nextAvailable()
    {
        return (void*) (m_cPtr + m_length);
    }
    /**
     * Resets the buffer so that it is again an empty buffer
     */
    void clear()
    {
        m_length = 0; m_cPtr[0] = (char)0;
    }

    void append(void* data, std::size_t len)
    {
        if ( ( (m_length + len) >= m_capacity )  ) {
            std::size_t new_capacity = m_strategy.reallocate_size(m_capacity, m_length + len);
            void* tmp = m_strategy.reallocate(m_memPtr, new_capacity);
            m_memPtr = tmp;
            m_cPtr = (char*) m_memPtr;
            m_capacity = new_capacity;
        }
        void* na = nextAvailable();
        memcpy(na, data, len);
        m_length = m_length + len;
        m_size = m_length;
        
        m_cPtr = (char*) m_memPtr;
    }
    void append(std::string const & str)
    {
        append((void*)str.c_str(), str.size());
    }
    void append(std::string&& str)
    {
        append((void*)str.c_str(), str.size());
    }
    void append(std::string* str)
    {
        append((void*)str->c_str(), str->size());
    }
    void setSize(std::size_t n)
    {
        m_length = n;
        m_size = n;
    }

    /**
     * Returns a string that has the same value as the used portion of the buffer
     */
    std::string toString()
    {
        char* p = m_cPtr;
        std::string s(p, m_size);
        return s;
    }

    /**
     * Detremines if an address value (pointer) is within the address range of the
     * the buffer ie
     *      buffer.dada() < = ptr < buffer.data() + buffer.capacity();
     *  or, should it be
     *      buffer.dada() < = ptr < buffer.data() + buffer.size();
     *
     */
    bool contains(void* ptr)
    {
        char* p = (char*) ptr;
        return contains(p);
    }
    bool contains(char* ptr)
    {
        char* endPtr = m_cPtr + (long)m_capacity;
        char* sPtr = m_cPtr;
    //    bool r1 = ptr <= endPtr;
    //    bool r2 = ptr >= sPtr;
        bool r = ( ptr <= endPtr && ptr >= sPtr);
        return r;
    }

    /**
     * converts an ContigBuffer to a boost::asio::const_buffer
     */
    friend boost::asio::const_buffer mb_as_const_buffer(ContigBufferT& cbt);

    /**
     * converts an ContigBuffer to a boost::asio::mutable_buffer
     */
    friend boost::asio::mutable_buffer mb_as_mutable_buffer(ContigBufferT& cbt);
//    friend boost::asio::const_buffer_1 mb_as_asio_const_buffer(ContigBuffer& mb);

    friend std::ostream &operator<< (std::ostream &os, ContigBufferT &b);

};


} //namespace Marvin
#endif
