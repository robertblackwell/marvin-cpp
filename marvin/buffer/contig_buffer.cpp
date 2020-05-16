#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <boost/asio/buffer.hpp>
#include <marvin/buffer/contig_buffer.hpp>
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)
using namespace Marvin;
namespace  Marvin {
  
std::size_t ContigBuffer::min_buffer_size = 1000;

#pragma mark - ContigBuffer implementation
ContigBufferSPtr ContigBuffer::makeSPtr(std::size_t capacity)
{
    std::size_t sz = (capacity > ContigBuffer::min_buffer_size) ? capacity : ContigBuffer::min_buffer_size ;
    ContigBufferSPtr mbp = std::make_shared<ContigBuffer>((sz));
    return mbp;
}
ContigBufferSPtr ContigBuffer::makeSPtr(std::string s)
{
    ContigBufferSPtr mbp = std::make_shared<ContigBuffer>((s.size()));
    mbp->append((void*) s.c_str(), s.size());
    return mbp;
}
ContigBufferSPtr ContigBuffer::makeSPtr(void* mem, std::size_t size)
{
    ContigBufferSPtr mbp = std::make_shared<ContigBuffer>((size));
    mbp->append(mem, size);
    return mbp;
}
ContigBufferSPtr ContigBuffer::makeSPtr(ContigBuffer& mb)
{
    ContigBufferSPtr mbp = std::make_shared<ContigBuffer>((mb.capacity()));
    mbp->append(mb.data(), mb.size());
    return mbp;
}

ContigBuffer::ContigBuffer(std::size_t cap)
{
    std::size_t tmp_cap = (cap > ContigBuffer::min_buffer_size) ? cap : ContigBuffer::min_buffer_size ;
    m_memPtr = malloc(tmp_cap);
    m_cPtr = (char*) m_memPtr;
    m_length = 0;
    m_size = 0;
    m_capacity = tmp_cap;
}
ContigBuffer::ContigBuffer(std::string str): ContigBuffer(str.size())
{
    this->append((void*)str.c_str(), str.size());
}
ContigBuffer::ContigBuffer(ContigBuffer& other)
{
    m_capacity = other.m_capacity;
    m_memPtr = malloc(m_capacity);
    m_cPtr = (char*) m_memPtr;
    m_size = other.m_size;
    memcpy(m_memPtr, other.m_memPtr, other.m_size);
}
ContigBuffer& ContigBuffer::operator =(ContigBuffer& other)
{
    if (&other == this) {
        return *this;
    }
    m_capacity = other.m_capacity;
    m_memPtr = malloc(m_capacity);
    m_cPtr = (char*) m_memPtr;
    m_size = other.m_size;
    memcpy(m_memPtr, other.m_memPtr, other.m_size);
    return *this;
}
ContigBuffer::ContigBuffer(ContigBuffer&& other)
{
    m_capacity = other.m_capacity;
    m_memPtr = other.m_memPtr;
    m_cPtr = (char*) m_memPtr;
    m_size = other.m_size;
    other = ContigBuffer(m_capacity);
}
ContigBuffer& ContigBuffer::operator =(ContigBuffer&& other)
{
    if (&other == this) {
        return *this;
    }
    m_memPtr = other.m_memPtr;
    m_cPtr = other.m_cPtr;
    m_capacity = other.m_capacity;
    m_size = other.m_size;
    other = ContigBuffer(m_capacity);
}

ContigBuffer::~ContigBuffer()
{
    if( (m_memPtr != nullptr) && (m_capacity > 0) ){
        free(m_memPtr);
    }
}

void* ContigBuffer::data()
{
    return m_memPtr;
}
std::size_t ContigBuffer::size()
{
    return m_length;
}
std::size_t ContigBuffer::capacity()
{
    return m_capacity;
}
void* ContigBuffer::nextAvailable()
{
    return (void*) (m_cPtr + m_length);
}

ContigBuffer& ContigBuffer::empty()
{
    m_length = 0; m_cPtr[0] = (char)0;
    return *this;
}

ContigBuffer& ContigBuffer::append(void* data, std::size_t len)
{
    if ( ( (m_length + len) >= m_capacity )  ) {
        std::size_t new_capacity = m_capacity * 2;
        void* tmp = realloc(m_memPtr, m_capacity*2);
        m_memPtr = tmp;
        m_cPtr = (char*) m_memPtr;
        m_capacity = new_capacity;
    }
    void* na = nextAvailable();
    
    memcpy(na, data, len);
    m_length = m_length + len;
    m_size = m_length;
    
    m_cPtr = (char*) m_memPtr;
    return *this;
}
ContigBuffer& ContigBuffer::append(std::string const & str)
{
    append((void*)str.c_str(), str.size());
}
ContigBuffer& ContigBuffer::append(std::string&& str)
{
    append((void*)str.c_str(), str.size());
}
ContigBuffer& ContigBuffer::append(std::string* str)
{
    append((void*)str->c_str(), str->size());
}
//ContigBuffer& ContigBuffer::append(char const* c_str)
//{
//    std::size_t len = strlen(c_str);
//    append((void*)c_str, len+1);
//}

ContigBuffer& ContigBuffer::setSize(std::size_t n)
{
    m_length = n;
    m_size = n;
    return *this;
}

std::string ContigBuffer::toString()
{
    char* p = m_cPtr;

    std::string s(p, m_size);
    return s;
}

bool ContigBuffer::contains(void* ptr)
{
    char* p = (char*) ptr;
    return contains(p);
}
bool ContigBuffer::contains(char* ptr)
{
    char* endPtr = m_cPtr + (long)m_capacity;
    char* sPtr = m_cPtr;
//    bool r1 = ptr <= endPtr;
//    bool r2 = ptr >= sPtr;
    bool r = ( ptr <= endPtr && ptr >= sPtr);
    return r;
}
ContigBufferSPtr m_buffer(std::size_t capacity)
{
    ContigBufferSPtr mbp = std::make_shared<ContigBuffer>(capacity);
    return mbp;
}
ContigBufferSPtr m_buffer(std::string s)
{
    
    ContigBufferSPtr mbp = std::make_shared<ContigBuffer>(s.size());
    mbp->append((void*) s.c_str(), s.size());
    return mbp;
}
ContigBufferSPtr m_buffer(void* mem, std::size_t size)
{
    ContigBufferSPtr mbp = std::make_shared<ContigBuffer>((size));
    mbp->append(mem, size);
    return mbp;

}
ContigBufferSPtr m_buffer(ContigBuffer& mb)
{
    ContigBufferSPtr mbp = std::make_shared<ContigBuffer>((mb.capacity()));
    mbp->append(mb.data(), mb.size());
    return mbp;
}

#pragma mark - friend functions
std::ostream &operator<< (std::ostream &os, ContigBuffer &b)
{
    if(b.m_length == 0){
        os << "Empty ";
    }else{
        const std::size_t sz = b.size();
        std::string s((char*)b.m_memPtr, sz);
        os << "\r\nContigBuffer{ length: " << b.m_length << "content: [" << s << "]}";
    }
    return os;
}
boost::asio::const_buffer mb_as_const_buffer(ContigBuffer& mb)
{
    return boost::asio::const_buffer(mb.data(), mb.size());
}
boost::asio::mutable_buffer mb_as_mutable_buffer(ContigBuffer& mb)
{
    return boost::asio::mutable_buffer(mb.data(), mb.size());
}
} //namespace Marvin
