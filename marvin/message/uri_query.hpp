#ifndef marvin_query_parser_hpp
#define marvin_query_parser_hpp

#include <stdio.h>
#include <string>
#include <map>
/// \ingroup HttpMessage
void printMap(std::map<std::string, std::string> m);

/// \ingroup HttpMessage
class UriQuery
{
public:
    UriQuery(std::string q_str);
    void parse(std::string s);
    std::string& str();
    std::map<std::string, std::string>& keyValues();
private:
    std::string                         _queryString;
    std::map<std::string, std::string>  _keyValues;
    
};

#endif /* query_parser_hpp */
