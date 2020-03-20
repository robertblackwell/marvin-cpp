#include <map>
#include <marvin/http/http_method.hpp>

std::string httpMethodString(HttpMethod m){

    enum http_method c_m = (enum http_method) m;
    std::string s(http_method_str(c_m));
    return s;
}
