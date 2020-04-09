
#include "bf_testcase.hpp"
Trog_SETLEVEL(LOG_LEVEL_WARN)

using namespace body_format;

Testcase::Testcase(TestType type, std::string desc, std::vector<std::string> bufs)
: m_test_type(type), m_description(desc), m_body_buffers(bufs)
{
   m_url = "http://localhost:9991";
}
std::size_t     Testcase::chunks_count() const
{
    return m_body_buffers.size();
}
Marvin::MBufferSPtr     Testcase::buffers_as_mbuffer() const
{
    return Marvin::MBuffer::makeSPtr(buffers_as_string());
//    return m_buffer(buffers_as_string());
}
std::string     Testcase::buffers_as_string() const
{
    std::string res = "";
    for(const std::string& s: m_body_buffers) {
        res += s;
    }
    return res;
}
Marvin::BufferChainSPtr Testcase::buffers_as_buffer_chain() const
{
    std::string res = "";
    Marvin::BufferChainSPtr sp = std::make_shared<Marvin::BufferChain>();
    for(const std::string& s: m_body_buffers) {
        res += s;
        Marvin::MBufferSPtr mb = Marvin::MBuffer::makeSPtr(s);
        sp->push_back(mb);
    }
    return sp;
}

Marvin::MBufferSPtr     Testcase::chunk_as_mbuffer(std::size_t index)
{
    return Marvin::MBuffer::makeSPtr(chunk_as_string(index));
}
std::string     Testcase::chunk_as_string(std::size_t index)
{
    return m_body_buffers[index];
}
Marvin::BufferChainSPtr Testcase::chunk_as_buffer_chain(std::size_t index)
{
    return Marvin::BufferChain::makeSPtr(chunk_as_mbuffer(index));
}
