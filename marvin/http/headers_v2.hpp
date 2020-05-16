#ifndef guard_marvin_new_headers_hpp
#define guard_marvin_new_headers_hpp
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <boost/optional/optional.hpp>


namespace Marvin {

class HeadersV2;

// test a string for a token 'keep-alive' case insensitive
bool isConnectionKeepAlive(std::string value);
// test a string for a token 'close' case insensitive
bool isConnectionClose(std::string value);

// tests whether a header structure has a connection header with a 'keep-alive' substring
bool isConnectionKeepAlive(Marvin::HeadersV2& h);
// tests whether a header structure has a connection header with a 'close' substring
bool isConnectionClose(Marvin::HeadersV2& h);


/**
 * \ingroup http
 * \brief A class that represents http header fields in request and response messages.
 *
 * Maintains an ordered list of std::Field<std::string, std::string>
 * To represent the header lines of an http message.
 * They are maintained in the order they are added.
 * 
 * Keys are case insensitive. They are converted to upper case
 * whenever used or stored. Keys are always passed by ref and key values
 * should always be one of the string constants defined below.
 * 
 * Values are case sensitive, or at least this class does not mess with the case except
 * in regard to keep-alive/close,  chunked and upgrade. In those circumstances all possible case
 * variations are considered.  
 * 
 */
class HeadersV2
{
    public:

        /// \ingroup HttpMessage
        /// \brief  Filters a header map to remove all headers whose keys are IN the filterList
        static void copyExcept(HeadersV2& source, HeadersV2& dest, std::set<std::string> filterList);
        
        class Exception: public std::exception
        {
            public:
                explicit Exception(std::string message) : marvin_message(message){}
                [[nodiscard]] const char* what() const noexcept override {return marvin_message.c_str();}
            protected:
                std::string marvin_message;
        };
        typedef std::vector<std::pair<std::string, std::string>>  Initializer;

        using FieldKeyArg = const std::string; /** type for function arguments */
        struct Field
        {
            using KeyType = std::string;
            KeyType     key;
            std::string value;
        };
        class Iterator
        {
            public:
                explicit Iterator(HeadersV2& headers, long index = 0) : m_headers(headers), m_index(index){ }
            
                Iterator operator++();
                Iterator const operator++(int junk);
                Field& operator*();
                Field* operator->();
                Iterator& operator=(const Iterator& rhs);
                bool operator==(const Iterator& rhs);
                bool operator!=(const Iterator& rhs);
            private:
                HeadersV2& m_headers;
                // std::vector<std::Field<std::string, std::string>>& m_vector;
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
        static const std::string ProxyAuthorization;
        static const std::string ProxyAuthentication;
        static const std::string TransferEncoding;
        static const std::string ETag;
        static const std::string Upgrade;
        static const std::string TE;
        static const std::string Trailer;
        static const std::string RequestHandlerId;
        // important connect header values
        static const std::string ConnectionClose;
        static const std::string ConnectionKeepAlive;
        HeadersV2();
        HeadersV2(HeadersV2& other);
        HeadersV2(HeadersV2&& other) noexcept ;
        HeadersV2& operator =(HeadersV2 const& other);
        HeadersV2& operator =(HeadersV2&& other);

        /**
         * \brief This constructor provide a means of initializing a HeadersV2 object
         * using a literal value; specifically a vector of Fields of string values.
         */
         explicit HeadersV2(std::vector<std::pair<std::string, std::string>> initialValue);

        /** Returns the number of header key-value Fields*/
        std::size_t size() const;
        
        /**
         * \brief Gets a header <key value> Field by index - header order is maintained.
         * Will throw an exception if the index is out of range
        */
        Field atIndex(std::size_t index);
        /**
         * \brief finds the index of a header by key.
         * @return optional value
         */
        boost::optional<std::size_t> findAtIndex(FieldKeyArg key);

        /**
         * \brief Gets the current value for a key or undefined if there is not one.
         * Returns this as an optional so that existence can be tested at the 
         * same time as getting the value
         */
        boost::optional<std::string> atKey(FieldKeyArg k);

        /**
         * \brief add a new header <key value> will update an existing key with a new value
         */
        void setAtKey(FieldKeyArg k, std::string v);

        /**
         * \brief Removes the header with the given key value. If there was no such header
         * no action is taken.
         * */
        void removeAtKey(FieldKeyArg k);

        /** Iterator base find function*/
        Iterator find(FieldKeyArg k);
        Iterator begin();
        Iterator end();
        std::string str();
        friend std::ostream &operator<< (std::ostream &os, HeadersV2 &headers);

        /// used for testing
        bool sameValues(HeadersV2& other);
        bool sameOrderAndValues(HeadersV2& other);

    private:
        void eraseAtIndex(std::size_t position);

        std::vector<Field> m_fields_vector;
        // std::map<std::string, long> m_keys;
};
} //namespace
#endif