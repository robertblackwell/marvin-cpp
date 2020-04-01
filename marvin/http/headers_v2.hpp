#ifndef guard_marvin_new_headers_hpp
#define guard_marvin_new_headers_hpp
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <boost/optional/optional.hpp>


namespace Marvin {
namespace Http {
/**
 * A class that represents http headers in request and response messages.
 * Maintains an ordered list of std::pair<std::string, std::string>
 * To represent the header lines of an http message.
 * They are maintained in the order they are added.
 * 
 * Keys are case insensitive and hence are converted to upper case 
 * whenever used or stored.
 * 
 * Values are case sensitive, or at least this class does not mess with the case except
 * in regard to keep-alive/close  and chunked. In those circumstances all possible case 
 * variations are considered.  
 * 
 */
class HeadersV2
{
    public:
        
        class Exception: public std::exception
        {
            public:
                Exception(std::string message) : marvin_message(message){}
                const char* what() const noexcept {return marvin_message.c_str();}
            protected:
                std::string marvin_message;
        };

        typedef std::pair<std::string, std::string> Pair;

        class Iterator
        {
            public:
                Iterator(HeadersV2& headers, long index = 0) : m_headers(headers), m_index(index){ }
            
                Iterator operator++();
                Iterator operator++(int junk);
                Pair& operator*();
                Pair* operator->();
                Iterator operator=(const Iterator& rhs);
                bool operator==(const Iterator& rhs);
                bool operator!=(const Iterator& rhs);
            private:
                HeadersV2& m_headers;
                // std::vector<std::pair<std::string, std::string>>& m_vector;
                long     m_index;
        };
        // commonly used header keys
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
        // important connect header values
        static const std::string ConnectionClose;
        static const std::string ConnectionKeepAlive;

        HeadersV2();
        /// \brief This construct provide a means of initializing a OrderedKeyValues object
        /// using a literal value; specifically a vector of pairs of string values.
        HeadersV2(std::vector<std::pair<std::string, std::string>> initialValue);

        /** Returns the number of header key-value pairs*/
        std::size_t size() const;
        
        /** Gets a header key and value pair by index - header order is maintained.
         * Will throw an exception if the index is out of range
        */
        std::pair<std::string, std::string> atIndex(std::size_t index);

        boost::optional<std::size_t> findAtIndex(std::string key);

        /** gets the current value for a key or undefined if there is not one. 
         * Returns this as an optional so that existence can be tested at the 
         * same time as getting the value
         * */ 
        boost::optional< std::string> atKey(std::string k);

        /** will add a new header keya dn value or will update an existing key with a new value*/
        void setAtKey(std::string k, std::string v);

        /** returns true if there is a header with the given key
         * Made redundant by atKey returning optional
        */
        bool hasKey(std::string k);

        /**
         * Removes the header with the given key value. If there was no such header
         * no action is taken.
         * */
        void removeAtKey(std::string k);

        /** deprecated*/
        void erase(std::string k);

        /** Iterator base find function*/
        Iterator find(std::string k);
        Iterator begin();
        Iterator end();

    private:
        void eraseAtIndex(std::size_t position);

        std::vector<Pair> m_pairs_vector;
        // std::map<std::string, long> m_keys;
};
} //namespace
} //namespace
#endif