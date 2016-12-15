//
//  query_parser.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/14/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef query_parser_hpp
#define query_parser_hpp

#include <stdio.h>
#include <string>
#include <map>
void printMap(std::map<std::string, std::string> m);

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
