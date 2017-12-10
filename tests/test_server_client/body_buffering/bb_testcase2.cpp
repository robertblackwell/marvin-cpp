
#include "bf_testcase.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

using namespace body_buffering;

EchoTestcase::EchoTestcase(EchoTestType type, std::string desc, std::vector<std::string> bufs) : _test_type(type), _description(desc), _body_buffers(bufs)
{
   _url = "http://localhost:9991";
}
std::size_t     EchoTestcase::chunks_count()
{
    return _body_buffers.size();
}
MBufferSPtr     EchoTestcase::buffers_as_mbuffer()
{
    return m_buffer(buffers_as_string());
}
std::string     EchoTestcase::buffers_as_string()
{
    std::string res = "";
    for(std::string& s: _body_buffers) {
        res += s;
    }
    return res;
}
BufferChainSPtr EchoTestcase::buffers_as_buffer_chain()
{
    std::string res = "";
    BufferChainSPtr sp = std::make_shared<BufferChain>();
    for(std::string& s: _body_buffers) {
        res += s;
        MBufferSPtr mb = m_buffer(s);
        sp->push_back(mb);
    }
    return sp;
}

MBufferSPtr     EchoTestcase::chunk_as_mbuffer(std::size_t index)
{
    return m_buffer(chunk_as_string(index));
}
std::string     EchoTestcase::chunk_as_string(std::size_t index)
{
    return _body_buffers[index];
}
BufferChainSPtr EchoTestcase::chunk_as_buffer_chain(std::size_t index)
{
    return buffer_chain(chunk_as_mbuffer(index));
}
