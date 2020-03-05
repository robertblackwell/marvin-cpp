//
//  query_parser.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/14/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <marvin/http/uri_query.hpp>
#include <regex>
using namespace Marvin;

void printMap(std::map<std::string, std::string> m)
{
    for(auto const& ent: m){
        printf("key: %s value: %s \n ", ent.first.c_str(), ent.second.c_str());
    }
}
UriQuery::UriQuery(std::string q_str)
{
    _queryString = q_str;
    parse(_queryString);
//    printMap(_keyValues);
    
}
std::string&
UriQuery::str(){return _queryString;}

std::map<std::string, std::string>&
UriQuery::keyValues(){return _keyValues;}

void
UriQuery::parse(std::string s)
{
    std::regex pattern("([\\w+%]+)=([^&]*)");
    auto words_begin = std::sregex_iterator(s.begin(), s.end(), pattern);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; i++)
    {
        std::string key = (*i)[1].str();
        std::string value = (*i)[2].str();
        _keyValues[key] = value;
    }
//    printMap(_keyValues);
}
