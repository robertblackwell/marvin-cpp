
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cassert>
#include <vector>
#include "bufferV2.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN )

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
    LogTorTrace();
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
    assert( ( (length_ + len)< capacity_ )  );
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
#pragma mark - friend functions
std::ostream &operator<< (std::ostream &os, MBuffer const &b)
{
    if(b.length_ == 0){
        os << "Empty ";
    }else{
        std::string s((char*)b.memPtr);
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


#pragma mark - Fragment class
Fragment::Fragment(void* ptr, std::size_t size){
    _ptr = ptr;
    _cPtr = (char*) ptr;
    _size = size;
    _asio_buf = boost::asio::mutable_buffer(ptr, size);
}
Fragment::~Fragment(){} // owns no allocated memory, so destructor has nothing to do

void* Fragment:: startPointer()
{
    return boost::asio::buffer_cast<void*>(_asio_buf);
}
void* Fragment::start()
{
    return boost::asio::buffer_cast<void*>(_asio_buf);
}
std::size_t Fragment::size()
{
    return boost::asio::buffer_size(_asio_buf);
}

// points at the last byte in the fragment
void*  Fragment::endPointer(){
    std::size_t sz = boost::asio::buffer_size(_asio_buf);
    char* pp = boost::asio::buffer_cast<char*>(_asio_buf);
    char* e = pp + sz - 1;
    return (void*) e;
}
void Fragment::addToEnd(void* p, std::size_t len)
{
    void* cur_void_ptr = boost::asio::buffer_cast<void*>(_asio_buf);
    char* cur_ptr = boost::asio::buffer_cast<char*>(_asio_buf);
    std::size_t cur_len = boost::asio::buffer_size(_asio_buf);
    char* p2 = &cur_ptr[cur_len];
    assert( p == (cur_ptr + cur_len));
    _asio_buf = boost::asio::mutable_buffer(cur_void_ptr, len + cur_len);
}
//void Fragment::extendBy(std::size_t len)
//{
//    _size = _size + len;
//}

#pragma mark - FBuffer class
FBuffer::FBuffer(std::size_t capacity)
{
    _container = new MBuffer(capacity);
    _fragments.clear();
}
FBuffer::FBuffer(MBuffer* mbuf)
{
    assert((mbuf != nullptr));
    _container = mbuf;
    _fragments.clear();
    _size = 0;
};
    
FBuffer::~FBuffer()
{
    delete _container;
}
std::size_t FBuffer::size()
{
    return _size;
}
// copies these bytes into the FBuffer so that they are continguous with
// (that is added to) the last fragement
void FBuffer::copyIn(void* bytes, std::size_t len)
{
//    assert((this != nullptr));
    if (len == 0)
        return;
    assert((_container != nullptr));
    void* na = _container->nextAvailable();
    _container->append(bytes, len);
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
//    char* containerEndPtr = ((char*)_container->data()) + _container->capacity();
    // make sure fragment is inside container
    if(len == 0) return;
    bool startOK = _container->contains((char*)bytes);
    bool endOK   = _container->contains((char*)bytes + len) ;
    _size += len;
    if( ! startOK || !endOK ){
        assert( startOK );
        assert( endOK );
    }
    // check fragments are increasing
    if( _fragments.size() > 0 ){
        Fragment& last = _fragments.back();
        assert(len > 0);
        assert(((char*)bytes > last.endPointer()));
        if( (char*)bytes == ((char*)last.endPointer() + 1) ) {
//            last.extendBy(len);
            last.addToEnd(bytes, len);
            void* ptr = last.start();
            std::size_t tmp_l = last.size();
//            _fragments.pop_back();
//            Fragment f(ptr, tmp_l + len);
//            _fragments.push_back(f);

        }else{
            Fragment f(bytes, len);
            _fragments.push_back(f);
        }
    }else{
        Fragment f(bytes, len);
        _fragments.push_back(f);
    }
    
}

std::ostream &operator<< (std::ostream &os, FBuffer const &fb)
{
    std::size_t sz = fb._size;
    for(auto const& frag: fb._fragments) {
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
    for(Fragment& frag: fb._fragments) {
        auto b = boost::asio::buffer(frag.start(), frag.size());
        v.push_back(b);
    }
    return v;
}
std::vector<boost::asio::mutable_buffer> fb_as_mutable_buffer_sequence(FBuffer& fb)
{
    std::vector<boost::asio::mutable_buffer> v;
    for(Fragment& frag: fb._fragments) {
        auto b = boost::asio::buffer(frag.start(), frag.size());
        v.push_back(b);
    }
    return v;
}
