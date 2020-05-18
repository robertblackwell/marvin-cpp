#ifndef buffer_chain_template_hpp
#define buffer_chain_template_hpp
#include <stddef.h>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/asio/buffer.hpp>
#include <marvin/buffer/contig_buffer.hpp>

namespace Marvin {
using AsioConstBufferSeq = std::vector<boost::asio::const_buffer>;
using AsioMutableBufferSeq = std::vector<boost::asio::mutable_buffer>;

class BufferChainT
{
    public:
        using SPtr = std::shared_ptr<BufferChainT>;
        using CBuf = ContigBufferT;

    private:
        using Factory = ContigBufferFactoryT;

        std::vector<CBuf::SPtr>     m_chain;
        AsioConstBufferSeq          m_asio_chain;
        std::size_t                 m_size;
        Factory&                    m_buf_factory;

    public:
        /** shared pointer factories*/
#if 0
        SPtr makeSPtr(ContigBufferFactoryT& factory)
        {
            SPtr sp = std::make_shared<BufferChainT>(m_buf_factory);
            return sp;
        }
        SPtr makeSPtr(BufferChainT&& other)
        {
            SPtr sp = std::make_shared<BufferChainT>(std::move(other));
            return sp;
        }
        SPtr makeSPtr(ContigBufferFactoryT& factory, std::string& s)
        {
            SPtr sp = std::make_shared<BufferChainT>();
            sp->push_back(m_buf_factory.makeSPtr(s));
            return sp;
        }
        SPtr makeSPtr(ContigBufferFactoryT& factory, CBuf& cb)
        {
            SPtr sp = std::make_shared<BufferChainT>();
            sp->push_back(m_buf_factory.makeSPtr(cb));
            return sp;
        }
        SPtr makeSPtr(ContigBufferFactoryT& factory, CBuf::SPtr cb_sptr)
        {
            SPtr sp = std::make_shared<BufferChainT>();
            sp->push_back(cb_sptr);
            return sp;
        }
#endif
        /** constructors adnassignment operators */    
        BufferChainT(Factory& factory): m_buf_factory(factory)
        {
            m_chain = std::vector<ContigBufferT::SPtr>();
            m_size = 0;
        }
        BufferChainT(BufferChainT& other): m_buf_factory(other.m_buf_factory)
        {
            m_size = 0;
            for (ContigBufferT::SPtr mb_sptr: other.m_chain) {
                this->push_back(m_buf_factory.makeSPtr(*mb_sptr));
            }
        }
        BufferChainT& operator =(BufferChainT& other)
        {
            if(&other == this) {
                return *this;
            }
            m_size = 0;
            for (ContigBufferT::SPtr mb_sptr: other.m_chain) {
                this->push_back(m_buf_factory.makeSPtr(*mb_sptr));
            }
            return *this;
        }
        BufferChainT(BufferChainT&& other): m_buf_factory(other.m_buf_factory)
        {
            m_size = other.m_size;
            m_chain = std::move(other.m_chain);
            m_asio_chain = std::move(other.m_asio_chain);
            other.m_size = 0;
            // m_size = other.m_size;
            // m_chain = std::move(other.m_chain);
            // other.m_size = 0;
        }
        BufferChainT& operator =(BufferChainT&& other)
        {
            if (&other == this) {
                return *this;
            }
            std::swap(m_chain, other.m_chain);
            std::swap(m_size, other.m_size);
            return *this;
        }

        /** append data - either append to the final block or add a new block*/

        void append(void* buf, std::size_t len)
        {
            if (m_chain.size() > 0) {
                CBuf::SPtr last_mb = m_chain.at(m_chain.size()-1);
                if ((last_mb->capacity() - last_mb->size()) >= len) {
                    last_mb->append(buf, len);
        #define MARVIN_MK_CONSTBUF
        #ifdef MARVIN_MK_CONSTBUF
                    m_asio_chain.back() = boost::asio::const_buffer(last_mb->data(), last_mb->size());
        #endif
                    m_size += len;
                    return;
                }
            }
            std::size_t required_len = (len > 256*4*8) ? len+100 : 256*4*8;
            CBuf::SPtr new_mb = m_buf_factory.makeSPtr(required_len);
            new_mb->append(buf, len);
            this->push_back(new_mb);
        }
        void append(std::string str)
        {
            append((void*)str.c_str(), str.size());
        }
        void append(std::string& str)
        {
            append((void*)str.c_str(), str.size());
        }

        void push_back(CBuf::SPtr mb)
        {
            m_size += mb->size();
            m_chain.push_back(mb);
        // this is not working
        #ifdef MARVIN_MK_CONSTBUF
            m_asio_chain.emplace_back(boost::asio::const_buffer(mb->data(), mb->size()));
        #endif
        }
        void clear()
        {
            m_chain.clear();
            m_asio_chain.clear();
            m_size = 0;
        }
        std::size_t size()
        {
            return m_size;
        }
                
        std::size_t blocks()
        {
            return m_chain.size();
        }
        ContigBufferT& block_at(std::size_t index)
        {
            if (index >= m_chain.size()) {
                    MARVIN_THROW("index out of range");
            }
            return *(m_chain.at(index));
        }
        std::string to_string()
        {
            std::string s = "";
            for(CBuf::SPtr& mb : m_chain) {
                s += mb->toString();
            }
            return s;
        }
        CBuf::SPtr amalgamate()
        {
            CBuf::SPtr mb_final = m_buf_factory.makeSPtr(this->size());
            for(CBuf::SPtr& mb : m_chain) {
                mb_final->append(mb->data(), mb->size());
            }
            return mb_final;
        }


        void createAsioBufferSequence()
        {
        #ifdef MARVIN_MK_CONSTBUF
            return;
        #else
            m_asio_chain = AsioConstBufferSeq();
            for(auto mb: m_chain) {
                m_asio_chain.emplace_back(boost::asio::const_buffer(mb->data(), mb->size()));
            }
        #endif
        }

        friend std::ostream& operator <<(std::ostream &os, BufferChainT& b)
        {
            if(b.size() == 0){
                os << "Empty ";
            }else{
                std::string s = b.to_string();
                os << std::endl << "BufferChain{ length: " << b.size() << "content: [" << s << "]}";
            }
            return os;
        }
        /**
        * converts a Bufferchain to a boost buffer for use in async write calls
        * thus need const_bufer_seq
        */
        AsioConstBufferSeq& asio_buffer_sequence()
        {
            createAsioBufferSequence();
            return this->m_asio_chain;
        }
        friend AsioConstBufferSeq& buffer_chain_to_const_buffer_sequence(BufferChainT& bchain)
        {
            bchain.createAsioBufferSequence();
            return bchain.m_asio_chain;
        }
        friend AsioConstBufferSeq& buffer_chain_to_mutable_buffer_sequence(BufferChainT& bchain)
        {
            bchain.createAsioBufferSequence();
            return bchain.m_asio_chain;
        }

};

inline std::string buffersequence_to_string(AsioConstBufferSeq& mutbufs)
{
    std::string s;
    for(auto& m: mutbufs) {
        s = s + std::string((char*)m.data(), m.size());
    }
    return s;
}

} // namespave Marvin
#endif
