#ifndef marvin_buffer_allocator_hpp
#define marvin_buffer_allocator_hpp
#include <stddef.h>
#include <memory>
#include <iostream>
#include <iterator>
#include <boost/asio/buffer.hpp>
#include <marvin/error_handler/error_handler.hpp>

namespace Marvin {

// the default buffer allocator
class BufferMallocator
{
protected:
    std::size_t m_min_size;
    std::size_t m_max_size;
public:
    BufferMallocator(std::size_t min_size = 256, std::size_t max_size = 1024*1024)
    : m_min_size(min_size), m_max_size(max_size)
    {}
    void* allocate(std::size_t size)
    {
        if (size > m_max_size) MARVIN_THROW("requested allocation too big");
        return malloc(std::max(size, m_min_size));
    }
    std::size_t max_size()
    {
        return m_max_size;
    }
    std::size_t min_size()
    {
        return m_min_size;
    }

    std::size_t reallocate_size(std::size_t original_size, std::size_t new_size)
    {
        if (new_size > m_max_size) MARVIN_THROW("requested allocation too big");
        return std::max(new_size, std::min(m_max_size, 2*original_size));
    }
    void* reallocate(void* p, std::size_t new_size)
    {
        if (new_size > m_max_size) MARVIN_THROW("requested allocation too big");
        realloc(p, new_size);
    }
    void free(void* p)
    {
        free(p);
    }
};
class HeaderAllocator: private BufferMallocator
{
    HeaderAllocator(): BufferMallocator(1024*8)
    {}
    void* allocate(std::size_t size)
    {
        return BufferMallocator::allocate(size);
    }
    std::size_t reallocate_size(std::size_t original_size, std::size_t new_size)
    {
        if (new_size > m_max_size) MARVIN_THROW("requested allocation too big");
        return std::max(new_size, std::min(m_max_size, 2*original_size));
    }
    void* reallocate(void* p, std::size_t new_size)
    {
        BufferMallocator::reallocate(p, new_size);
    }
    void free(void* p)
    {
        BufferMallocator::free(p);
    }
};
class BodyAllocator: private BufferMallocator
{
    BodyAllocator(): BufferMallocator(1024*32)
    {}
    void* allocate(std::size_t size)
    {
        return BufferMallocator::allocate(size);
    }
    std::size_t reallocate_size(std::size_t original_size, std::size_t new_size)
    {
        if (new_size > m_max_size) MARVIN_THROW("requested allocation too big");
        return std::max(new_size, std::min(m_max_size, 2*original_size));
    }
    void* reallocate(void* p, std::size_t new_size)
    {
        BufferMallocator::reallocate(p, new_size);
    }
    void free(void* p)
    {
        BufferMallocator::free(p);
    }
};
} //namespace Marvin
#endif
