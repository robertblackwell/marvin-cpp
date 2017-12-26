
// #include <iostream>
// #include <iterator>
// #include <algorithm>
// #include <stdexcept>
// #include <boost/asio.hpp>
// #include <boost/bind.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>
// #include <cassert>
// #include <vector>
#include "buffer_chain.hpp"
#include "rb_logger.hpp"

using namespace Marvin;
namespace Marvin {
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN )


#pragma mark - BufferChain
BufferChainSPtr BufferChain::makeSPtr(std::string& s)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(Marvin::MBuffer::makeSPtr(s));
    return sp;
}
BufferChainSPtr BufferChain::makeSPtr(MBuffer& mb)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(Marvin::MBuffer::makeSPtr(mb));
    return sp;
}
BufferChainSPtr BufferChain::makeSPtr(MBufferSPtr mb_sptr)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(mb_sptr);
    return sp;
}


BufferChain::BufferChain()
{
    _chain = std::vector<MBufferSPtr>();
    _size = 0;
}
void BufferChain::push_back(MBufferSPtr mb)
{
    _size += mb->size();
    _chain.push_back(mb);
    _asio_chain.push_back(boost::asio::buffer(mb->data(), mb->size()));
}
void BufferChain::clear()
{
    _chain.clear();
    _asio_chain.clear();
    _size = 0;
}
std::size_t BufferChain::size()
{
    return _size;
}
std::string BufferChain::to_string()
{
    std::string s = "";
    for(MBufferSPtr& mb : _chain) {
        s += mb->toString();
    }
    return s;
}
MBufferSPtr BufferChain::amalgamate()
{
    MBufferSPtr mb_final = std::shared_ptr<MBuffer>(new MBuffer(this->size()));
    for(MBufferSPtr& mb : _chain) {
        mb_final->append(mb->data(), mb->size());
    }
    return mb_final;
}

BufferChainSPtr buffer_chain(std::string& s)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(Marvin::MBuffer::makeSPtr(s));
    return sp;
}
BufferChainSPtr buffer_chain(MBuffer& mb)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(Marvin::MBuffer::makeSPtr(mb));
    return sp;
}
BufferChainSPtr buffer_chain(MBufferSPtr mb_sptr)
{
    BufferChainSPtr sp = std::shared_ptr<BufferChain>(new BufferChain());
    sp->push_back(mb_sptr);
    return sp;
}

std::vector<boost::asio::mutable_buffer> BufferChain::asio_buffer_sequence()
{
    return this->_asio_chain;
}

std::vector<boost::asio::const_buffer> buffer_chain_to_const_buffer_sequence(BufferChain& bchain)
{
    assert(false);
//    return bchain._asio_chain;
}
std::vector<boost::asio::mutable_buffer> buffer_chain_to_mutable_buffer_sequence(BufferChain& bchain)
{
    return bchain._asio_chain;
}


std::ostream &operator<< (std::ostream &os, BufferChain &b)
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
