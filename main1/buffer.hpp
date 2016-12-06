#ifndef mock_buffer_hpp
#define mock_buffer_hpp


#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cassert>
#include <vector>

//
// Buffer class is used as a standard opaque container to pass data between the layers/objects
// Under the covers this is a slab of memory and associated indexes.
// A key feature is that down at the boost::asio level we can apply boost:;asio::buffer()
// to the memory slab to use async_read and async_write functions.
//

struct MBuffer {
public:
    MBuffer(void* mem, std::size_t length): memPtr(mem), length_(length), cPtr((char*)mem)
    {
        
    }
    ~MBuffer()
    {
        if( (memPtr != nullptr) && (capacity_ > 0) ){
            free(memPtr);
        }
    }
    
    MBuffer(std::size_t cap)
    {
        memPtr = malloc(cap);
        cPtr = (char*) memPtr;
        length_ = 0;
        size_ = 0;
        capacity_ = cap;
    }
    void*   data()
    {
        return memPtr;
    }
    // size of used portion of the buffer
    std::size_t size(){
        return length_;
    }
    // capacity of the buffer - max value of size
    std::size_t capacity(){
        return capacity_;
    }
    
    MBuffer& empty()
    {
        length_ = 0; cPtr[0] = (char)0;
        return *this;
    }
    MBuffer& append(void* data, std::size_t len)
    {
        memcpy(memPtr, data, len);
        length_ = len;
        cPtr = (char*) memPtr;
        return *this;
    }
    MBuffer& setSize(std::size_t n)
    {
        length_ = n;
        size_ = n;
        return *this;
    }
    
    std::string toString(){
        std::string s;
        return s;
    }
    bool contains(void* ptr){
        char* p = (char*) ptr;
        return contains(p);
    }
    bool contains(char* ptr){
        char* endPtr = cPtr + (long)capacity_;
        char* sPtr = cPtr;
        bool r1 = ptr <= endPtr;
        bool r2 = ptr >= sPtr;
        bool r = ( ptr <= endPtr && ptr >= sPtr);
        return r;
    }
    friend std::ostream &operator<< (std::ostream &os, MBuffer const &b);
private:
    void*       memPtr;
    char*       cPtr;
    std::size_t length_;
    std::size_t capacity_;
    std::size_t size_;
};

MBuffer mbuffer(void* mem, std::size_t len){
    MBuffer mb{mem, len};
    return mb;
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

//class Fragment {
//public:
//    Fragment(void* ptr, std::size_t size){
//        _ptr = ptr;
//        _cPtr = (char*) ptr;
//        _size = size;
//    }
//    void*  endPointer(){
//        char* e = _cPtr + _size - 1;
//        return (void*) e;
//    }
//private:
//    void*   _ptr;
//    
//    // this is just for debugging so that the text in the buffer can be seen in xcode debugger
//    char*   _cPtr;
//    
//    std::size_t _size;
//};

#define FGFGF
#ifdef FGFGF
class FBuffer;
typedef std::unique_ptr<FBuffer> FBufferUniquePtr;

//
//  A fragemented buffer - made up of a list of {ptr, len} tuples all pointing into the same MBuffer
//
class FBuffer{
private:
    //
    // class represents a single fragment of the buffer. Private to FBuffer
    //
    class Fragment {
    public:
        Fragment(void* ptr, std::size_t size){
            _ptr = ptr;
            _cPtr = (char*) ptr;
            _size = size;
        }
        ~Fragment(){} // owns no allocated memory, so destructor has nothing to do
        
        // points at the last byte in the fragment
        void*  endPointer(){
            char* e = _cPtr + _size - 1;
            return (void*) e;
        }
        void extendBy(std::size_t len){
            _size = _size + len;
        }
    private:
        void*   _ptr;
        
        // this is just for debugging so that the text in the buffer can be seen in xcode debugger
        char*   _cPtr;
        
        std::size_t _size;
    };

public:
    
    FBuffer(MBuffer* mbuf)
    {
        _container = mbuf;
        _fragments.clear();
    };
    
    ~FBuffer(){}
    
    //
    // add a new fragment to the FBuffer
    // check the fragment is inside the container and that
    // the new buffer is "past" (higher address value) than the previously "last" fragment
    // if this fragment is contiguous with the "last" fragment consolidate the two
    //
    void append(void* bytes, std::size_t len)
    {
        char* containerPtr = (char*)_container->data();
        char* containerEndPtr = ((char*)_container->data()) + _container->capacity();
        // make sure fragment is inside container
        bool startOK = _container->contains((char*)bytes);
        bool endOK   = _container->contains((char*)bytes + len) ;
        assert( startOK );
        assert( endOK );
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
private:
    MBuffer*                _container; // where all the fragments reside
    std::vector<Fragment>   _fragments; // a list of fragments
};
#endif
        
#endif