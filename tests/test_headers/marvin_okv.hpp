#ifndef marvin_okv_hpp
#define marvin_okv_hpp
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <cassert>
//class KeyValue : public std::pair<std::string, std::string>
//{
//    public:
//        std::string key() { return this->first;}
//        std::string value() {return this->second;};
//};
/// \brief this class provides a collection key/value pairs that are iterable in, and retains, the order in which the
/// key/values were added. This will be the basis of a http header data structure which needs to preserve order
class OrderedKeyValues
{
    public:
        typedef int size_type;
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
        /// \brief construct from a vector of key/value pairs
        OrderedKeyValues(std::vector<std::pair<std::string, std::string>> initialValue);

        size_type size() const;
    
        std::string& operator[](std::string k);
        bool operator==(const OrderedKeyValues& rhs);
        bool operator!=(const OrderedKeyValues& rhs);
        std::string get(std::string k);
        void set(std::string k, std::string v);
        void remove(std::string k);
        void remove(std::vector<std::string> keys);
        Iterator find(std::string k);
        bool has(std::string k);
        Iterator begin();
        Iterator end();

    private:
        void p_rebuild_index();
        std::vector<KVPair> m_key_value_vec;
        std::map<std::string, long> m_keys;
};
#endif

