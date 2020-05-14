#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <boost/asio/buffer.hpp>
#include <marvin/buffer/m_buffer.hpp>
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)
using namespace Marvin;
namespace  Marvin {
  
std::size_t MBuffer::min_buffer_size = 1000;

#pragma mark - MBuffer implementation
MBufferSPtr MBuffer::makeSPtr(std::size_t capacity)
{
    std::size_t sz = (capacity > MBuffer::min_buffer_size) ? capacity : MBuffer::min_buffer_size ;
    MBufferSPtr mbp = std::make_shared<MBuffer>((sz));
    return mbp;
}
MBufferSPtr MBuffer::makeSPtr(std::string s)
{
    MBufferSPtr mbp = std::make_shared<MBuffer>((s.size()));
    mbp->append((void*) s.c_str(), s.size());
    return mbp;
}
MBufferSPtr MBuffer::makeSPtr(void* mem, std::size_t size)
{
    MBufferSPtr mbp = std::make_shared<MBuffer>((size));
    mbp->append(mem, size);
    return mbp;
}
MBufferSPtr MBuffer::makeSPtr(MBuffer& mb)
{
    MBufferSPtr mbp = std::make_shared<MBuffer>((mb.capacity()));
    mbp->append(mb.data(), mb.size());
    return mbp;
}

MBuffer::MBuffer(std::size_t cap)
{
    std::size_t tmp_cap = (cap > MBuffer::min_buffer_size) ? cap : MBuffer::min_buffer_size ;
    m_memPtr = malloc(tmp_cap);
    m_cPtr = (char*) m_memPtr;
    m_length = 0;
    m_size = 0;
    m_capacity = tmp_cap;
}
MBuffer::MBuffer(std::string str): MBuffer(str.size())
{
    this->append((void*)str.c_str(), str.size());
}
MBuffer::MBuffer(MBuffer& other)
{
    m_capacity = other.m_capacity;
    m_memPtr = malloc(m_capacity);
    m_cPtr = (char*) m_memPtr;
    m_size = other.m_size;
    memcpy(m_memPtr, other.m_memPtr, other.m_size);
}
MBuffer& MBuffer::operator =(MBuffer& other)
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
MBuffer::MBuffer(MBuffer&& other)
{
    m_capacity = other.m_capacity;
    m_memPtr = other.m_memPtr;
    m_cPtr = (char*) m_memPtr;
    m_size = other.m_size;
    other = MBuffer(m_capacity);
}
MBuffer& MBuffer::operator =(MBuffer&& other)
{
    if (&other == this) {
        return *this;
    }
    m_memPtr = other.m_memPtr;
    m_cPtr = other.m_cPtr;
    m_capacity = other.m_capacity;
    m_size = other.m_size;
    other = MBuffer(m_capacity);
}

MBuffer::~MBuffer()
{
    if( (m_memPtr != nullptr) && (m_capacity > 0) ){
        free(m_memPtr);
    }
}

void* MBuffer::data()
{
    return m_memPtr;
}
std::size_t MBuffer::size()
{
    return m_length;
}
std::size_t MBuffer::capacity()
{
    return m_capacity;
}
void* MBuffer::nextAvailable()
{
    return (void*) (m_cPtr + m_length);
}

MBuffer& MBuffer::empty()
{
    m_length = 0; m_cPtr[0] = (char)0;
    return *this;
}

MBuffer& MBuffer::append(void* data, std::size_t len)
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
MBuffer& MBuffer::append(std::string const & str)
{
    append((void*)str.c_str(), str.size());
}
MBuffer& MBuffer::append(std::string&& str)
{
    append((void*)str.c_str(), str.size());
}
MBuffer& MBuffer::append(std::string* str)
{
    append((void*)str->c_str(), str->size());
}
//MBuffer& MBuffer::append(char const* c_str)
//{
//    std::size_t len = strlen(c_str);
//    append((void*)c_str, len+1);
//}

MBuffer& MBuffer::setSize(std::size_t n)
{
    m_length = n;
    m_size = n;
    return *this;
}

std::string MBuffer::toString()
{
    char* p = m_cPtr;

    std::string s(p, m_size);
    return s;
}

bool MBuffer::contains(void* ptr)
{
    char* p = (char*) ptr;
    return contains(p);
}
bool MBuffer::contains(char* ptr)
{
    char* endPtr = m_cPtr + (long)m_capacity;
    char* sPtr = m_cPtr;
//    bool r1 = ptr <= endPtr;
//    bool r2 = ptr >= sPtr;
    bool r = ( ptr <= endPtr && ptr >= sPtr);
    return r;
}
MBufferSPtr m_buffer(std::size_t capacity)
{
    MBufferSPtr mbp = std::make_shared<MBuffer>(capacity);
    return mbp;
}
MBufferSPtr m_buffer(std::string s)
{
    
    MBufferSPtr mbp = std::make_shared<MBuffer>(s.size());
    mbp->append((void*) s.c_str(), s.size());
    return mbp;
}
MBufferSPtr m_buffer(void* mem, std::size_t size)
{
    MBufferSPtr mbp = std::make_shared<MBuffer>((size));
    mbp->append(mem, size);
    return mbp;

}
MBufferSPtr m_buffer(MBuffer& mb)
{
    MBufferSPtr mbp = std::make_shared<MBuffer>((mb.capacity()));
    mbp->append(mb.data(), mb.size());
    return mbp;
}

#pragma mark - friend functions
std::ostream &operator<< (std::ostream &os, MBuffer &b)
{
    if(b.m_length == 0){
        os << "Empty ";
    }else{
        const std::size_t sz = b.size();
        std::string s((char*)b.m_memPtr, sz);
        os << "\r\nMBuffer{ length: " << b.m_length << "content: [" << s << "]}";
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
