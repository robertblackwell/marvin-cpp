#ifndef marvin_full_message_reader_v2_hpp
#define marvin_full_message_reader_v2_hpp

#include <iostream>
#include <iterator>
#include <algorithm>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

namespace Marvin {
/**
 * \brief A policy class that determines the the size of buffer reserved for each
 * async_read_some call.
 * Differentiates between buffers used for header reads and body reads.
 * Tries to grow the buffer on each read but is informed by
 * -    m_body_buffer_max
 * -    m_system_r_mem
 * for m_system_r_mm
 * on linux look in files
 * /proc/sys/net/core/r_mem_default
 * /proc/sys/net/core/r_mem_max
 * /proc/sys/net/core/w_mem_default
 * /proc/sys/net/core/w_mem_max
 * and pick a value that covers all of them.
 * On my dev system they all have the same value 212922
 */
class ReadBufferStrategy
{
protected:
    std::size_t m_current_body_buffer_size;
    std::size_t const m_header_buffer = 256*4;
    std::size_t const m_body_buffer_min = 256*4*8;
    /** look in files
     * /proc/sys/net/core/r_mem_default
     * /proc/sys/net/core/r_mem_max
     * /proc/sys/net/core/w_mem_default
     * /proc/sys/net/core/w_mem_max
     * and pick a value that covers all of them.
     * On my dev system they all have the same value
     */
    std::size_t const m_system_rmem = 212922;
    std::size_t const m_body_buffer_max = 256*4 * 256;
public:
    ReadBufferStrategy()
    {
    }
    /** the recommendation depends on where the reading/parsing process is up to
     * and should also consider what the system tcp buffer settings are
    */
    std::size_t recommended_buffer_size(MessageBase& partial_msg, Parser& parser)
    {
        if (!parser.header_done) {
            m_current_body_buffer_size = 0;
            return m_header_buffer;
        } else {
            if (m_current_body_buffer_size == 0) {
                auto clopt = partial_msg.header(HeaderFields::ContentLength);
                if( clopt ) {
                    // if there is a content length header try for a buffer that
                    // size but dont make it bigger than the system r_mem_max
                    std::size_t cl = atoi(clopt.get().c_str());
                    m_current_body_buffer_size = std::min(cl+100, m_system_rmem);
                } else {
                    m_current_body_buffer_size = m_body_buffer_min;
                }
            }  else {
                if (m_current_body_buffer_size < std::min(m_system_rmem, m_body_buffer_max)) {
                    m_current_body_buffer_size = 2*m_current_body_buffer_size;
                }
            }
            return m_current_body_buffer_size;
        }
    }
};

class FullMessageReader: public ReadBufferStrategy
{
public:

    using SPtr = std::shared_ptr<FullMessageReader>;
    using UPtr = std::unique_ptr<FullMessageReader>;
    using DoneCallback = std::function<void(Marvin::ErrorType err)>;

    FullMessageReader(ISocketSPtr read_socket_sptr);
    ~FullMessageReader();

    void async_read_message(MessageBaseSPtr message, DoneCallback cb);

protected:
    //----------------------------------------------------------------------------------------------------
    // protected methods
    //----------------------------------------------------------------------------------------------------
    void p_read_some();
    void p_parse_some();
    void p_on_read_error(ErrorType& err);
    void p_on_parse_error(ParserError pe);
    void p_on_eof();
    void p_on_done();
    std::size_t p_buffer_strategy(MessageBase& partial_msg, Parser& p);

    ISocketSPtr                  m_read_sock_sptr;
    Parser                       m_parser;
    boost::asio::streambuf       m_streambuffer;
    MessageBase::SPtr            m_current_message_sptr;
    DoneCallback                 m_read_cb;
};
} // namespcae
#endif
