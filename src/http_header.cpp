//
//  http_header.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/26/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <map>
#include <vector>
#include <set>

std::set<std::string> s{"one","two"};

#include "http_header.hpp"

namespace HttpHeader
{
    void canonicalKey(char* key, int length)
    {
        for(int i = 0; i < length; i++)
        {
            key[i] = toupper(key[i]);
        }
    }
    void canonicalKey(std::string& key)
    {
        for (auto & c: key){
            c = toupper(c);
        }
    }
    void filterOut(HttpHeadersType& hdrs, std::set<std::string> filterList)
    {
    
        for(auto it = hdrs.begin(); it != hdrs.end(); )
        {
            std::string k = it->first;
            if( filterList.end() != filterList.find(k) )
            {
                // IS in filterList so remove from headers
                hdrs.erase(it);
            }
        }
    }

    void filterNotInList(HttpHeadersType& hdrs,
            HttpHeaderFilterSetType filterList,
            std::function<void(HttpHeadersType& hdrs, std::string key, std::string value)> cb)
    {
    
        for(auto it = hdrs.begin(); it != hdrs.end(); it++ )
        {
            std::string k = it->first;
            auto x = filterList.find(k);
            auto y = filterList.end();
            if( filterList.end() == filterList.find(k) )
            {
                // IS in filterList so remove from headers
//                hdrs.erase(it);
                cb(hdrs, it->first, it->second);
            }
        }
    }


    void filterIn(HttpHeadersType& hdrs, std::set<std::string> filterList)
    {
    
        for(auto it = hdrs.begin(); it != hdrs.end(); )
        {
            std::string k = it->first;
            if( filterList.end() == filterList.find(k) )
            {
                // is NOT in filterlist so remove it
                hdrs.erase(it);
            }
        }
    }

    void filterInList(HttpHeadersType&  hdrs,
            HttpHeaderFilterSetType filterList,
            std::function<void(HttpHeadersType& hdrs, std::string key, std::string value)> cb)
    {
    
        for(auto it = hdrs.begin(); it != hdrs.end(); it++)
        {
            std::string k = it->first;
            if( filterList.end() != filterList.find(k) )
            {
                // is NOT in filterlist so remove it
                hdrs.erase(it);
                cb(hdrs, it->first, it->second);
            }
        }
    }

};
