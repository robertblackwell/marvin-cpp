#ifndef marvin_tests_guard_chunked_error_hpp
#define marvin_tests_guard_chunked_error_hpp
// #include <doctest/doctest.h>

#include <boost/asio.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/error/marvin_error.hpp>

class ChunkedError
{
public:
    ChunkedError(
            std::string path,   // the string that goes after the method usually for non proxy requests a relative path like /echo/smart
            HttpMethod  method,
            std::string scheme, // http or https
            std::string host,   // host name without the port so localhost not localhost:3000
            std::string port,    // port such as 3000
            std::string body
    );
    void verifyResponse(Marvin::ErrorType& err, Marvin::Http::MessageBaseSPtr response);
    Marvin::Http::MessageBaseSPtr makeRequest();
    Marvin::BufferChainSPtr makeBody();
    std::string getHost();
    std::string getPort();
protected:
    Marvin::Http::MessageBaseSPtr   m_request_sptr;
    Marvin::Http::MessageBaseSPtr   m_response_sptr;
    std::string                     m_path;
    HttpMethod                      m_method;
    std::string                     m_scheme;
    std::string                     m_host;
    std::string                     m_port;
    std::string                     m_body;
};
#endif
