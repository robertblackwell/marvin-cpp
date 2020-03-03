#ifndef buffer_chain_hpp
#define buffer_chain_hpp
 #include <iostream>
 #include <iterator>
 #include <algorithm>
 #include <stdexcept>
 #include <cassert>
 #include <vector>
 #include<marvin/boost_stuff.hpp>
 #include <marvin/buffer/m_buffer.hpp>
/**
* \ingroup Buffer
*/

namespace Marvin {
class BufferChain;

/// \ingroup Buffer
using BufferChainSPtr = std::shared_ptr<BufferChain>;
/**
* \ingroup Buffer
* \brief Provides a chain of MBuffers to allow for reading data in chunks without copying into a single contigous buffer.
*/
class BufferChain
{
    public:
        static BufferChainSPtr makeSPtr(std::string& s);
        static BufferChainSPtr makeSPtr(MBuffer& mb);
        static BufferChainSPtr makeSPtr(MBufferSPtr mb_sptr);
    
        BufferChain();
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
    
        friend std::vector<boost::asio::const_buffer> buffer_chain_to_const_buffer_sequence(BufferChain& bchain);
        friend std::vector<boost::asio::mutable_buffer> buffer_chain_to_mutable_buffer_sequence(BufferChain& bchain);

    private:
        std::vector<MBufferSPtr>                    _chain;
        std::vector<boost::asio::mutable_buffer>    _asio_chain;
        std::size_t                                 _size;
};

} // namespave Marvin
#endif
