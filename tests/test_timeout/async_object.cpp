
/// \ingroup Tests
/// \brief A class that simulates successful and failed async operations with timeout; it runs all its handlers on a strand
/// and passes that strand to a timeout handler, the tiimeout handler is a member of this object..
///
class AsyncObject
{
public:
    AsyncObject(boost::asio::io_service& io)
    : m_io(io),
    m_timeout(io),
    m_timer(io, boost::posix_time::seconds(posix_time::pos_infin)),
    m_count(0)
    {}
    ~AsyncObject()
    {
        std::cout << __FUNCTION__ << std::endl;
    }
    void cancel()
    {
        m_timer.cancel();
    }
    ///
    /// \brief this method and the associated handler simulate a failed async operation
    ///
    void async_fail_operation(long interval_secs, long to_interval_secs, std::function<void(const boost::system::error_code err)> handler)
    {
        m_op_handler = handler;
        long to_interval = to_interval_secs*1000;
        m_timer.expires_from_now(boost::posix_time::seconds(interval_secs));
        m_timeout.setTimeout(to_interval, ([this](){
            m_timer.cancel();
        }));
        auto xhw = (std::bind(&AsyncObject::async_fail_op_handler, this, std::placeholders::_1));
        m_timer.async_wait((std::bind(&AsyncObject::async_fail_op_handler, this, std::placeholders::_1)));
    }
    /// handler for simulated failed async operation
    void async_fail_op_handler(const boost::system::error_code& err)
    {
        boost::system::error_code ec = err;
        if(!err)
            /// ht here if we were not timed out - so simulate an error
            ec = boost::asio::error::make_error_code(boost::asio::error::broken_pipe);
        else {
            /// the only way we should come into this function with an error is if we
            /// were tiimed out - so test that
            auto asio_op_aborted = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);
            REQUIRE(ec == asio_op_aborted);
        }
        std::string s = Marvin::make_error_description(ec);
        LogDebug(" err: ", s);
        m_timeout.cancelTimeout( ([this, ec, s](){
            LogDebug(" err: ", s);
            m_op_handler(ec);
        }));
    }

    ///
    /// \brief this method and the associated handler simulate a successful async operation
    ///
    void async_operation(long interval_secs, long to_interval_secs, std::function<void(const boost::system::error_code err)> handler)
    {
        m_op_handler = handler;
        long to_interval = to_interval_secs*1000;
        m_timer.expires_from_now(boost::posix_time::seconds(interval_secs));
        m_timeout.setTimeout(to_interval, [this](){
            m_timer.cancel();
        });
        m_timer.async_wait((std::bind(&AsyncObject::async_op_handler, this, std::placeholders::_1)));
    }
    /// handler for simulated successful async operation
    void async_op_handler(const boost::system::error_code& err)
    {
        boost::system::error_code ec = err;
        std::string s = Marvin::make_error_description(ec);
        LogDebug(" err: ", s);
        m_timeout.cancelTimeout([this, ec, s](){
            LogDebug(" err: ", s);
            m_op_handler(ec);
        });
    }
    
private:
    std::function<void(const boost::system::error_code err)>  m_op_handler;
    boost::asio::io_service&        m_io;
    boost::asio::deadline_timer     m_timer;
    Timeout m_timeout;
    int m_count;
};

