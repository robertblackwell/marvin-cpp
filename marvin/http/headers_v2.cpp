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
const std::string HeadersV2::AcceptEncoding = "ACCEPT-ENCODING";
const std::string HeadersV2::Authorization = "AUTHORIZATION";
const std::string HeadersV2::Connection = "CONNECTION";
const std::string HeadersV2::ConnectionHandlerId = "CONNECT-HANDLER-ID";
const std::string HeadersV2::ContentLength = "CONTENT-LENGTH";
const std::string HeadersV2::ContentType = "CONTENT-TYPE";
const std::string HeadersV2::Date = "DATE";
const std::string HeadersV2::Host = "HOST";
const std::string HeadersV2::ProxyAuthorization = "PROXY-AUTHORIZATION";
const std::string HeadersV2::ProxyAuthentication = "PROXY-AUTHENTICATION";
const std::string HeadersV2::ProxyConnection = "PROXY-CONNECTION";
const std::string HeadersV2::TE = "TE";
const std::string HeadersV2::TransferEncoding = "TRANSFER-ENCODING";
const std::string HeadersV2::ETag = "ETAG";
const std::string HeadersV2::Upgrade = "UPGRADE";
const std::string HeadersV2::Trailer = "TRAILER";
const std::string HeadersV2::RequestHandlerId = "REQUEST-HANDLER-ID";

const std::string HeadersV2::ConnectionClose = "CLOSE";
const std::string HeadersV2::ConnectionKeepAlive = "KEEP-ALIVE";

bool isConnectionKeepAlive(std::string value)
{
    std::regex r( R"((\s*)(,|;|\s|^)(\s*)keep-alive(\s*)(,|;|\s|$))", std::regex::icase);
    auto x = std::regex_search(value, r);
    return x;
}
// test whether a header string value has a 'close' substring
bool isConnectionClose(std::string value)
{
    std::regex r(R"((\s*)(,|;|\s|^)(\s*)close(\s*)(,|;|\s|$))", std::regex::icase);
    auto x = std::regex_search(value, r);
    return x;
}
// tests whether a header structure has a connection header with a 'keep-alive' substring
bool isConnectionKeepAlive(Marvin::HeadersV2& h)
{
    boost::optional<std::string> opt = h.atKey(HeadersV2::Connection);
    if(opt) {
        std::string& s = opt.get();
        return isConnectionKeepAlive(s);
    }
    return false;
}
// tests whether a header structure has a connection header with a 'close' substring
bool isConnectionClose(Marvin::HeadersV2& h)
{
    boost::optional<std::string> opt = h.atKey(HeadersV2::Connection);
    if(opt) {
        std::string& s = opt.get();
        return isConnectionClose(s);
    }
    return false;
}


std::ostream &operator<< (std::ostream &os, HeadersV2& headers)
{
    int num = headers.size();
    for(int i = 0; i < num; i++) {
        auto p = headers.atIndex(i);
        os << " " << p.key << ": " << p.value;
    }
}

std::string HeadersV2::str()
{
    std::ostringstream os;
    os << (*this);
    return os.str();
}
void HeadersV2::copyExcept(HeadersV2& source, HeadersV2& dest, std::set<std::string> filterList)
{
    for(auto it = source.begin(); it != source.end(); it++) {
        std::string k = it->key;
        if( filterList.end() == filterList.find(k) ) {
            /// if not in list copy
            dest.setAtKey(it->key, it->value);
        }
    }
}

HeadersV2::Iterator HeadersV2::Iterator::operator++()
{
    Iterator i = *this;
    m_index++;
    return i;
}
HeadersV2::Iterator const HeadersV2::Iterator::operator++(int junk)
{
    m_index++; return *this;
}
HeadersV2::Field& HeadersV2::Iterator::operator*()
{
    return m_headers.m_fields_vector[m_index];
}
HeadersV2::Field* HeadersV2::Iterator::operator->()
{
    return &(m_headers.m_fields_vector[m_index]);
}
bool HeadersV2::Iterator::operator==(const Iterator& rhs)
{
    return m_index == rhs.m_index;
}
bool HeadersV2::Iterator::operator!=(const Iterator& rhs)
{
    return m_index != rhs.m_index;
}

HeadersV2::Iterator & HeadersV2::Iterator::operator=(const HeadersV2::Iterator &rhs)
{
    return *this;
}

HeadersV2::HeadersV2()
{
}
HeadersV2::HeadersV2(HeadersV2& other): m_fields_vector(other.m_fields_vector)
{}
HeadersV2::HeadersV2(HeadersV2&& other) noexcept: m_fields_vector(std::move(other.m_fields_vector))
{}
HeadersV2& HeadersV2::operator =(HeadersV2 const& other)
{
    m_fields_vector = other.m_fields_vector;
}
HeadersV2& HeadersV2::operator =(HeadersV2&& other)
{
    m_fields_vector = other.m_fields_vector;
    other.m_fields_vector.clear();
}
HeadersV2::HeadersV2(std::vector<std::pair<std::string, std::string>> initialValue)
{
    typedef std::vector<std::pair<std::string, std::string>> okv_init;
    for(int i = 0; i < initialValue.size(); i++) {
        auto x = initialValue[i];
        std::string tmp_first = toupper_copy(x.first);
        setAtKey(tmp_first, x.second);
    }
}

std::size_t HeadersV2::size() const { return (std::size_t)m_fields_vector.size(); }

HeadersV2::Iterator HeadersV2::begin()
{
    return HeadersV2::Iterator(*this, 0);
}

HeadersV2::Iterator HeadersV2::end()
{
    return HeadersV2::Iterator(*this, (int)m_fields_vector.size());
}

HeadersV2::Iterator HeadersV2::find(HeadersV2::FieldKeyArg key)
{
    HeadersV2::Iterator it{*this, 0};
    std::string s{key};
    std::string key_upper = toupper_copy(s);
    for( it = this->begin(); it != this->end(); it++ ) {
        if ((*it).key == key_upper) {
            return it;
        }
    }
    return this->end();
}

boost::optional<std::size_t> HeadersV2::findAtIndex(HeadersV2::FieldKeyArg key)
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

HeadersV2::Field HeadersV2::atIndex(std::size_t index)
{
    if (index >= m_fields_vector.size()) {
        throw HeadersV2::Exception("index is out of bounds accessing headers");
    } else {
        return m_fields_vector[index];
    }
}
boost::optional<std::string> HeadersV2::atKey(HeadersV2::FieldKeyArg k)
{
    boost::optional<std::size_t> index = this->findAtIndex(k);
    if(index) {
        return m_fields_vector[index.get()].value;
    }
    return boost::optional<std::string>();
}
void HeadersV2::setAtKey(HeadersV2::FieldKeyArg k, std::string v)
{
    std::string s{k};
    std::string key_upper = toupper_copy(s);
    boost::optional<std::size_t> index = this->findAtIndex(k);
    if(index) {
        m_fields_vector[index.get()].value = v;
    } else {
        Field Field{.key=key_upper, .value=v};
        m_fields_vector.push_back(Field);
    }
}
void HeadersV2::eraseAtIndex(std::size_t position)
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
void HeadersV2::removeAtKey(HeadersV2::FieldKeyArg k)
{
    boost::optional<std::size_t> index = this->findAtIndex(k);
    if(index) {
        this->eraseAtIndex(index.get());
    }
}
#if 0
bool HeadersV2::hasKey(std::string k)
{
    return !(!(this->atKey(k)));
}
#endif
#if 1
bool HeadersV2::sameValues(HeadersV2& other)
{
    if (this->size() != other.size()) return false;
    for (std::size_t index = 0; index < this->size(); index++ ) {
    
        Field this_element = m_fields_vector[index];
        auto found_optional = other.atKey(this_element.key);
        if(found_optional) {
            // found it and unwrapped the options
        } else {
            return false;
        }
    }
    return true;
}
bool HeadersV2::sameOrderAndValues(HeadersV2& other)
{
    if (this->size() != other.size()) return false;
    for (std::size_t index = 0; index < this->size(); index++ ) {
    
        Field other_element = other.atIndex(index);
        Field this_element = m_fields_vector[index];

        if( (other_element.key != this_element.key) || (other_element.value != this_element.value)) {
            return false;
        }
    }
    return true;
}
#endif

} // namespace NewHeaders
