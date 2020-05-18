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

class HeaderFields;

// test a string for a token 'keep-alive' case insensitive
bool is_connection_keep_alive(std::string value);
// test a string for a token 'close' case insensitive
bool is_connection_close(std::string value);

// tests whether a header structure has a connection header with a 'keep-alive' substring
bool is_connection_keep_alive(Marvin::HeaderFields& h);
// tests whether a header structure has a connection header with a 'close' substring
bool is_connection_close(Marvin::HeaderFields& h);


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
class HeaderFields
{
    public:

        /// \ingroup HttpMessage
        /// \brief  Filters a header map to remove all headers whose keys are IN the filterList
        static void copy_except(HeaderFields& source, HeaderFields& dest, std::set<std::string> filterList);
        
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
            std::string key;
            std::string value;
            /// best way I found of forcing a steal of memory from source
            /// used in praser only
            Field(std::string* k, std::string* v)
            {
                std::swap(*v, value);
                std::swap(*k, key);
            }
            Field(std::string const & k, std::string const& v): key(k), value(v)
            {
            }

            Field(std::string&& k, std::string&& v): key(k), value(v)
            {
            }
        };
        class Iterator
        {
            public:
                explicit Iterator(HeaderFields& headers, long index = 0) : m_headers(headers), m_index(index){ }
            
                Iterator operator++();
                Iterator const operator++(int junk);
                Field& operator*();
                Field* operator->();
                Iterator& operator=(const Iterator& rhs);
                bool operator==(const Iterator& rhs);
                bool operator!=(const Iterator& rhs);
            private:
                HeaderFields& m_headers;
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
        HeaderFields();
        HeaderFields(HeaderFields& other);
        HeaderFields(HeaderFields&& other) noexcept ;
        HeaderFields& operator =(HeaderFields const& other);
        HeaderFields& operator =(HeaderFields&& other);

        /**
         * \brief This constructor provide a means of initializing a HeaderFields object
         * using a literal value; specifically a vector of Fields of string values.
         */
         explicit HeaderFields(std::vector<std::pair<std::string, std::string>> initialValue);

        /** Returns the number of header key-value Fields*/
        std::size_t size() const;
        
        /**
         * \brief Gets a header <key value> Field by index - header order is maintained.
         * Will throw an exception if the index is out of range
        */
        Field at_index(std::size_t index);
        /**
         * \brief finds the index of a header by key.
         * @return optional value
         */
        boost::optional<std::size_t> find_at_index(FieldKeyArg key);

        /**
         * \brief Gets the current value for a key or undefined if there is not one.
         * Returns this as an optional so that existence can be tested at the 
         * same time as getting the value
         */
        boost::optional<std::string> at_key(FieldKeyArg k);

        /**
         * \brief add a new header <key value> will update an existing key with a new value
         */
        void set_at_key(FieldKeyArg k, std::string v);
        void set_at_key(std::string* k, std::string* v);

        /**
         * \brief Removes the header with the given key value. If there was no such header
         * no action is taken.
         * */
        void remove_at_key(FieldKeyArg k);

        /** Iterator base find function*/
        Iterator find(FieldKeyArg k);
        Iterator begin();
        Iterator end();
        std::string str();
        friend std::ostream &operator<< (std::ostream &os, HeaderFields &headers);

        /// used for testing
        bool same_values(HeaderFields& other);
        bool same_order_and_values(HeaderFields& other);

    private:
        void erase_at_index(std::size_t position);

        std::vector<Field> m_fields_vector;
        // std::map<std::string, long> m_keys;
};
} //namespace
#endif