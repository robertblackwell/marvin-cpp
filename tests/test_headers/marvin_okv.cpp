#include "marvin_okv.hpp"

#pragma mark - implementation of Iterator
using OKV = OrderedKeyValues;
using OI = OrderedKeyValues::Iterator;
OI::self_type OI::operator++()
{
    self_type i = *this;
    m_index++;
    return i;
}
OI::self_type OI::operator++(int junk)
{
    m_index++; return *this;
}
OKV::KVPair& OI::operator*()
{
    return m_map.m_key_value_vec[m_index];
}
OrderedKeyValues::KVPair* OI::operator->()
{
    return &(m_map.m_key_value_vec[m_index]);
}
bool OI::operator==(const self_type& rhs)
{
    return m_index == rhs.m_index;
}
bool OI::operator!=(const self_type& rhs)
{
    return m_index != rhs.m_index;
}


#pragma mark - implementation of OrderedKeyValues
OKV::OrderedKeyValues()
{
}

OKV::OrderedKeyValues(std::vector<std::pair<std::string, std::string>> initialValue)
{
    typedef std::vector<std::pair<std::string, std::string>> okv_init;
    for(okv_init::iterator it = initialValue.begin(); it != initialValue.end(); it++) {
        set((*it).first, (*it).second);
    }
}

OKV::size_type OKV::size() const { return (size_type)m_keys.size(); }

std::string& OKV::operator[](std::string k)
{
    OKV::KVPair tmp;
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
bool OKV::operator==(const OrderedKeyValues& rhs)
{
    return (m_key_value_vec == rhs.m_key_value_vec);
}
bool OKV::operator!=(const OrderedKeyValues& rhs)
{
    return (m_key_value_vec != rhs.m_key_value_vec);
}
std::string OKV::get(std::string k)
{
    assert(has(k));
    return (*this)[k];
}
void OKV::set(std::string k, std::string v)
{
    (*this)[k] = v;
}
void OKV::remove(std::string k)
{
    auto it = find(k);
    if(it != (*this).end()) {
        auto kit = m_keys.find(k);
        auto i = (*kit).second;
        m_key_value_vec.erase(m_key_value_vec.begin() + i);
        typedef std::vector<std::pair<std::string, std::string>> okv_init;
        /// have to rebuild the index
        m_keys.clear();
        int index = 0;
        for(okv_init::iterator it = m_key_value_vec.begin(); it != m_key_value_vec.end(); it++) {
            m_keys[(*it).first] = index;
            index++;
        }
    }
}
void OKV::remove(std::vector<std::string> keys)
{

    typedef std::vector<std::pair<std::string, std::string>> okv_init;
    std::vector<long> to_remove;
    for(std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); it++) {
        std::string k = (*it);
        long idx = 0;
        for(okv_init::const_iterator kvit = m_key_value_vec.begin(); kvit != m_key_value_vec.end(); kvit++) {
            if((*kvit).first == k) {
                m_key_value_vec.erase(m_key_value_vec.cbegin() + idx);
                break;
            }
            idx++;
        }
    }
    m_keys.clear();
    /// have to rebuild the index
    int index = 0;
    for(okv_init::iterator it = m_key_value_vec.begin(); it != m_key_value_vec.end(); it++) {
        m_keys[(*it).first] = index;
        index++;
    }
}
OKV::Iterator OrderedKeyValues::find(std::string k)
{
    auto vi = m_keys.find(k);
    if(vi == m_keys.end()) {
        return this->end();
    } else {
        return Iterator(*this, m_keys[k]);
    }
}
bool OKV::has(std::string k)
{
    return ! (find(k) == this->end());
}

OKV::Iterator OrderedKeyValues::begin()
{
    return Iterator(*this, 0);
}

OKV::Iterator OrderedKeyValues::end()
{
    return Iterator(*this, (int)m_key_value_vec.size());
}
void OKV::p_rebuild_index()
{
    typedef std::vector<std::pair<std::string, std::string>> okv_init;
    m_keys.clear();
    /// have to rebuild the index
    int index = 0;
    for(okv_init::iterator it = m_key_value_vec.begin(); it != m_key_value_vec.end(); it++) {
        m_keys[(*it).first] = index;
        index++;
    }
}
