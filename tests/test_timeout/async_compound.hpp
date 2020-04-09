#ifndef marvin_test_timeout_async_compound_hpp
#define marvin_test_timeout_async_compound_hpp

#include <string>

#include <marvin/boost_stuff.hpp>
#include <marvin/connection/timeout.hpp>
#include <doctest/doctest.h>
#include <marvin/error/marvin_error.hpp>
#include <marvin/external_src/trog/trog.hpp>
// Trog_SETLEVEL(LOG_LEVEL_WARN)

class AsyncComposedOp
{
public:
    AsyncComposedOp(
        boost::asio::io_service& io,
        long op_1_interval_secs, std::string op_1_result_string,
        long op_2_interval_secs, std::string op_2_result_string,
        long op_1_timeout_interval_secs,
        long op_2_timeout_interval_secs

        )
    : m_io(io),
    m_strand(io),
    m_timeout(io),
    m_timer(io),
    m_count(0),
    m_op_1_interval_secs(op_1_interval_secs), m_op_1_result(op_1_result_string),
    m_op_2_interval_secs(op_2_interval_secs), m_op_2_result(op_2_result_string),
    m_op_1_timeout_interval_secs(op_1_timeout_interval_secs*1000),
    m_op_2_timeout_interval_secs(op_2_timeout_interval_secs*1000),
    m_result("")
    {}
    ~AsyncComposedOp()
    {
//        std::cout << "Final count is " << m_count << std::endl;
    }
    void cancel()
    {
        m_timer.cancel();
    }
    void async_composed_op(std::function<void(std::string result, const boost::system::error_code err)> handler)
    {
        m_composed_handler = handler;
        async_op_1();
    }
    /// this method and the associated handler simulate a failed async operation
    void async_op_1()
    {
        m_timer.expires_from_now(boost::posix_time::seconds(m_op_1_interval_secs));
        m_timeout.setTimeout(m_op_1_timeout_interval_secs, ([this](){
            m_timer.cancel();
        }));
        m_timer.async_wait((std::bind(&AsyncComposedOp::async_op_1_handler, this, std::placeholders::_1)));
    }
    /// handler for simulated failed async operation
    void async_op_1_handler(const boost::system::error_code& err)
    {
        boost::system::error_code ec = err;
        m_timeout.cancelTimeout( ([this, err, ec](){
            if(!err) {
                m_result += m_op_1_result;
                async_op_2();
            } else {
                m_io.post(std::bind(m_composed_handler, "", err));
            }
        }));
    }

    /// this method and the associated handler simulate a successful async operation
    void async_op_2()
    {
        m_timer.expires_from_now(boost::posix_time::seconds(m_op_2_interval_secs));
        m_timeout.setTimeout(m_op_2_timeout_interval_secs, m_strand.wrap([this](){
            m_timer.cancel();
        }));
        m_timer.async_wait((std::bind(&AsyncComposedOp::async_op_2_handler, this, std::placeholders::_1)));
    }
    /// handler for simulated successful async operation
    void async_op_2_handler(const boost::system::error_code& err)
    {
        boost::system::error_code ec = err;
        std::string s = Marvin::make_error_description(ec);
        LogDebug(" err: ", s);
        m_timeout.cancelTimeout(m_strand.wrap([this, err, ec, s](){
            LogDebug(" err: ", s);
            if(!err) {
                m_result += m_op_2_result;
                m_io.post(std::bind(m_composed_handler, m_result, err));
            } else {
                m_io.post(std::bind(m_composed_handler, m_result, err));
            }
        }));
    }
    
private:
    std::function<void(const boost::system::error_code err)>  m_op_handler;
    std::function<void(std::string result, const boost::system::error_code err)>  m_composed_handler;
    boost::asio::io_service&        m_io;
    boost::asio::io_service::strand m_strand;
    boost::asio::deadline_timer     m_timer;
    ::Marvin::Timeout m_timeout;
    int m_count;
    std::string m_result, m_op_1_result, m_op_2_result;
    long m_op_1_interval_secs, m_op_2_interval_secs, m_op_1_timeout_interval_secs, m_op_2_timeout_interval_secs;
};
#endif

