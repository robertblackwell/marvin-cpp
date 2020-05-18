#ifndef marvin_buffer_allocator_hpp
#define marvin_buffer_allocator_hpp
#include <stddef.h>
#include <memory>
#include <iostream>
#include <iterator>
#include <boost/asio/buffer.hpp>
#include <marvin/error_handler/error_handler.hpp>

namespace Marvin {

class BufferStrategyInterface
{
public:

    virtual void* allocate(std::size_t size) = 0;
    virtual std::size_t max_size() = 0;
    virtual std::size_t min_size() = 0;
    virtual std::size_t reallocate_size(std::size_t original_size, std::size_t new_size) = 0;
    virtual void* reallocate(void* p, std::size_t new_size) = 0;
    virtual void deallocate(void* p) = 0;
};

// the default buffer allocator
class BufferMallocator: public BufferStrategyInterface
{
protected:
    std::size_t m_min_size;
    std::size_t m_max_size;
public:

    BufferMallocator(std::size_t min_size = 256, std::size_t max_size = 1024*1024)
    : m_min_size(min_size), m_max_size(max_size)
    {}
    void* allocate(std::size_t size) override
    {
        if (size > m_max_size) MARVIN_THROW("requested allocation too big");
        return malloc(std::max(size, m_min_size));
    }
    std::size_t max_size() override
    {
        return m_max_size;
    }
    std::size_t min_size() override
    {
        return m_min_size;
    }

    std::size_t reallocate_size(std::size_t original_size, std::size_t new_size) override
    {
        if (new_size > m_max_size) MARVIN_THROW("requested allocation too big");
        return std::max(new_size, std::min(m_max_size, 2*original_size));
    }
    void* reallocate(void* p, std::size_t new_size) override
    {
        if (new_size > m_max_size) MARVIN_THROW("requested allocation too big");
        realloc(p, new_size);
    }
    void deallocate(void* p) override
    {
        free(p);
    }
};
class HeaderAllocator: public BufferMallocator
{
    public:
    HeaderAllocator(): BufferMallocator(1024*8)
    {}
    std::size_t reallocate_size(std::size_t original_size, std::size_t new_size) override
    {
        if (new_size > m_max_size) MARVIN_THROW("requested allocation too big");
        return std::max(new_size, std::min(m_max_size, 2*original_size));
    }

};
class BodyAllocator: public BufferMallocator
{
public:
    BodyAllocator(): BufferMallocator(1024*32)
    {}
    std::size_t reallocate_size(std::size_t original_size, std::size_t new_size) override
    {
        if (new_size > m_max_size) MARVIN_THROW("requested allocation too big");
        return std::max(new_size, std::min(m_max_size, 2*original_size));
    }

};
class TunnelAllocator: public BufferMallocator
{
public:
    BodyAllocator(): BufferMallocator(1024*128)
    {}
    std::size_t reallocate_size(std::size_t original_size, std::size_t new_size) override
    {
        if (new_size > m_max_size) MARVIN_THROW("requested allocation too big");
        return std::max(new_size, std::min(m_max_size, 2*original_size));
    }

};
} //namespace Marvin
#endif
