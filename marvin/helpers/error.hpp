#ifndef marvin_helpers_error_include_hpp
#define marvin_helpers_error_include_hpp
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
    #define IFTRUE_THROW(value, msg) \
        do { \
            if(value) { \
                std::stringstream messageStream; \
                messageStream << msg ; \
                ::Marvin::errorHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, messageStream.str()); \
            } \
        } while(0);
        
    #define IFFALSE_THROW(value, msg) \
        do { \
            if(!(value)) { \
                std::stringstream messageStream; \
                messageStream << msg ; \
                ::Marvin::errorHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, messageStream.str()); \
            } \
        } while(0);

    #define THROW(msg) \
        do { \
            std::stringstream messageStream; \
            messageStream << msg ; \
            ::Marvin::errorHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, messageStream.str()); \
        } while(0);

    #define NOT_IMPLEMENTED() Cert::errorHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, "function not implemented"); 

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
