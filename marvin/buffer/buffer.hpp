#ifndef mock_buffer_hpp
#define mock_buffer_hpp


#include <iostream>
#include <iterator>
#include <algorithm>
#include "boost_stuff.hpp"
//#include <boost/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <cassert>
#include <vector>
class MBuffer;

typedef std::shared_ptr<MBuffer> MBufferSPtr;
typedef std::unique_ptr<MBuffer> MBufferUPtr;

//=========================================================================================================
//
// Buffer class is used as a standard opaque container to pass data between the layers/objects
// Under the covers this is a slab of memory and associated indexes.
// A key feature is that down at the boost::asio level we can apply boost:;asio::buffer()
// to the memory slab to use async_read and async_write functions.
//
//=========================================================================================================
struct MBuffer {
public:
    MBuffer(void* mem, std::size_t length);
    ~MBuffer();
    
    MBuffer(const std::size_t cap);
    void* data();
    
    // size of used portion of the buffer
    std::size_t size();
    
    // capacity of the buffer - max value of size
    std::size_t capacity();
    
    // returns a pointer to the next available position in the buffer
    void* nextAvailable();
    
    MBuffer& empty();
    
    // adds (by copying) data to the end of the buffer
    MBuffer& append(void* data, std::size_t len);
    
    MBuffer& setSize(std::size_t n);
    
    std::string toString();
    
    bool contains(void* ptr);
    
    bool contains(char* ptr);
    
    friend std::ostream &operator<< (std::ostream &os, MBuffer const &b);
private:
    void*       memPtr;
    char*       cPtr;
    std::size_t length_;
    std::size_t capacity_;
    std::size_t size_;
};
//========================================================================================================
//
// class represents a single fragment of the buffer. Private to FBuffer
//
//========================================================================================================
class Fragment {
public:
    Fragment(void* ptr, std::size_t size);
    ~Fragment();
    
    std::size_t size();

    void* start();
    void* startPointer();
    
    // points at the last byte in the fragment
    void* endPointer();
    
    void extendBy(std::size_t len);

private:
    void*       _ptr;
    // this is just for debugging so that the text in the buffer can be seen in xcode debugger
    char*       _cPtr;
    std::size_t _size;
};


//========================================================================================================
//
//  A fragemented buffer - made up of a list of {ptr, len} tuples all pointing into the same MBuffer
//
//========================================================================================================
class FBuffer;
typedef std::unique_ptr<FBuffer> FBufferUniquePtr;
typedef std::shared_ptr<FBuffer> FBufferSharedPtr;

class FBuffer{
private:

public:
    friend std::ostream &operator<< (std::ostream &os, FBuffer const &b);

    FBuffer(std::size_t capacity);
    FBuffer(MBuffer* mbuf);
    
    ~FBuffer();
    
    // copies these bytes into the FBuffer so that they are continguous with
    // (that is added to) the last fragement
    void copyIn(void* bytes, std::size_t len);
    
    //
    // add a new fragment to the FBuffer
    // check the fragment is inside the container and that
    // the new buffer is "past" (higher address value) than the previously "last" fragment
    // if this fragment is contiguous with the "last" fragment consolidate the two
    //
    void addFragment(void* bytes, std::size_t len);
private:
    MBuffer*                _container; // where all the fragments reside
    std::vector<Fragment>   _fragments; // a list of fragments
};
#endif
