#ifndef marvin_collector_capture_filter_hpp
#define marvin_collector_capture_filter_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <unistd.h>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/message/message_reader_v2.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/collector/collector_interface.hpp>

namespace Marvin {

/*
 * \ingroup collector
 * \brief CaptureFilter captures a subset of collected traffic and provides an
 * interface through which other components can interogate the captured traffic.
 *
 * Maintains a set of regex expressions that are used to filter http(s)
 * traffic and capture the traffic that matches one of the regex filters.
 *
 * The captured traffic is maintained in a circular buffer. The buffer
 * has a max size and traffic is discarded from the buffer on a fifo basis.
 *
 * The buffer is indexed by the traffic host name.
 */
class CaptureFilter
{
public:
    using SPtr = std::shared_ptr<CaptureFilter>;
    using Filters = std::list<std::string>;

    CaptureFilter(Filters initial): m_filters(initial)
    {
        m_show_message_bodies = false;
    }
    CaptureFilter(std::vector<std::string> strings)
    {
        for(std::string& s: strings) {
            m_filters.push_back(s);
        }
        m_show_message_bodies = false;
    }

    CaptureFilter(CaptureFilter const&)   = delete;
    void operator=(CaptureFilter const&)  = delete;

    bool is_collectable(std::string& host)
    {
        return is_collectable("", host, nullptr, nullptr);
    }

    bool is_collectable(
        std::string scheme,
        std::string host,
        MessageBase::SPtr req,
        MessageBase::SPtr resp)
    {
        for(auto f: m_filters) {
            std::regex e{f};
            if (regex_match(host, e)) {
                return true;
            }
        }
        return false;
    }
    // determines if body is collectable. Basically is human readable
    bool is_body_collectable(MessageBaseSPtr msg)
    {
        return true;
    }
    bool get_show_message_bodies()
    {
        return m_show_message_bodies;
    }
    void set_show_message_bodies(bool on_off)
    {
        m_show_message_bodies = on_off;
    }
    Filters get_filters()
    {
        return m_filters;
    }
    void set_filters(Filters &filters)
    {
        m_filters = filters;
    }
private:
    Filters                     m_filters;
    bool                        m_show_message_bodies;
};
} // namespace
#endif



