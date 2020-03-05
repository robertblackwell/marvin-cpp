#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <cassert>

#include <marvin/boost_stuff.hpp>

#include <marvin/error/marvin_error.hpp>
#include <marvin/callback_typedefs.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>

RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

#include <marvin/connection/timeout.hpp>

using boost::asio::ip::tcp;
using boost::bind;
using boost::function;
using boost::system::error_code;
using boost::asio::io_service;

#pragma mark - CTOR
Timeout::Timeout(
            boost::asio::io_service& io_service
            )
            :
            m_io(io_service),
            m_timer(m_io)
{
    LogTorTrace();
    /// timeout 
    m_timer.expires_from_now(boost::posix_time::pos_infin);
}

Timeout::~Timeout()
{
    LogTorTrace();
}
#pragma mark - public interface getters

void Timeout::cancelTimeout(std::function<void()> handler)
{
    LogDebug(" m_active: ", m_active);
#define XDISABLE_TIMEOUT
#ifdef DISABLE_TIMEOUT
    m_io.post(handler);
#else
    m_cancel_handler = handler;
    m_timer.expires_from_now(boost::posix_time::pos_infin);
    if( ! m_active) {
        if( m_expire_handler != nullptr)
            assert(m_expire_handler == nullptr);
        if( m_cancel_handler != nullptr)
            assert(m_cancel_handler != nullptr);
        m_io.post(handler);
        m_cancel_handler = nullptr;
    } else {
        m_expire_handler = nullptr;
        m_timer.cancel();
        m_timer.expires_from_now(boost::posix_time::pos_infin);
    }
#endif
}
void Timeout::setTimeout(long interval_millisecs, std::function<void()> handler)
{
    LogDebug(" interval millisecs: ", interval_millisecs);
#ifdef DISABLE_TIMEOUT
// do nothing
#else
    m_expire_handler = handler;
    m_cancel_handler = nullptr;
    m_active = true;
    auto whandler = std::bind(&Timeout::p_handle_timeout, this, std::placeholders::_1);
    m_timer.expires_from_now(boost::posix_time::milliseconds(interval_millisecs));
    m_timer.async_wait(whandler);
#endif
}
#pragma mark - private methods
void Timeout::p_handle_timeout(const boost::system::error_code& err)
{
    boost::system::error_code ec = err;
    LogDebug(" errc: ", Marvin::make_error_description(ec));
    if( err == boost::asio::error::operation_aborted) {
        // timeout was cancelled - presumably by a successful i/o operation
        assert(m_active);
        m_active = false;
        assert(m_cancel_handler != nullptr);
        auto h = m_cancel_handler;
        m_cancel_handler = nullptr;
        m_io.post(h);
    } else if ( err) {
        // some other error - close down ?
        assert(m_active);
        m_active = false;
        assert(false);
    } else {
        // no error - timeout
        assert(m_active);
//        assert(m_cancel_handler == nullptr);
        /// \warn this is a hack
        if( m_expire_handler == nullptr) {
            return;
        }
        assert(m_expire_handler != nullptr);
        m_active = false;
        auto h = m_expire_handler;
        m_expire_handler =nullptr;
        m_io.post(h);
    }
    m_active = false;
}
