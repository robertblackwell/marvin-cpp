#ifndef marvin_http_header_hpp
#define marvin_http_header_hpp

#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include <set>
#include "marvin_okv.hpp"

/// \ingroup HttpMessage
/// Alias for a map of strings, this is the data structure that represents the headers
/// in a http message. Probably inadequate for general use as does not cope well with
/// headers that have multiple value separated by ';'
/// \ingroup HttpMessage

using HttpHeaderFilterSetType = std::set<std::string>;

/// \ingroup HttpMessage
namespace Marvin {
namespace Http {
    class Headers : public std::map<std::string, std::string>
    {
        public:
            /// \ingroup HttpMessage
            /// \brief Converts a c-string header key to canonical form
            static void canonicalKey(char* key, int length);

            /// \ingroup HttpMessage
            /// \brief  Converts a std::string header key to canonical form
            static void canonicalKey(std::string& key);
            /// \ingroup HttpMessage
            /// \brief  Filters a header map to remove all headers whose keys are IN the filterList
            static void copyExcept(Marvin::Http::Headers& source, Marvin::Http::Headers& dest, std::set<std::string> filterList);
#if 0
            /// \ingroup HttpMessage
            /// \brief  Filters a header map to remove all headers whose keys are IN the filterList
            static void removeInList(Marvin::Http::Headers& hdrs, std::vector<std::string> filterList);
//
//            static void removeHeadersInList(Marvin::Http::Headers& hdrs, std::vector<std::string> list);

            /// \ingroup HttpMessage
            /// \brief  Filters a header map to remove all headers whose keys are NOT IN the filterList;
            static void keepInList(Marvin::Http::Headers& hdrs, std::set<std::string> filterList);
//            static void removeHeadersNotInList(Marvin::Http::Headers& hdrs, std::vector<std::string> list);

            /// \ingroup HttpMessage
            /// \brief  Filters a header map, calls the cb function for all headers
            /// whose keys ARE NOT IN the filterList
            static void filterNotInList(
                Marvin::Http::Headers&        hdrs,
                HttpHeaderFilterSetType list,
                std::function<void(Marvin::Http::Headers& hdrs,
                                    std::string key,
                                    std::string value)> cb);

            /// \ingroup HttpMessage
            /// \brief Filters a header map, calls the cb function for all headers
            /// whose keys ARE IN the filterList
            static void filterInList(
                Marvin::Http::Headers&        hdrs,
                HttpHeaderFilterSetType list,
                std::function<void(Marvin::Http::Headers& hdrs,
                                    std::string key,
                                    std::string value)> cb);

#endif
        
            class Name {
                public:
                static const std::string AcceptEncoding;
                static const std::string Authorization;
                static const std::string Connection;
                static const std::string ConnectionHandlerId;
                static const std::string ContentLength;
                static const std::string ContentType;
                static const std::string Date;
                static const std::string Host;
                static const std::string ProxyConnection;
                static const std::string TE;
                static const std::string TransferEncoding;
                static const std::string ETag;
                static const std::string RequestHandlerId;
            };
            /// \ingroup HttpMessage
            class Value{
                public:
                static const std::string ConnectionClose;
                static const std::string ConnectionKeepAlive;
            };
            typedef std::string SchemeType;
            class Scheme {
                public:
                static const std::string https;
                static const std::string http;
            };

    };
}
}
#endif /* http_header_hpp */
