#ifndef marvin_helper_macros_include_hpp
#define marvin_helper_macros_include_hpp
#include <marvin/error/exception.hpp>

#ifndef NDEBUG
    #define MTHROW(msg) \
        do { \
            std::stringstream messageStream; \
            messageStream << msg ; \
            Marvin::errorHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, messageStream.str()); \
        } while(0);

#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif
#endif
