#ifndef buffer_chain_hpp
#define buffer_chain_hpp
#include <stddef.h>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <marvin/buffer/contig_buffer.hpp>

namespace Marvin {

class BufferChain;
/**
* \defgroup buffers Buffers
* \brief Provides types of buffers for use insocket communication.
*/
/**
* Shared smart pointer to a BufferChain
*/
using BufferChainSPtr = std::shared_ptr<BufferChain>;
/**
 * \ingroup buffers
 * \brief BufferChain provides a concept of an expanding buffer without the need to reallocate and copy.
 *
 * BufferChain provides a means of stringing together ContigBuffers to provide a mechanism for
 * keeping the results of multiple read operations in the same place without the need to provide
 * an expanding contiguous buffer mechanism.
 *
 *
*/
class BufferChain
{
    public:
        static BufferChainSPtr makeSPtr();
        static BufferChainSPtr makeSPtr(BufferChain&& other);
        static BufferChainSPtr makeSPtr(std::string& s);
        static BufferChainSPtr makeSPtr(ContigBuffer& mb);
        static BufferChainSPtr makeSPtr(ContigBufferSPtr mb_sptr);
    
        using AsioConstBufferSeq = std::vector<boost::asio::const_buffer>;
        using AsioMutableBufferSeq = std::vector<boost::asio::mutable_buffer>; 

        BufferChain();
        /**
         * \brief Copy constructor and assignment operator perform deep copies
         * which will copy the entire buffer content. Prefer the Move constructor
         * and assignment operator.
         */
        BufferChain(BufferChain& other);
        BufferChain& operator =(BufferChain& other);
        BufferChain(BufferChain&& other);
        BufferChain& operator =(BufferChain&& other);

        /** append data - iether append to the final block or add a new block*/
        void    append(void* buf, std::size_t len);
        void    append(std::string str);
        void    append(std::string& str);
        void    push_back(ContigBufferSPtr mb);
        void    clear();
        AsioConstBufferSeq& asio_buffer_sequence();
        /** total bytes in the chain */
        std::size_t      size();
        /** number of blocks in the chain */
        std::size_t      blocks();
        ContigBuffer&    block_at(std::size_t index);
        std::string      to_string();
        ContigBufferSPtr amalgamate();
        void createAsioBufferSequence();

        friend std::ostream &operator <<(std::ostream& os, BufferChain& b);
        /**
        * converts a Bufferchain to a boost buffer for use in async write calls
        * thus need const_bufer_seq
        */
        friend AsioConstBufferSeq& buffer_chain_to_const_buffer_sequence(BufferChain& bchain);
        friend AsioConstBufferSeq& buffer_chain_to_mutable_buffer_sequence(BufferChain& bchain);
        friend std::string buffersequence_to_string(AsioConstBufferSeq& mutbufs);
        // chunk encoding helpers
        friend BufferChainSPtr chunk_buffer(BufferChainSPtr buf_sptr);
        friend BufferChainSPtr chunk_empty();
        friend BufferChainSPtr chunk_last(BufferChainSPtr buf_sptr);

    private:
        std::vector<ContigBufferSPtr>    m_chain;
        AsioConstBufferSeq          m_asio_chain;
        std::size_t                 m_size;
};
std::string buffersequence_to_string(BufferChain::AsioConstBufferSeq& mutbufs);

} // namespave Marvin
#endif
