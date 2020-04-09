
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cassert>
#include <vector>
#include <marvin/buffer/buffer.hpp>
#include <marvin/external_src/trog/trog.hpp>
Trog_SETLEVEL(LOG_LEVEL_WARN )

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
#pragma mark - BufferChain
BufferChain::BufferChain()
{
    m_chain = std::vector<MBufferSPtr>();
    m_size = 0;
}
void BufferChain::push_back(MBufferSPtr mb)
{
    m_size += mb->size();
    m_chain.push_back(mb);
    m_asio_chain.push_back(boost::asio::buffer(mb->data(), mb->size()));
}
void BufferChain::clear()
{
    m_chain.clear();
    m_asio_chain.clear();
    m_size = 0;
}
std::size_t BufferChain::size()
{
    return m_size;
}
std::string BufferChain::to_string()
{
    std::string s = "";
    for(MBufferSPtr& mb : m_chain) {
        s += mb->toString();
    }
    return s;
}
MBufferSPtr BufferChain::amalgamate()
{
    MBufferSPtr mb_final = std::shared_ptr<MBuffer>(new MBuffer(this->size()));
    for(MBufferSPtr& mb : m_chain) {
        mb_final->append(mb->data(), mb->size());
    }
    return mb_final;
}

BufferChainSPtr buffer_chain(std::string& s)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(m_buffer(s));
    return sp;
}
BufferChainSPtr buffer_chain(MBuffer& mb)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(m_buffer(mb));
    return sp;
}
BufferChainSPtr buffer_chain(MBufferSPtr mb_sptr)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(mb_sptr);
    return sp;
}

std::vector<boost::asio::mutable_buffer> BufferChain::asio_buffer_sequence()
{
    return this->_asio_chain;
}

std::vector<boost::asio::const_buffer> buffer_chain_to_const_buffer_sequence(BufferChain& bchain)
{
    assert(false);
//    return bchain._asio_chain;
}
std::vector<boost::asio::mutable_buffer> buffer_chain_to_mutable_buffer_sequence(BufferChain& bchain)
{
    return bchain._asio_chain;
}


std::ostream &operator<< (std::ostream &os, BufferChain &b)
{
    if(b.size() == 0){
        os << "Empty ";
    }else{
        std::string s = b.to_string();
        os << std::endl << "BufferChain{ length: " << b.size() << "content: [" << s << "]}";
    }
    return os;
}

#pragma mark - Fragment class
Fragment::Fragment(void* ptr, std::size_t size){
    m_ptr = ptr;
    m_cPtr = (char*) ptr;
    m_size = size;
    m_asio_buf = boost::asio::mutable_buffer(ptr, size);
}
Fragment::~Fragment(){} // owns no allocated memory, so destructor has nothing to do

void* Fragment:: startPointer()
{
    return boost::asio::buffer_cast<void*>(m_asio_buf);
}
void* Fragment::start()
{
    return boost::asio::buffer_cast<void*>(m_asio_buf);
}
std::size_t Fragment::size()
{
    return boost::asio::buffer_size(m_asio_buf);
}

// points at the last byte in the fragment
void*  Fragment::endPointer(){
    std::size_t sz = boost::asio::buffer_size(m_asio_buf);
    char* pp = boost::asio::buffer_cast<char*>(m_asio_buf);
    char* e = pp + sz - 1;
    return (void*) e;
}
void Fragment::addToEnd(void* p, std::size_t len)
{
    void* cur_void_ptr = boost::asio::buffer_cast<void*>(m_asio_buf);
    char* cur_ptr = boost::asio::buffer_cast<char*>(m_asio_buf);
    std::size_t cur_len = boost::asio::buffer_size(m_asio_buf);
//    char* p2 = &cur_ptr[cur_len];
    assert( p == (cur_ptr + cur_len));
    m_asio_buf = boost::asio::mutable_buffer(cur_void_ptr, len + cur_len);
}
//void Fragment::extendBy(std::size_t len)
//{
//   m_size = m_size + len;
//}

#pragma mark - FBuffer class
FBuffer::FBuffer(std::size_t capacity)
{
    m_container = new MBuffer(capacity);
    m_fragments.clear();
}
FBuffer::FBuffer(MBuffer* mbuf)
{
    assert((mbuf != nullptr));
    m_container = mbuf;
    m_fragments.clear();
    m_size = 0;
};
    
FBuffer::~FBuffer()
{
    delete m_container;
}
std::size_t FBuffer::size()
{
    return m_size;
}
// copies these bytes into the FBuffer so that they are continguous with
// (that is added to) the last fragement
void FBuffer::copyIn(void* bytes, std::size_t len)
{
//    assert((this != nullptr));
    if (len == 0)
        return;
    assert((m_container != nullptr));
    void* na = m_container->nextAvailable();
    m_container->append(bytes, len);
    addFragment(na, len);
}

//
// add a new fragment to the FBuffer
// check the fragment is inside the container and that
// the new buffer is "past" (higher address value) than the previously "last" fragment
// if this fragment is contiguous with the "last" fragment consolidate the two
//
void FBuffer::addFragment(void* bytes, std::size_t len)
{
//    char* containerPtr = (char*)_container->data();
//    char* containerEndPtr = ((char*)_container->data()) + m_container->capacity();
    // make sure fragment is inside container
    if(len == 0) return;
    bool startOK = m_container->contains((char*)bytes);
    bool endOK   = m_container->contains((char*)bytes + len) ;
    m_size += len;
    if( ! startOK || !endOK ){
        assert( startOK );
        assert( endOK );
    }
    // check fragments are increasing
    if( m_fragments.size() > 0 ){
        Fragment& last = m_fragments.back();
        assert(len > 0);
        assert(((char*)bytes > last.endPointer()));
        if( (char*)bytes == ((char*)last.endPointer() + 1) ) {
//            last.extendBy(len);
            last.addToEnd(bytes, len);
//            void* ptr = last.start();
//            std::size_t tmp_l = last.size();
//            m_fragments.pop_back();
//            Fragment f(ptr, tmp_l + len);
//            m_fragments.push_back(f);

        } else {
            Fragment f(bytes, len);
            m_fragments.push_back(f);
        }
    } else {
        Fragment f(bytes, len);
        m_fragments.push_back(f);
    }
    
}

std::ostream &operator<< (std::ostream &os, FBuffer const &fb)
{
    std::size_t sz = fb._size;
    for(auto const& frag: fb.m_fragments) {
        Fragment fg = frag;
//        char* st = (char*)fg.startPointer();
//        std::size_t  sz = fg.size();
        std::string s((char*)fg.startPointer(), fg.size());
        os << std::string((char*)fg.startPointer(), fg.size());
    }
    os << " size: " << sz;
    return os;
}
std::vector<boost::asio::const_buffer> fb_as_const_buffer_sequence(FBuffer& fb)
{
    std::vector<boost::asio::const_buffer> v;
    for(Fragment& frag: fb.m_fragments) {
        auto b = boost::asio::buffer(frag.start(), frag.size());
        v.push_back(b);
    }
    return v;
}
std::vector<boost::asio::mutable_buffer> fb_as_mutable_buffer_sequence(FBuffer& fb)
{
    std::vector<boost::asio::mutable_buffer> v;
    for(Fragment& frag: fb.m_fragments) {
        auto b = boost::asio::buffer(frag.start(), frag.size());
        v.push_back(b);
    }
    return v;
}
