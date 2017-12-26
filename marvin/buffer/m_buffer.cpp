
// #include <iostream>
// #include <iterator>
// #include <algorithm>
// #include <stdexcept>
// #include <boost/asio.hpp>
// #include <boost/bind.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>
// #include <cassert>
// #include <vector>
#include "buffer.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN )

using namespace Marvin;
namespace  Marvin {
  

#pragma mark - MBuffer implementation

MBuffer::MBuffer(std::size_t cap)
{
    memPtr = malloc(cap);
    cPtr = (char*) memPtr;
    length_ = 0;
    size_ = 0;
    capacity_ = cap;
}

MBuffer::~MBuffer()
{
    if( (memPtr != nullptr) && (capacity_ > 0) ){
        free(memPtr);
    }
}

void* MBuffer::data()
{
    return memPtr;
}
std::size_t MBuffer::size()
{
    return length_;
}
std::size_t MBuffer::capacity()
{
    return capacity_;
}
void* MBuffer::nextAvailable()
{
    return (void*) (cPtr + length_);
}

MBuffer& MBuffer::empty()
{
    length_ = 0; cPtr[0] = (char)0;
    return *this;
}

MBuffer& MBuffer::append(void* data, std::size_t len)
{
    assert( ( (length_ + len) <= capacity_ )  );
    void* na = nextAvailable();
    
    memcpy(na, data, len);
    length_ = length_ + len;
    size_ = length_;
    
    cPtr = (char*) memPtr;
    return *this;
}
MBuffer& MBuffer::setSize(std::size_t n)
{
    length_ = n;
    size_ = n;
    return *this;
}

std::string MBuffer::toString()
{
    std::string s((char*)this->data(), this->size());
    return s;
}

bool MBuffer::contains(void* ptr)
{
    char* p = (char*) ptr;
    return contains(p);
}
bool MBuffer::contains(char* ptr)
{
    char* endPtr = cPtr + (long)capacity_;
    char* sPtr = cPtr;
//    bool r1 = ptr <= endPtr;
//    bool r2 = ptr >= sPtr;
    bool r = ( ptr <= endPtr && ptr >= sPtr);
    return r;
}
MBufferSPtr m_buffer(std::size_t capacity)
{
    MBufferSPtr mbp = std::shared_ptr<MBuffer>(new MBuffer(capacity));
    return mbp;
}
MBufferSPtr m_buffer(std::string s)
{
    MBufferSPtr mbp = std::shared_ptr<MBuffer>(new MBuffer(s.size()));
    mbp->append((void*) s.c_str(), s.size());
    return mbp;
}
MBufferSPtr m_buffer(void* mem, std::size_t size)
{
    MBufferSPtr mbp = std::shared_ptr<MBuffer>(new MBuffer(size));
    mbp->append(mem, size);
    return mbp;

}
MBufferSPtr m_buffer(MBuffer& mb)
{
    MBufferSPtr mbp = std::shared_ptr<MBuffer>(new MBuffer(mb.capacity()));
    mbp->append(mb.data(), mb.size());
    return mbp;

}

#pragma mark - friend functions
std::ostream &operator<< (std::ostream &os, MBuffer &b)
{
    if(b.length_ == 0){
        os << "Empty ";
    }else{
        const std::size_t sz = b.size();
        std::string s((char*)b.memPtr, sz);
        os << "\r\nMBuffer{ length: " << b.length_ << "content: [" << s << "]}";
    }
    return os;
}
boost::asio::const_buffer mb_as_const_buffer(MBuffer& mb)
{
    return boost::asio::const_buffer(mb.data(), mb.size());
}
boost::asio::mutable_buffer mb_as_mutable_buffer(MBuffer& mb)
{
    return boost::asio::mutable_buffer(mb.data(), mb.size());
}
} //namespace Marvin
