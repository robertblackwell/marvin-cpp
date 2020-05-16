#include <cassert>
#include <boost/asio/buffer.hpp>
#include <marvin/buffer/contig_buffer.hpp>
#include <marvin/buffer/buffer_chain.hpp>
#include <marvin/macros.hpp>
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

using namespace Marvin;
namespace Marvin {


#pragma mark - BufferChain
BufferChainSPtr BufferChain::makeSPtr()
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    return sp;
}
BufferChainSPtr BufferChain::makeSPtr(BufferChain&& other)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain(std::move(other)));
    return sp;
}
BufferChainSPtr BufferChain::makeSPtr(std::string& s)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(Marvin::ContigBuffer::makeSPtr(s));
    return sp;
}
BufferChainSPtr BufferChain::makeSPtr(ContigBuffer& mb)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(Marvin::ContigBuffer::makeSPtr(mb));
    return sp;
}
BufferChainSPtr BufferChain::makeSPtr(ContigBufferSPtr mb_sptr)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(mb_sptr);
    return sp;
}
BufferChainSPtr chunk_buffer(BufferChainSPtr buf_sptr)
{
    return nullptr;
}
BufferChainSPtr chunk_empty()
{
    return nullptr;

}
BufferChainSPtr chunk_last(BufferChainSPtr buf_sptr)
{
    return nullptr;
}

BufferChain::BufferChain()
{
    m_chain = std::vector<ContigBufferSPtr>();
    m_size = 0;
}
BufferChain::BufferChain(BufferChain& other)
{
    TROG_WARN("buffer chain being copied");
    m_size = 0;
    for (ContigBufferSPtr mb_sptr: other.m_chain) {
        this->push_back(ContigBuffer::makeSPtr(*mb_sptr));
    }
}
BufferChain& BufferChain::operator =(BufferChain& other)
{
    TROG_WARN("buffer chain being copied");
    if(&other == this) {
        return *this;
    }
    m_size = 0;
    for (ContigBufferSPtr mb_sptr: other.m_chain) {
        this->push_back(ContigBuffer::makeSPtr(*mb_sptr));
    }
    return *this;
}
BufferChain::BufferChain(BufferChain&& other)
{
    TROG_WARN("buffer chain being moved");
    m_size = other.m_size;
    m_chain = std::move(other.m_chain);
    m_asio_chain = std::move(other.m_asio_chain);
    other.m_size = 0;
    // m_size = other.m_size;
    // m_chain = std::move(other.m_chain);
    // other.m_size = 0;
}
BufferChain& BufferChain::operator =(BufferChain&& other)
{
    TROG_WARN("buffer chain being moved");
    if (&other == this) {
        return *this;
    }
    std::swap(m_chain, other.m_chain);
    std::swap(m_size, other.m_size);
    return *this;
}

void BufferChain::append(void* buf, std::size_t len)
{
    if (m_chain.size() > 0) {
        ContigBufferSPtr last_mb = m_chain.at(m_chain.size()-1);
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
    ContigBufferSPtr new_mb = ContigBuffer::makeSPtr(required_len);
    new_mb->append(buf, len);
    this->push_back(new_mb);
}
void BufferChain::append(std::string str)
{
    append((void*)str.c_str(), str.size());
}
void BufferChain::append(std::string& str)
{
    append((void*)str.c_str(), str.size());
}

void BufferChain::push_back(ContigBufferSPtr mb)
{
    m_size += mb->size();
    m_chain.push_back(mb);
// this is not working
#ifdef MARVIN_MK_CONSTBUF
    m_asio_chain.emplace_back(boost::asio::const_buffer(mb->data(), mb->size()));
#endif
}
void BufferChain::clear()
{
    m_chain.clear();
    m_asio_chain.clear();
    m_size = 0;
}
std::size_t BufferChain::size()
{
    return m_size;
}
std::size_t BufferChain::blocks()
{
    return m_chain.size();
}
ContigBuffer& BufferChain::block_at(std::size_t index)
{
    if (index >= m_chain.size()) {
            MARVIN_THROW("index out of range");
    }
    return *(m_chain.at(index));
}
std::string BufferChain::to_string()
{
    std::string s = "";
    for(ContigBufferSPtr& mb : m_chain) {
        s += mb->toString();
    }
    return s;
}
ContigBufferSPtr BufferChain::amalgamate()
{
    ContigBufferSPtr mb_final = std::make_shared<ContigBuffer>(this->size());
    for(ContigBufferSPtr& mb : m_chain) {
        mb_final->append(mb->data(), mb->size());
    }
    return mb_final;
}

BufferChainSPtr buffer_chain(std::string& s)
{
    BufferChainSPtr sp = std::make_shared<BufferChain>();
    sp->push_back(Marvin::ContigBuffer::makeSPtr(s));
    return sp;
}
BufferChainSPtr buffer_chain(ContigBuffer& mb)
{
    BufferChainSPtr sp = std::make_shared<BufferChain>();
    sp->push_back(Marvin::ContigBuffer::makeSPtr(mb));
    return sp;
}
BufferChainSPtr buffer_chain(ContigBufferSPtr mb_sptr)
{
    BufferChainSPtr sp = std::make_shared<BufferChain>();
    sp->push_back(std::move(mb_sptr));
    return sp;
}
void BufferChain::createAsioBufferSequence()
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
BufferChain::AsioConstBufferSeq& BufferChain::asio_buffer_sequence()
{
    createAsioBufferSequence();
    return this->m_asio_chain;
}

BufferChain::AsioConstBufferSeq& buffer_chain_to_const_buffer_sequence(BufferChain& bchain)
{
    bchain.createAsioBufferSequence();
    return bchain.m_asio_chain;
}
BufferChain::AsioConstBufferSeq& buffer_chain_to_mutable_buffer_sequence(BufferChain& bchain)
{
    bchain.createAsioBufferSequence();
    return bchain.m_asio_chain;
}
std::string buffersequence_to_string(BufferChain::AsioConstBufferSeq& bufs)
{
    std::string s;
    for(auto& m: bufs) {
        s = s + std::string((char*)m.data(), m.size());
    }
    return s;
}
std::ostream& operator <<(std::ostream &os, BufferChain& b)
{
    if(b.size() == 0){
        os << "Empty ";
    }else{
        std::string s = b.to_string();
        os << std::endl << "BufferChain{ length: " << b.size() << "content: [" << s << "]}";
    }
    return os;
}
} // namespace Marvin
