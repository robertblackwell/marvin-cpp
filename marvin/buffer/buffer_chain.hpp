#ifndef buffer_chain_hpp
#define buffer_chain_hpp
#include <stddef.h>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <marvin/buffer/m_buffer.hpp>

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
* BufferChain provides a means of stringing together MBuffers to provide a mechanism for
* keeping the results of multiple read operations in the same place without the need to provide
* an expanding contiguous buffer mechanism.
*/
class BufferChain
{
    public:
        static BufferChainSPtr makeSPtr(std::string& s);
        static BufferChainSPtr makeSPtr(MBuffer& mb);
        static BufferChainSPtr makeSPtr(MBufferSPtr mb_sptr);
    
        BufferChain();
        void            append(void* buf, std::size_t len);
        void            push_back(MBufferSPtr mb);
        void            clear();
        std::vector<boost::asio::mutable_buffer> asio_buffer_sequence();
        std::size_t     size();
        std::string     to_string();
        MBufferSPtr     amalgamate();

        /**
         * outputs the content to a stream
         */
        friend std::ostream &operator<< (std::ostream &os, BufferChain const &b);
        /**
        * converts a Bufferchain to a boost buffer for use in async io calls
        */
        friend std::vector<boost::asio::const_buffer> buffer_chain_to_const_buffer_sequence(BufferChain& bchain);
        friend std::vector<boost::asio::mutable_buffer> buffer_chain_to_mutable_buffer_sequence(BufferChain& bchain);
        // chunk encoding helpers
        friend BufferChainSPtr chunk_buffer(BufferChainSPtr buf_sptr);
        friend BufferChainSPtr chunk_empty();
        friend BufferChainSPtr chunk_last(BufferChainSPtr buf_sptr);

    private:
        std::vector<MBufferSPtr>                    m_chain;
        std::vector<boost::asio::mutable_buffer>    m_asio_chain;
        std::size_t                                 m_size;
};

} // namespave Marvin
#endif
