#include <set>
#include <regex>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <marvin/http/headers_v2.hpp>

namespace {
    void toupper(std::string& s ) {
        boost::to_upper(s);
    }
    std::string toupper_copy(std::string& s) {
        return boost::to_upper_copy(s);
    }
}

namespace Marvin {
const std::string HeaderFields::AcceptEncoding = "ACCEPT-ENCODING";
const std::string HeaderFields::Authorization = "AUTHORIZATION";
const std::string HeaderFields::Connection = "CONNECTION";
const std::string HeaderFields::ConnectionHandlerId = "CONNECT-HANDLER-ID";
const std::string HeaderFields::ContentLength = "CONTENT-LENGTH";
const std::string HeaderFields::ContentType = "CONTENT-TYPE";
const std::string HeaderFields::Date = "DATE";
const std::string HeaderFields::Host = "HOST";
const std::string HeaderFields::ProxyAuthorization = "PROXY-AUTHORIZATION";
const std::string HeaderFields::ProxyAuthentication = "PROXY-AUTHENTICATION";
const std::string HeaderFields::ProxyConnection = "PROXY-CONNECTION";
const std::string HeaderFields::TE = "TE";
const std::string HeaderFields::TransferEncoding = "TRANSFER-ENCODING";
const std::string HeaderFields::ETag = "ETAG";
const std::string HeaderFields::Upgrade = "UPGRADE";
const std::string HeaderFields::Trailer = "TRAILER";
const std::string HeaderFields::RequestHandlerId = "REQUEST-HANDLER-ID";

const std::string HeaderFields::ConnectionClose = "CLOSE";
const std::string HeaderFields::ConnectionKeepAlive = "KEEP-ALIVE";

bool is_connection_keep_alive(std::string value)
{
    std::regex r( R"((\s*)(,|;|\s|^)(\s*)keep-alive(\s*)(,|;|\s|$))", std::regex::icase);
    auto x = std::regex_search(value, r);
    return x;
}
// test whether a header string value has a 'close' substring
bool is_connection_close(std::string value)
{
    std::regex r(R"((\s*)(,|;|\s|^)(\s*)close(\s*)(,|;|\s|$))", std::regex::icase);
    auto x = std::regex_search(value, r);
    return x;
}
// tests whether a header structure has a connection header with a 'keep-alive' substring
bool is_connection_keep_alive(Marvin::HeaderFields& h)
{
    boost::optional<std::string> opt = h.at_key(HeaderFields::Connection);
    if(opt) {
        std::string& s = opt.get();
        return is_connection_keep_alive(s);
    }
    return false;
}
// tests whether a header structure has a connection header with a 'close' substring
bool is_connection_close(Marvin::HeaderFields& h)
{
    boost::optional<std::string> opt = h.at_key(HeaderFields::Connection);
    if(opt) {
        std::string& s = opt.get();
        return is_connection_close(s);
    }
    return false;
}


std::ostream &operator<< (std::ostream &os, HeaderFields& headers)
{
    int num = headers.size();
    for(int i = 0; i < num; i++) {
        auto p = headers.at_index(i);
        os << " " << p.key << ": " << p.value;
    }
}

std::string HeaderFields::str()
{
    std::ostringstream os;
    os << (*this);
    return os.str();
}
void HeaderFields::copy_except(HeaderFields& source, HeaderFields& dest, std::set<std::string> filterList)
{
    for(auto it = source.begin(); it != source.end(); it++) {
        std::string k = it->key;
        if( filterList.end() == filterList.find(k) ) {
            /// if not in list copy
            dest.set_at_key(it->key, it->value);
        }
    }
}

HeaderFields::Iterator HeaderFields::Iterator::operator++()
{
    Iterator i = *this;
    m_index++;
    return i;
}
HeaderFields::Iterator const HeaderFields::Iterator::operator++(int junk)
{
    m_index++; return *this;
}
HeaderFields::Field& HeaderFields::Iterator::operator*()
{
    return m_headers.m_fields_vector[m_index];
}
HeaderFields::Field* HeaderFields::Iterator::operator->()
{
    return &(m_headers.m_fields_vector[m_index]);
}
bool HeaderFields::Iterator::operator==(const Iterator& rhs)
{
    return m_index == rhs.m_index;
}
bool HeaderFields::Iterator::operator!=(const Iterator& rhs)
{
    return m_index != rhs.m_index;
}

HeaderFields::Iterator & HeaderFields::Iterator::operator=(const HeaderFields::Iterator &rhs)
{
    return *this;
}

HeaderFields::HeaderFields()
{
}
HeaderFields::HeaderFields(HeaderFields& other): m_fields_vector(other.m_fields_vector)
{}
HeaderFields::HeaderFields(HeaderFields&& other) noexcept: m_fields_vector(std::move(other.m_fields_vector))
{}
HeaderFields& HeaderFields::operator =(HeaderFields const& other)
{
    m_fields_vector = other.m_fields_vector;
}
HeaderFields& HeaderFields::operator =(HeaderFields&& other)
{
    m_fields_vector = other.m_fields_vector;
    other.m_fields_vector.clear();
}
HeaderFields::HeaderFields(std::vector<std::pair<std::string, std::string>> initialValue)
{
    typedef std::vector<std::pair<std::string, std::string>> okv_init;
    for(int i = 0; i < initialValue.size(); i++) {
        auto x = initialValue[i];
        std::string tmp_first = toupper_copy(x.first);
        set_at_key(tmp_first, x.second);
    }
}

std::size_t HeaderFields::size() const { return (std::size_t)m_fields_vector.size(); }

HeaderFields::Iterator HeaderFields::begin()
{
    return HeaderFields::Iterator(*this, 0);
}

HeaderFields::Iterator HeaderFields::end()
{
    return HeaderFields::Iterator(*this, (int)m_fields_vector.size());
}

HeaderFields::Iterator HeaderFields::find(HeaderFields::FieldKeyArg key)
{
    HeaderFields::Iterator it{*this, 0};
    std::string s{key};
    std::string key_upper = toupper_copy(s);
    for( it = this->begin(); it != this->end(); it++ ) {
        if ((*it).key == key_upper) {
            return it;
        }
    }
    return this->end();
}

boost::optional<std::size_t> HeaderFields::find_at_index(FieldKeyArg key)
{
    std::string s{key};
    std::string key_upper = toupper_copy(s);
    for(std::size_t index = 0; index < m_fields_vector.size(); index++) {
        if (m_fields_vector[index].key == key_upper) {
            return boost::optional<std::size_t>(index);
        }
    }
    return boost::optional<std::size_t>();
}

HeaderFields::Field HeaderFields::at_index(std::size_t index)
{
    if (index >= m_fields_vector.size()) {
        throw HeaderFields::Exception("index is out of bounds accessing headers");
    } else {
        return m_fields_vector[index];
    }
}
boost::optional<std::string> HeaderFields::at_key(HeaderFields::FieldKeyArg k)
{
    boost::optional<std::size_t> index = this->find_at_index(k);
    if(index) {
        return m_fields_vector[index.get()].value;
    }
    return boost::optional<std::string>();
}
void HeaderFields::set_at_key(std::string* k, std::string* v)
{
    std::string k_upper = toupper_copy(*k);
    m_fields_vector.emplace_back(Field(&k_upper, v));
}
void HeaderFields::set_at_key(HeaderFields::FieldKeyArg k, std::string v)
{
    std::string s{k};
    std::string key_upper = toupper_copy(s);
    boost::optional<std::size_t> index = this->find_at_index(k);
    if(index) {
        m_fields_vector[index.get()].value = v;
    } else {
        m_fields_vector.emplace_back(Field(key_upper, v));
    }
}
void HeaderFields::erase_at_index(std::size_t position)
{
    if((position >= 0)&&(position < m_fields_vector.size())) {
        for(std::size_t idx = position; idx < m_fields_vector.size() - 1; idx ++ ) {
            m_fields_vector[idx] = m_fields_vector[idx+1];
        }
        m_fields_vector.erase(m_fields_vector.end());
    } else {
        return;
    }    
}
void HeaderFields::remove_at_key(FieldKeyArg k)
{
    boost::optional<std::size_t> index = this->find_at_index(k);
    if(index) {
        this->erase_at_index(index.get());
    }
}
#if 0
bool HeaderFields::hasKey(std::string k)
{
    return !(!(this->at_key(k)));
}
#endif
#if 1
bool HeaderFields::same_values(HeaderFields& other)
{
    if (this->size() != other.size()) return false;
    for (std::size_t index = 0; index < this->size(); index++ ) {
    
        Field this_element = m_fields_vector[index];
        auto found_optional = other.at_key(this_element.key);
        if(found_optional) {
            // found it and unwrapped the options
        } else {
            return false;
        }
    }
    return true;
}
bool HeaderFields::same_order_and_values(HeaderFields& other)
{
    if (this->size() != other.size()) return false;
    for (std::size_t index = 0; index < this->size(); index++ ) {
    
        Field other_element = other.at_index(index);
        Field this_element = m_fields_vector[index];

        if( (other_element.key != this_element.key) || (other_element.value != this_element.value)) {
            return false;
        }
    }
    return true;
}
#endif

} // namespace NewHeaders
