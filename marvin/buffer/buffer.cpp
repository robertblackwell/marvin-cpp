
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cassert>
#include <vector>
#include "buffer.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN )
//
// Buffer class is used as a standard opaque container to pass data between the layers/objects
// Under the covers this is a slab of memory and associated indexes.
// A key feature is that down at the boost::asio level we can apply boost:;asio::buffer()
// to the memory slab to use async_read and async_write functions.
//

MBuffer::MBuffer(void* mem, std::size_t length): memPtr(mem), length_(length), cPtr((char*)mem)
{
    LogTorTrace();

}
MBuffer::~MBuffer()
{
    LogTorTrace();
    if( (memPtr != nullptr) && (capacity_ > 0) ){
        free(memPtr);
    }
}
    
MBuffer::MBuffer(std::size_t cap)
{
    memPtr = malloc(cap);
    cPtr = (char*) memPtr;
    length_ = 0;
    size_ = 0;
    capacity_ = cap;
}

void* MBuffer::data()
{
    return memPtr;
}
// size of used portion of the buffer
std::size_t MBuffer::size(){
    return length_;
}
// capacity of the buffer - max value of size
std::size_t MBuffer::capacity(){
    return capacity_;
}
// returns a pointer to the next available position in the buffer
void* MBuffer::nextAvailable(){
    return (void*) (cPtr + length_);
}

MBuffer& MBuffer::empty()
{
    length_ = 0; cPtr[0] = (char)0;
    return *this;
}

// adds (by copying) data to the end of the buffer
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

std::string MBuffer::toString(){
    std::string s;
    return s;
}
bool MBuffer::contains(void* ptr){
    char* p = (char*) ptr;
    return contains(p);
}
bool MBuffer::contains(char* ptr){
    char* endPtr = cPtr + (long)capacity_;
    char* sPtr = cPtr;
//    bool r1 = ptr <= endPtr;
//    bool r2 = ptr >= sPtr;
    bool r = ( ptr <= endPtr && ptr >= sPtr);
    return r;
}

std::ostream &operator<< (std::ostream &os, MBuffer const &b) {
    if(b.length_ == 0){
        os << "Empty ";
    }else{
        std::string s((char*)b.memPtr);
        os << "\r\nMBuffer{ length: " << b.length_ << "content: [" << s << "]}";
    }
    return os;
}
boost::asio::const_buffer asio_buffer(MBuffer& mb)
{
    return boost::asio::buffer(mb.data(), mb.size());
}



Fragment::Fragment(void* ptr, std::size_t size){
    _ptr = ptr;
    _cPtr = (char*) ptr;
    _size = size;
}
Fragment::~Fragment(){} // owns no allocated memory, so destructor has nothing to do

void* Fragment:: startPointer(){return start();}
void* Fragment::start() { return (char*)_ptr; }
std::size_t Fragment::size(){ return _size; }

// points at the last byte in the fragment
void*  Fragment::endPointer(){
    char* e = _cPtr + _size - 1;
    return (void*) e;
}
void Fragment::extendBy(std::size_t len){ _size = _size + len;}


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
        if( (char*)bytes == ((char*)last.endPointer() + 1) ){
            last.extendBy(len);
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
//    os << " size: " << sz;
    return os;
}
boost::asio::const_buffer asio_buffer(FBuffer& fb)
{
    std::vector<boost::asio::const_buffer> v;
    for(Fragment& frag: fb._fragments) {
        auto b = boost::asio::buffer(frag.start(), frag.size());
        v.push_back(b);
    }
    return boost::asio::buffer(v);
}
