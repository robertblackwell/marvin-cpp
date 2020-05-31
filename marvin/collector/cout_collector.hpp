#ifndef marvin_collector_cout_hpp
#define marvin_collector_cout_hpp
#include <iostream>
#include <marvin/collector/collector_interface.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/io_service.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/message/message_reader_v2.hpp>
#include <marvin/collector/capture_filter.hpp>

namespace Marvin {
/**
* \defgroup collector Collectors
* \brief Provides a number of classes for collecting and distributing traffic captured by a mitm proxy.
*/

/**
* \ingroup collector
* \brief The base class, and simplest type of collector.
*
* Collectors are a group of classes with a common interface that collect,
* format and dispatch the traffic intercepted by a mitm application.
*
* This class dispatches the formatted traffic to stdout.
*/
class CoutCollector: public ICollector
{
    public:
        using SPtr = std::shared_ptr<CoutCollector>;

        CoutCollector(boost::asio::io_service& io, CaptureFilter::SPtr filter_sptr);

        /**
        ** Delete copy constructors
        **/
        CoutCollector(CoutCollector const&)   = delete;
        void operator=(CoutCollector const&)  = delete;
    

        /**
        ** Interface method for client code to call collect
        **/
        void collect(std::string scheme, std::string host, MessageBase::SPtr req, Marvin::MessageBase::SPtr resp);
    
    private:
        /**
        ** This method actually implements the collect function but run on a dedicated
        ** strand. Even if this method does IO-wait operations the other thread will
        ** keep going
        **/
        void posted_collect(
            std::string scheme,
            std::string host,
            MessageBase::SPtr req,
            Marvin::MessageBase::SPtr resp);

        boost::asio::io_service&                m_io;
        CaptureFilter::SPtr                     m_filter_sptr;

};
typedef std::shared_ptr<CoutCollector> CoutCollectorSPtr;

} //namespace
#endif
