#ifndef m_buffer_hpp
#define m_buffer_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "boost_stuff.hpp"
//#include <boost/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <cassert>
#include <vector>

namespace Marvin {
class MBuffer;

using MBufferSPtr =  std::shared_ptr<MBuffer>;
using MBufferUPtr =  std::unique_ptr<MBuffer>;

#pragma mark - MBuffer class

/*!
* \brief MBuffer provides a contiguous expanding buffer
 *
 * MBuffer class wraps a contigous buffer and provides manipulation methods.
 * Once constructed the Mbuffer instance "own" the raw memory.
 * MBuffer destructor releases the raw memory.
 */
struct MBuffer {
public:
    /**
     * Constructor - give it a slab of memory to manage
     * Let the MBuffer constructor allocate the memory - but tell it howmuch
     */
     MBuffer(const std::size_t cap);
    
    /**
     * destrtuctor - frees the memory the instance is managing
     */
    ~MBuffer();
    
    /**
     * gets a pointer to the start of the memory slab being managed by the instance
     */
    void* data();
    
    /**
     * gets the size of used portion of the buffer
    */
     std::size_t size();
    
    /**
     * capacity of the buffer - max value of size
    */
    std::size_t capacity();
    
    /**
     * returns a pointer to the next available unused position in the buffer
    */
    void* nextAvailable();
    
    /**
     * Resets the buffer so that it is again an empty buffer
     */
    MBuffer& empty();
    
    /**
     *  adds (by copying) data to the buffer starting at the first unsed byte
    */
    MBuffer& append(void* data, std::size_t len);
    
    MBuffer& setSize(std::size_t n);
    
    /**
     * Returns a string that has the same value as the used portion of the buffer
     */
    std::string toString();
    
    /**
     * Detremines if an address value (pointer) is within the address range of the
     * the buffer ie
     *      buffer.dada() < = ptr < buffer.data() + buffer.capacity();
     *  or, should it be
     *      buffer.dada() < = ptr < buffer.data() + buffer.size();
     *
     */
    bool contains(void* ptr);
    bool contains(char* ptr);
    
    /**
     * converts an MBuffer to a boost::asio::const_buffer
     */
    friend boost::asio::const_buffer mb_as_const_buffer(MBuffer& bm);

    /**
     * converts an MBuffer to a boost::asio::mutable_buffer
     */
    friend boost::asio::mutable_buffer mb_as_mutable_buffer(MBuffer& bm);
//    friend boost::asio::const_buffer_1 mb_as_asio_const_buffer(MBuffer& mb);

    /**
     * outputs the content to a stream
     */
    friend std::ostream &operator<< (std::ostream &os, MBuffer &b);

protected:
    
    void*       memPtr;     /// points to the start of the memory slab managed by the instance
    char*       cPtr;       /// same as memPtr but makes it easier in debugger to see whats in the buffer
    std::size_t length_;    ///
    std::size_t capacity_;  /// the capacity of the buffer, the value used for the malloc call
    std::size_t size_;      /// size of the currently filled portion of the memory slab
};

MBufferSPtr m_buffer(std::size_t capacity);
MBufferSPtr m_buffer(std::string s);
MBufferSPtr m_buffer(void* mem, std::size_t size);
MBufferSPtr m_buffer(MBuffer& mb);

} //namespace Marvin
#endif
