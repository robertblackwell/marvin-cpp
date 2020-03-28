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

void RoundTripTestCaseBase::verifyResponse(Marvin::ErrorType& err, Marvin::Http::MessageBaseSPtr response)
{
    throw "should be overriden";
}
Marvin::Http::MessageBaseSPtr RoundTripTestCaseBase::makeRequest(
        std::string path,   // the string that goes after the method usually for non proxy requests a relative path like /echo/smart
        std::string scheme, // http or https
        std::string host,   // host name without the port so localhost not localhost:3000
        std::string port    // port such as 3000
)
{
    throw "should be overriden";
    return nullptr;
}
