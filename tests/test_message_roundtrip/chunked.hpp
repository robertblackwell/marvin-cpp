#ifndef marvin_tests_guard_chunked_hpp
#define marvin_tests_guard_chunked_hpp
// #include <doctest/doctest.h>

#include <boost/asio.hpp>
// #include <boost/algorithm/string.hpp>
// #include <json/json.hpp>
// #include <marvin/buffer/buffer_chain.hpp>
// #include <marvin/http/uri.hpp>
#include <marvin/http/message_base.hpp>
// #include <marvin/http/message_factory.hpp>
#include <marvin/error/marvin_error.hpp>
// #include <marvin/helpers/helpers_fs.hpp>
// #include <marvin/collector/collector_base.hpp>
// #include <marvin/forwarding/forward_helpers.hpp>
#include "tm_roundtrip_testcase_base.hpp"

class Chunked
{
public:
    Chunked(
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
