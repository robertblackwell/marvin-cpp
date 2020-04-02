//
//  http_header.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/26/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include <ctype.h>
#include <map>
#include <set>
#include <boost/algorithm/string/case_conv.hpp>
#include <json/json.hpp>

std::set<std::string> s{"one","two"};

#include <marvin/http/http_header.hpp>

using namespace Marvin;
using namespace Http;
using namespace nlohmann;

namespace Marvin {
namespace Http {
    void to_json(json& j, const Headers& h)
    {
//        std::cout << __FUNCTION__ <<" " <<  __FILE__ <<  " " << __LINE__ <<std::endl;
        const OrderedKeyValues& okv = h;
        ::to_json(j, okv);
    }
    void from_json(const json& j, Headers& h)
    {
        OrderedKeyValues& okv = h;
//        std::cout << __FUNCTION__ <<" " <<  __FILE__ <<  " " << __LINE__ <<std::endl;
        ::from_json(j, okv);
    }

    Headers::Headers() : OrderedKeyValues() {}
//    Headers::Headers(std::vector<std::pair<std::string, std::string>> vals) : OrderedKeyValues(vals) {}
    Headers::Headers(Marvin::Http::Headers::Initializer vals) : OrderedKeyValues(vals) {}

    void Headers::canonicalKey(char* key, int length)
    {
        for(int i = 0; i < length; i++)
        {
            key[i] = toupper(key[i]);
        }
    }
    void Headers::canonicalKey(std::string& key) {  boost::algorithm::to_upper(key);}
    
    void Headers::copyExcept(Marvin::Http::Headers& source, Marvin::Http::Headers& dest, std::set<std::string> filterList)
    {
        for(auto it = source.begin(); it != source.end(); it++) {
            std::string k = it->first;
            if( filterList.end() == filterList.find(k) ) {
                /// if not in list copy
                dest[it->first] = it->second;
            }
        }
    }
#if 0
    
    static void removeInList(Marvin::Http::Headers& hdrs, std::set<std::string> filterList) {
        for(auto it = hdrs.begin(); it != hdrs.end(); ) {
            std::string k = it->first;
            if( filterList.end() != filterList.find(k) ) {
                // IS in filterList so remove from headers
                hdrs.erase(it);
            }
        }
    }

    void Headers::filterNotInList(Marvin::Http::Headers& hdrs,
            HttpHeaderFilterSetType filterList,
            std::function<void(Marvin::Http::Headers& hdrs, std::string key, std::string value)> cb)
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


    void Headers::keepInList(Marvin::Http::Headers& hdrs, std::set<std::string> filterList)
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

    void Headers::filterInList(Marvin::Http::Headers&  hdrs,
            HttpHeaderFilterSetType filterList,
            std::function<void(Marvin::Http::Headers& hdrs, std::string key, std::string value)> cb)
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

#endif

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

