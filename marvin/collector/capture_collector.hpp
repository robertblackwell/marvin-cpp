#ifndef marvin_capture_collector_hpp
#define marvin_capture_collector_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/collector/collector_interface.hpp>
#include <marvin/collector/capture_filter.hpp>

namespace Marvin {

struct CapturedExchange
{
    using SPtr = std::shared_ptr<CapturedExchange>;

    std::string scheme;
    std::string host;
    MessageBaseSPtr req;
    MessageBaseSPtr resp;
};
struct HostCaptures
{
    using SPtr = std::shared_ptr<HostCaptures>;
    std::string host;
    std::list<CapturedExchange::SPtr> exchanges;
};

using CapturedTraffic = std::list<HostCaptures::SPtr>;
/*
 * \ingroup collector
 * \brief CaptureCollector implements the rules that are applied when deciding
 * for which host traffic will be captured.
 *
 * IN this initial implementation it simply applies a set of regexes to the
 * host name and seeks a match.
 */
class CaptureCollector: public ICollector
{
public:
    using SPtr = std::shared_ptr<CaptureCollector>;
    using UpdateHandler = std::function<void()> ;

    CaptureCollector(
        boost::asio::io_service& io,
        CaptureFilter::SPtr filter_sptr);

    CaptureCollector(CaptureCollector const&)   = delete;
    void operator=(CaptureCollector const&)  = delete;

    /**
    ** Interface method for mitm app code to call in order to collect an exchange.
     * Important - this function copies both messages so that
     * captured traffic is independent of the continung operation
     * of the mimt-app. The copy is made before returning.
     *
     * The collect function is expected to run on the mitm thread(s)
     *
     * Update of the capture structures takes place on the thread associated with
     * the io_service passed to the constructor.
    **/
    void collect(
        std::string scheme,
        std::string host,
        MessageReaderSPtr req,
        MessageBaseSPtr resp);
    /**
     * Must be called on the io_service thread
     */
    HostCaptures get_captures();
    /**
     * registers a lambda to be run on the io_service whenever there is an update
     * to captures
     */
    void set_update_handler(UpdateHandler handler);

private:
    void posted_collect(
        std::string scheme,
        std::string host,
        MessageReaderSPtr req,
        MessageBaseSPtr resp);

    CapturedTraffic               m_captured_traffic;
    CaptureFilter::SPtr           m_filters_sptr;
    boost::asio::io_service&      m_io;
    UpdateHandler                 m_handler;

};
} // namespace
#endif
