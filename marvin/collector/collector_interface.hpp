#ifndef marvin_collector_interface_hpp
#define marvin_collector_interface_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/message/message_reader_v2.hpp>
namespace Marvin {

/**
* \ingroup collector
* \brief The common interface that all collectors are required to implement.
*
*/
class ICollector
{
    public:
        /**
        ** Interface method for client code to call collect
        **/
        virtual void collect(std::string scheme, std::string host, MessageBase::SPtr req, Marvin::MessageBase::SPtr resp) = 0;
    
    private:
};
typedef std::shared_ptr<ICollector> ICollectorSPtr;
} // namespace
#endif
