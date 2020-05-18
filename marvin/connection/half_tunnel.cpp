#include <marvin/connection/half_tunnel.hpp>

#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

namespace Marvin {

HalfTunnel::HalfTunnel(ISocketSPtr readEnd, ISocketSPtr writeEnd, long firstReadTimeoutMillisecs, long subsequentReadTimeoutMillisecs)
:   m_first_read_timeout_millisecs(firstReadTimeoutMillisecs),
    m_subsequent_read_timeout_millisecs(subsequentReadTimeoutMillisecs),
    m_strategy(TunnelAllocator()),
    m_factory(m_strategy)
{
    m_read_end = readEnd;
    m_write_end = writeEnd;
    m_bufferSPtr = m_factory.makeSPtr();
//    m_bufferUPtr = std::unique_ptr<Marvin::ContigBuffer>(m_bufferPtr);
}
void HalfTunnel::start(std::function<void(Marvin::ErrorType& err)> cb)
{
    m_read_end->setReadTimeout(m_first_read_timeout_millisecs);
    m_callback = cb;
    p_start_read();
}
void HalfTunnel::p_start_read()
{
    auto hf = std::bind(&HalfTunnel::p_handle_read, this, std::placeholders::_1, std::placeholders::_2);
    m_read_end->asyncRead(m_bufferSPtr, hf);
}
void HalfTunnel::p_handle_read(Marvin::ErrorType& err, std::size_t bytes_transfered)
{
    if( ! err ){
       TROG_TRACE4("OK Read bytes transfered : ", bytes_transfered);
        auto hf = std::bind(&HalfTunnel::p_handle_write, this, std::placeholders::_1, std::placeholders::_2);
        m_write_end->asyncWrite(*m_bufferSPtr, hf);
    } else {
        std::string m = Marvin::make_error_description(err);
        m_callback(err);
//        assert(false);
    }
}
void HalfTunnel::p_handle_write(Marvin::ErrorType& err, std::size_t bytes_transfered)
{
    if( ! err ){
       TROG_TRACE3("OK write");
        m_read_end->setReadTimeout(m_subsequent_read_timeout_millisecs);
        p_start_read();
    } else {
        std::string m = Marvin::make_error_description(err);
        m_callback(err);
//        assert(false);
    }
}
} // namespace Marvin
