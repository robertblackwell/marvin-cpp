#ifndef marvin_okv_hpp
#define marvin_okv_hpp
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <marvin/external_src/json.hpp>

//class KeyValue : public std::pair<std::string, std::string>
//{
//    public:
//        std::string key() { return this->first;}
//        std::string value() {return this->second;};
//};
/// \brief This class provides a collection of key/value pairs that are iterable in, and retain, the order in which the
/// key/values were added; this will be the basis of a http header data structure which needs to preserve order.
class OrderedKeyValues;
void to_json(nlohmann::json& j, const OrderedKeyValues& kv);
void from_json(const nlohmann::json& j, OrderedKeyValues& kv);
class OrderedKeyValues
{
    public:
        static void to_json(nlohmann::json& j, const OrderedKeyValues& kv);
        static void from_json(const nlohmann::json& j, OrderedKeyValues& kv);

        typedef int size_type;
        /// \brief holds an individual key/value pair
        class KVPair : public std::pair<std::string, std::string>
        {
            public:
                std::string key() { return this->first;}
                std::string value() {return this->second;};
        };

        class Iterator
        {
            public:
                typedef Iterator self_type;
                typedef KVPair value_type;
                typedef KVPair& reference;
                typedef long pointer;
                typedef std::forward_iterator_tag iterator_category;
                typedef int difference_type;
            
                Iterator(OrderedKeyValues& map, long index) : m_map(map), m_index(index){ }
            
                self_type operator++();
                self_type operator++(int junk);
                KVPair& operator*();
                KVPair* operator->();
                bool operator==(const self_type& rhs);
                bool operator!=(const self_type& rhs);
            private:
                OrderedKeyValues& m_map;
                long        m_index;
        };

        OrderedKeyValues();
        /// \brief This construct provide a means of initializing a OrderedKeyValues object
        /// using a literal value; specifically a vector of pairs of string values.
#define OKVMAP
#ifdef OKVMAP
        OrderedKeyValues(std::vector<std::pair<std::string, std::string>> initialValue);
#else
        OrderedKeyValues(std::map<std::string, std::string> initialValue);
#endif
        size_type size() const;
    
        std::string& operator[](std::string k);
        bool operator==(const OrderedKeyValues& rhs);
        bool operator!=(const OrderedKeyValues& rhs);
        std::string get(std::string k);
        void set(std::string k, std::string v);
        void remove(std::string k);
        void erase(std::string k);
        void remove(std::vector<std::string> keys);
        Iterator find(std::string k);
        bool has(std::string k);
        std::vector<std::pair<std::string, std::string>> jsonizable() const;
        Iterator begin();
        Iterator end();

    private:
        void p_rebuild_index();
        std::vector<KVPair> m_key_value_vec;
        std::map<std::string, long> m_keys;
};
#endif

