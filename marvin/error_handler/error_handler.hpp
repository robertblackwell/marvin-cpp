#ifndef marvin_error_handler_error_handler_hpp
#define marvin_error_handler_error_handler_hpp
#include <string>
#include <openssl/err.h>

namespace Marvin {

    /**
    * In the following macros the msg argument can be a stream expression of the form
    *
    * "something" << "somethingelse" << variable
    *
    * must not start or end with a <<
     */
    #define MARVIN_IFTRUE_MARVIN_THROW(value, msg) \
        do { \
            if(value) { \
                std::stringstream messageStream; \
                messageStream << msg ; \
                ::Marvin::errorHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, messageStream.str()); \
            } \
        } while(0);
        
    #define MARVIN_IFFALSE_MARVIN_THROW(value, msg) \
        do { \
            if(!(value)) { \
                std::stringstream messageStream; \
                messageStream << msg ; \
                ::Marvin::errorHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, messageStream.str()); \
            } \
        } while(0);

    #define MARVIN_NOT_IMPLEMENTED() Cert::errorHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, "function not implemented"); 

    #define MARVIN_THROW(msg) \
        do { \
            std::stringstream messageStream; \
            messageStream << msg ; \
            Marvin::errorHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, messageStream.str()); \
        } while(0);

    #define MARVIN_ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)

    void errorHandler (std::string func, std::string file, int lineno, std::string msg);
    /**
    * \brief Custom exception class for functions in namespace Cert::x509
    */
    class Exception : public std::exception
    {
        public:
            Exception(std::string message);
            const char* what() const noexcept;
        private:
            std::string errMessage;
    };

} //namespace Helpers

#endif
