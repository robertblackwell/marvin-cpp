#ifndef marvin_contig_buffer_hpp
#define marvin_contig_buffer_hpp
#include <stddef.h>
#include <memory>
#include <iostream>
#include <iterator>
#include <boost/asio/buffer.hpp>
#include <marvin/error_handler/error_handler.hpp>
//#include <marvin/buffer/buffer_allocator.hpp>
namespace Marvin {

class ContigBuffer;
using ContigBufferSPtr =  std::shared_ptr<ContigBuffer>;
using ContigBufferUPtr =  std::unique_ptr<ContigBuffer>;

#pragma mark - ContigBuffer class

/**
*
* \ingroup buffers
* \brief ContigBuffer provides a contiguous expanding buffer
*
* ContigBuffer class wraps a contigous buffer and provides manipulation methods.
* Once constructed the Mbuffer instance "own" the raw memory.
* ContigBuffer destructor releases the raw memory.
*/

class ContigBuffer
{
public:
    static std::size_t min_buffer_size;
    static ContigBufferSPtr makeSPtr(std::size_t capacity);
    static ContigBufferSPtr makeSPtr(std::string s);
    static ContigBufferSPtr makeSPtr(void* mem, std::size_t size);
    static ContigBufferSPtr makeSPtr(ContigBuffer& mb);

    /**
     * Constructor - give it a slab of memory to manage
     * Let the ContigBuffer constructor allocate the memory - but tell it howmuch
     */
    ContigBuffer(const std::size_t cap);
    ContigBuffer(std::string str);
    ContigBuffer(ContigBuffer& other);
    ContigBuffer& operator =(ContigBuffer& other);
    ContigBuffer(ContigBuffer&& other);
    ContigBuffer& operator =(ContigBuffer&& other);

    /**
     * destrtuctor - frees the memory the instance is managing
     */
    ~ContigBuffer();

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
    ContigBuffer& empty();

    /** adds (by copying) data to the buffer starting at the first unsed byte*/
    ContigBuffer& append(void* data, std::size_t len);
    /**
     * convenience overloads
     */
    ContigBuffer& append(std::string* str);
    /**
     * for constants and variables
     */
    ContigBuffer& append(std::string const& str);
    /**
     * for literals
     */
    ContigBuffer& append(std::string&& str);

    ContigBuffer& setSize(std::size_t n);

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
     * converts an ContigBuffer to a boost::asio::const_buffer
     */
    friend boost::asio::const_buffer mb_as_const_buffer(ContigBuffer& bm);

    /**
     * converts an ContigBuffer to a boost::asio::mutable_buffer
     */
    friend boost::asio::mutable_buffer mb_as_mutable_buffer(ContigBuffer& bm);
//    friend boost::asio::const_buffer_1 mb_as_asio_const_buffer(ContigBuffer& mb);

    friend std::ostream &operator<< (std::ostream &os, ContigBuffer &b);

protected:

    void*       m_memPtr;     /// points to the start of the memory slab managed by the instance
    char*       m_cPtr;       /// same as memPtr but makes it easier in debugger to see whats in the buffer
    std::size_t m_length;    ///
    std::size_t m_capacity;  /// the capacity of the buffer, the value used for the malloc call
    std::size_t m_size;      /// size of the currently filled portion of the memory slab
};

} //namespace Marvin
#endif
