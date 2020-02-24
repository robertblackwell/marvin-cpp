#ifndef cert_x509_error_include_hpp
#define cert_x509_error_include_hpp

namespace Cert {
namespace x509 {
#pragma mark - error

    /**
     * Error handlers for Cert::x509 functions - use the folowing macro to report an error.
     * Captures file and line number, reads crypto error messages,
     * packs all that into a Cert::x509::Exception and throws that exception
     */
    #define X509_TRIGGER_ERROR(msg) Cert::x509::Error_Handler(__FILE__, __LINE__, msg)

    void Error_Handler (std::string file, int lineno, std::string msg);
    /**
    * \brief Custom exception class for functions in namespace Cert::x509
    */
    class Exception : public std::exception
    {
        public:
            Exception(std::string message);
            const char* what() const noexcept;
        private:
            std::string x509_ErrMessage;
    };

    
} // namespace x509
} //namespace Cert

#endif
