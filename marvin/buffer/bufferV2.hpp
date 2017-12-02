#ifndef mock_buffer_v2_hpp
#define mock_buffer_v2_hpp


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
#pragma mark - MBuffer class
/**
 * MBuffer class wraps a contigous buffer an provides manipulation methods.
 * Once constructed the Mbuffer iinstance "own" the raw memory.
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

class BufferChain;
typedef std::shared_ptr<BufferChain> BufferChainSPtr;
class BufferChain
{
    public:
        BufferChain();
        void            push_back(MBufferSPtr mb);
        void            clear();
        std::vector<boost::asio::mutable_buffer> asio_buffer_sequence();
        std::size_t     size();
        std::string     to_string();
        /**
         * outputs the content to a stream
         */
        friend std::ostream &operator<< (std::ostream &os, BufferChain const &b);
    private:
        std::vector<MBufferSPtr>                    _chain;
        std::vector<boost::asio::mutable_buffer>    _asio_chain;
        std::size_t                                 _size;
};

#pragma mark - Fragment class
/**
 * A Fragment is conceptually a sub buffer of an MBuffer, it consists of a pointer
 * that lies within the address range of the memory managed by an MBuffer and a size which keeps within the
 * MBuffers memory slab.
 *
 * But the Fragment class itself does not know about the underlying MBuffer. That is reserved for
 * the  FBuffer class
 */
class Fragment {
public:
    Fragment(void* ptr, std::size_t size);
    ~Fragment();
    
    /**
     * Returns the length of the data contained in all fragments
     */
    std::size_t size();

    /**
     *  returns the address of the start of the fragment
     */
    void* start();
    void* startPointer();
    /**
     * returns the address of the end of the fragment = start + len - 1
     */
    void* endPointer();

    /**
     * addToEnd - adds the block of memory p to p + len - 1 to the end of the
     * current fragment.
     *
     * Throws error if endPointer() + 1 != p : that is if the new memory
     * is not contiguous with the existing memory.
     *
     * The caller must ensure that the full address range of the new piece
     * is within the address range of the underlying MBuffer.
     */
    void addToEnd(void* p, std::size_t len);
    
//    void extendBy(std::size_t len);

protected:
    boost::asio::mutable_buffer _asio_buf;
    void*       _ptr;
    // this is just for debugging so that the text in the buffer can be seen in xcode debugger
    char*       _cPtr;
    std::size_t _size;
};
#pragma mark - FBuffer class
/**
 * The FBuffer class manages a collection of fragments of a single MBuffer. The primary use-case
 * for this class is the parsing of incoming http message bodies. A read operation deposits
 * incoming data into an MBuffer while at the same time a parser decodes the chunked-encoding
 * of the incoming data. The decoding "removes" the chunk-encoding headers and footers
 * from the incoming data. The receiving MBuffer has the entire incoming data stream,
 * including chunk headers and foots. The chunk parsing process builds an FBuffer that
 * points into the MBuffer at only the actual data without the chunk headers and footers.
 */
class FBuffer;
typedef std::unique_ptr<FBuffer> FBufferUniquePtr;
typedef std::shared_ptr<FBuffer> FBufferSharedPtr;

class FBuffer
{
public:
    /**
     * Constructor that wraps an MBuffer - the FBuffer now "owns" the MBuffer and will free/delete
     * it in the FBuffer's destructor
     */
    FBuffer(MBuffer* mbuf);
    
    /**
     * Constructore that hides the underlying MBuffer
     */
    FBuffer(std::size_t capacity);
    
    /**
     * Destructor - will delete the managed MBuffer
     */
    ~FBuffer();
    
    /**
    * Returns the total size of all fragments
    */
    std::size_t size();
    
    /**
     * Return the underlying MBuffer
     */
    MBuffer& getMBuffer();
    /**
     * copies these bytes into the FBuffer so that they are continguous with
     * (that is added to) the last fragement (fragment with the highest starting address)
    */
    void copyIn(void* bytes, std::size_t len);
    
    /**
     * add a new fragment to the FBuffer
     * check the fragment is inside address bounds of the managed MBuffer and that
     * the new fragment is "past" (has a higher starting address) than the previously "last" fragment
     * if this new fragment is contiguous with the "last" fragment consolidate the two
     * if this new fragment is empty (len = 0) then do nothing
    */
    void addFragment(void* bytes, std::size_t len);
    
    friend std::vector<boost::asio::const_buffer> fb_as_const_buffer_sequence(FBuffer& bm);
    friend std::vector<boost::asio::mutable_buffer> fb_as_mutable_buffer_sequence(FBuffer& bm);

    friend std::ostream &operator<< (std::ostream &os, FBuffer const &b);

protected:
    MBuffer*                    _container; /// where all the fragments reside
    std::vector<Fragment>       _fragments; /// a list of fragments
    std::size_t                 _size;      /// cumulative size of all fragments
};

#endif
