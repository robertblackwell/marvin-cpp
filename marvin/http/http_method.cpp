#include <map>
#include <marvin/http/http_method.hpp>
namespace Marvin {
std::string http_method_string(HttpMethod m)
{

    enum http_method c_m = (enum http_method) m;
    std::string s(http_method_str(c_m));
    return s;
}
std::string method_as_string(HttpMethod m)
{
    enum http_method c_m = (enum http_method) m;
    std::string s(http_method_str(c_m));
    return s;
}
std::string http_method_string(enum http_method m)
{
    std::string s(http_method_str(m));
    return s;
}
std::string method_as_string(enum http_method m)
{
    std::string s(http_method_str(m));
    return s;
}
} // namespace
