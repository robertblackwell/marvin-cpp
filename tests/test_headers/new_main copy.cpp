
// Sample custom iterator.
// By perfectly.insane (http://www.dreamincode.net/forums/index.php?showuser=76558)
// From: http://www.dreamincode.net/forums/index.php?showtopic=58468

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <cassert>
class KeyValue : public std::pair<std::string, std::string>
{
    public:
        std::string key() { return this->first;}
        std::string value() {return this->second;};
};
class OrderedKeyValues
{
    public:

        typedef int size_type;

        class Iterator
        {
            public:
                typedef Iterator self_type;
                typedef KeyValue value_type;
                typedef KeyValue& reference;
                typedef long pointer;
                typedef std::forward_iterator_tag iterator_category;
                typedef int difference_type;
            
                Iterator(OrderedKeyValues& map, long index) : m_map(map), m_index(index){ }
            
                self_type operator++() {
                    self_type i = *this;
                    m_index++;
                    return i;
                }
                self_type operator++(int junk)
                {
                    m_index++; return *this;
                }
                KeyValue& operator*()
                {
                    return m_map.m_key_value_vec[m_index];
                }
                pointer operator->()
                {
                    return m_index;
                }
                bool operator==(const self_type& rhs)
                {
                    return m_index == rhs.m_index;
                }
                bool operator!=(const self_type& rhs)
                {
                    return m_index != rhs.m_index;
                }
            private:
                OrderedKeyValues& m_map;
                long        m_index;
        };

        OrderedKeyValues() //: m_keys(std::map<std::string, long>()), m_values(std::vector<std::string>())
        {
        }

        size_type size() const { return (size_type)m_keys.size(); }

        std::string& operator[](std::string k)
        {
            KeyValue tmp;
            long i;
            if(m_keys.find(k) == m_keys.end()) {
                tmp.first = k;
                tmp.second = "";
                i = m_key_value_vec.size();
                m_keys[k] = (int)i;
                m_key_value_vec.push_back(tmp);
            } else {
                i = m_keys[k];
            }
            return m_key_value_vec[i].second;
        }
        Iterator find(std::string k)
        {
            auto vi = m_keys.find(k);
            if(vi == m_keys.end()) {
                return this->end();
            } else {
                return Iterator(*this, m_keys[k]);
            }
        }
        bool has(std::string k)
        {
            return ! (find(k) == this->end());
        }

        Iterator begin()
        {
            return Iterator(*this, 0);
        }

        Iterator end()
        {
            return Iterator(*this, (int)m_key_value_vec.size());
        }

#if 0
        const_Iterator begin() const
        {
            return const_Iterator(data_);
        }

        const_Iterator end() const
        {
            return const_Iterator(data_ + size_);
        }
#endif

    private:
        std::vector<KeyValue> m_key_value_vec;
        std::map<std::string, long> m_keys;
};
OrderedKeyValues::OrderedKeyValues() //: m_keys(std::map<std::string, long>()), m_values(std::vector<std::string>())
        {
        }

size_type OrderedKeyValues::size() const { return (size_type)m_keys.size(); }

std::string& OrderedKeyValues::operator[](std::string k)
        {
            KeyValue tmp;
            long i;
            if(m_keys.find(k) == m_keys.end()) {
                tmp.first = k;
                tmp.second = "";
                i = m_key_value_vec.size();
                m_keys[k] = (int)i;
                m_key_value_vec.push_back(tmp);
            } else {
                i = m_keys[k];
            }
            return m_key_value_vec[i].second;
        }
        OrderedKeyValues::Iterator OrderedKeyValues::find(std::string k)
        {
            auto vi = m_keys.find(k);
            if(vi == m_keys.end()) {
                return this->end();
            } else {
                return Iterator(*this, m_keys[k]);
            }
        }
        bool has(std::string k)
        {
            return ! (find(k) == this->end());
        }

        Iterator begin()
        {
            return Iterator(*this, 0);
        }

        Iterator end()
        {
            return Iterator(*this, (int)m_key_value_vec.size());
        }

#if 0
        const_Iterator begin() const
        {
            return const_Iterator(data_);
        }

        const_Iterator end() const
        {
            return const_Iterator(data_ + size_);
        }
#endif

int main()
{
    OrderedKeyValues hdrs{};
    hdrs["one"] = "header1";
    hdrs["two"] = "headers2";
    hdrs["three"] = "headers3";
    auto i = hdrs.begin();
    auto end = hdrs.end();
    auto f = hdrs.find("two");
    auto b = (f == hdrs.end());
    auto hs = hdrs.has("two");
    for(OrderedKeyValues::Iterator i = hdrs.begin(); i != hdrs.end(); i++)
    {
        auto xx = *i;
        std::cout << "key: " << (*i).key() << " value: " << (*i).value() << " " << hdrs[(*i).key()] << std::endl;
    }
    for(auto& h : hdrs){
        std::cout << h.key() << " " << h.value() << std::endl;
    }

    std::cout << std::endl;

//    std::vector<double> vec;
//    std::copy(point3d.begin(), point3d.end(), std::back_inserter(vec));
//
//    for(std::vector<double>::iterator i = vec.begin(); i != vec.end(); i++)
//    {
//        std::cout << *i << " ";
//    }
//
//    std::cout << std::endl;
    return 0;
}
