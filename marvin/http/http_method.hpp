#ifndef marvin_http_method_hpp
#define marvin_http_method_hpp
#include <string>
#include <map>
#include <sstream>
#include <marvin/buffer/buffer.hpp>
#include<http-parser/http_parser.h>
#include <marvin/boost_stuff.hpp>

namespace Marvin {


enum class HttpMethod{
#define EC(num, name, string) name = HTTP_##name,
    HTTP_METHOD_MAP(EC)
#undef EC

};
std::string http_method_string(enum http_method m);
std::string method_as_string(enum http_method m);
std::string http_method_string(HttpMethod m);
std::string method_as_string(HttpMethod method);

} // namespace
#endif
