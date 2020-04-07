#ifndef marvin_collector_base_hpp
#define marvin_collector_base_hpp
#include <iostream>                                  // for string, ostream
#include <marvin/collector/collector_interface.hpp>  // for ICollector
#include <boost/asio/io_context.hpp>                 // for io_context
#include <boost/asio/io_context_strand.hpp>          // for io_context::strand
#include <boost/asio/io_service.hpp>                 // for io_service
#include <marvin/http/message_base.hpp>              // for MessageBaseSPtr
#include <marvin/message/message_reader.hpp>         // for MessageReaderSPtr

namespace Marvin {

class CollectorBase: public ICollector
{
    public:
        CollectorBase(boost::asio::io_service& io);
        // CollectorBase(boost::asio::io_service& io, boost::filesystem::path file_path);
        CollectorBase(boost::asio::io_service& io, std::ostream& out_stream);

        /**
        ** Delete copy constructors
        **/
        CollectorBase(CollectorBase const&)   = delete;
        void operator=(CollectorBase const&)  = delete;
    

        /**
        ** Interface method for client code to call collect
        **/
        void collect(std::string scheme, std::string host, MessageReaderSPtr req, Marvin::MessageBaseSPtr resp);
    
    private:
        /**
        ** This method actually implements the collect function but run on a dedicated
        ** strand. Even if this method does IO-wait operations the other thread will
        ** keep going
        **/
        void postedCollect(
            std::string scheme,
            std::string host,
            MessageReaderSPtr req,
            Marvin::MessageBaseSPtr resp);

        boost::asio::io_context::strand         m_my_strand;
        boost::asio::io_service&                m_io;
        std::ostream&                           m_output_stream;
    
};
typedef std::shared_ptr<CollectorBase> CollectorBaseSPtr;

} //namespace
#endif
