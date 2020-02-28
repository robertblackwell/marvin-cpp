#ifndef marvin_http_method_hpp
#define marvin_http_method_hpp
#include <string>
#include <map>
#include <sstream>
#include <marvin/buffer/buffer.hpp>
#include <marvin/external_src/http-parser/http_parser.h>
#include <marvin/include/boost_stuff.hpp>

/**
* \ingroup HttpMessage
* \brief Enum for HTTP method. It is a BAD kludge to get C++ enum same as http_parser c enum;
* usage HttpMethod::GET -- see http_parser.h for list of names
*/
enum class HttpMethod{
#define EC(num, name, string) name = HTTP_##name,
    HTTP_METHOD_MAP(EC)
#undef EC
};
std::string httpMethodString(HttpMethod m);
#endif
