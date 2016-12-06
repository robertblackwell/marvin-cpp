#ifndef mock_buffer_hpp
#define mock_buffer_hpp


#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


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
        return *this;
    }
    
    std::string toString(){
        std::string s;
        return s;
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

//typedef std::function<void(MBuffer& buf)> MBufferCallback;
//
//class BodyBuffer
//{
//    
//};
//class ChunkBuffer
//{
//    
//};

#endif