#ifndef guard_marvin_simple_logger_hpp
#define  guard_marvin_simple_logger_hpp
#include <mutex>
#define XNO_MARVIN_SIMPLE_LOGGER
namespace Logger {
extern std::mutex log_mutex;
}
#ifdef NO_MARVIN_SIMPLE_LOGGER
#    define MTRACE(x) do {} while (0)
#else
/// This is not sophisticated, nor high performabnce but it works adequately for
/// a demo project
#    define MTRACE(x)                                                                                                                                           \
        do {                                                                                                                                                   \
            std::lock_guard<std::mutex> guard(Logger::log_mutex);                                                                                              \
            std::cout << "T[" << __PRETTY_FUNCTION__ << "(" << __LINE__ << ")]::" << x << std::endl;                                                           \
        } while (0)

#    define Trace(x) TRACE(x)
#endif // NO_LOGGER
#endif
