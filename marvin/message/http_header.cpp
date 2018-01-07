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
#include <boost/algorithm/string/case_conv.hpp>
std::set<std::string> s{"one","two"};

#include "http_header.hpp"
using namespace Marvin;
using namespace Http;
namespace Marvin {
namespace Http {
    void Headers::canonicalKey(char* key, int length)
    {
        for(int i = 0; i < length; i++)
        {
            key[i] = toupper(key[i]);
        }
    }
    void Headers::canonicalKey(std::string& key) {  boost::algorithm::to_upper(key);}

    const std::string Headers::Name::AcceptEncoding = "ACCEPT-ENCODING";
    const std::string Headers::Name::Authorization = "AUTHORIZATION";
    const std::string Headers::Name::Connection = "CONNECTION";
    const std::string Headers::Name::ConnectionHandlerId = "CONNECT-HANDLER-ID";
    const std::string Headers::Name::ContentLength = "CONTENT-LENGTH";
    const std::string Headers::Name::ContentType = "CONTENT-TYPE";
    const std::string Headers::Name::Date = "DATE";
    const std::string Headers::Name::Host = "HOST";
    const std::string Headers::Name::ProxyConnection = "PROXY-CONNECTION";
    const std::string Headers::Name::TE = "TE";
    const std::string Headers::Name::TransferEncoding = "TRANSFER-ENCODING";
    const std::string Headers::Name::ETag = "ETAG";
    const std::string Headers::Name::RequestHandlerId = "REQUEST-HANDLER-ID";

    const std::string Headers::Value::ConnectionClose = "CLOSE";
    const std::string Headers::Value::ConnectionKeepAlive = "KEEP-ALIVE";

    const std::string Headers::Scheme::https = "https";
    const std::string Headers::Scheme::http = "http";


} // namespace Http
} //namespace Marvin

namespace HttpHeaderXX
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
        boost::algorithm::to_upper(key);
//        for (auto & c: key){
//            c = toupper(c);
//        }
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
//            auto x = filterList.find(k);
//            auto y = filterList.end();
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
