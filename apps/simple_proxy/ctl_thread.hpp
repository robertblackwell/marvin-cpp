
#ifndef marvin_mitm_xctl_thread_hpp
#define marvin_mitm_xctl_thread_hpp

#include <string>
#include <thread>
#include <boost/optional.hpp>


namespace Marvin {
class CtlThread
{
    public:
    CtlThread(boost::optional<std::string> ctl_pipe_path)
    {
        m_thread_uptr = std::make_unique<std::thread>([this]()
        {
            while (true) {
                sleep(1);
                std::cout << __func__ << std::endl;
            };
        });
    }

    ~CtlThread()
    {

    }
    std::thread& getThread() 
    { 
        return *m_thread_uptr; 
    }

    std::unique_ptr<std::thread> m_thread_uptr;    
};
}

#endif
