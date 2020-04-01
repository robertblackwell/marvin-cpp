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

namespace Marvin::Http {

const std::string HeadersV2::AcceptEncoding = "ACCEPT-ENCODING";
const std::string HeadersV2::Authorization = "AUTHORIZATION";
const std::string HeadersV2::Connection = "CONNECTION";
const std::string HeadersV2::ConnectionHandlerId = "CONNECT-HANDLER-ID";
const std::string HeadersV2::ContentLength = "CONTENT-LENGTH";
const std::string HeadersV2::ContentType = "CONTENT-TYPE";
const std::string HeadersV2::Date = "DATE";
const std::string HeadersV2::Host = "HOST";
const std::string HeadersV2::ProxyConnection = "PROXY-CONNECTION";
const std::string HeadersV2::TE = "TE";
const std::string HeadersV2::TransferEncoding = "TRANSFER-ENCODING";
const std::string HeadersV2::ETag = "ETAG";
const std::string HeadersV2::RequestHandlerId = "REQUEST-HANDLER-ID";

const std::string HeadersV2::ConnectionClose = "CLOSE";
const std::string HeadersV2::ConnectionKeepAlive = "KEEP-ALIVE";


HeadersV2::Iterator HeadersV2::Iterator::operator++()
{
    Iterator i = *this;
    m_index++;
    return i;
}
HeadersV2::Iterator HeadersV2::Iterator::operator++(int junk)
{
    m_index++; return *this;
}
HeadersV2::Pair& HeadersV2::Iterator::operator*()
{
    return m_headers.m_pairs_vector[m_index];
}
HeadersV2::Pair* HeadersV2::Iterator::operator->()
{
    return &(m_headers.m_pairs_vector[m_index]);
}
bool HeadersV2::Iterator::operator==(const Iterator& rhs)
{
    return m_index == rhs.m_index;
}
bool HeadersV2::Iterator::operator!=(const Iterator& rhs)
{
    return m_index != rhs.m_index;
}

HeadersV2::Iterator HeadersV2::Iterator::operator=(const HeadersV2::Iterator &rhs)
{
    return *this;
}

HeadersV2::HeadersV2()
{
}
HeadersV2::HeadersV2(std::vector<std::pair<std::string, std::string>> initialValue)
{
    typedef std::vector<std::pair<std::string, std::string>> okv_init;
    for(int i = 0; i < initialValue.size(); i++) {
        auto x = initialValue[i];
        setAtKey(toupper_copy(x.first), x.second);
    }
}

std::size_t HeadersV2::size() const { return (std::size_t)m_pairs_vector.size(); }

HeadersV2::Iterator HeadersV2::begin()
{
    return HeadersV2::Iterator(*this, 0);
}

HeadersV2::Iterator HeadersV2::end()
{
    return HeadersV2::Iterator(*this, (int)m_pairs_vector.size());
}

HeadersV2::Iterator HeadersV2::find(std::string key)
{
    HeadersV2::Iterator it{*this, 0};
    std::string key_upper = toupper_copy(key);
    for( it = this->begin(); it != this->end(); it++ ) {
        if ((*it).first == key_upper) {
            return it;
        }
    }
    return this->end();
}

boost::optional<std::size_t> HeadersV2::findAtIndex(std::string key)
{
    std::string key_upper = toupper_copy(key);
    for(std::size_t index = 0; index < m_pairs_vector.size(); index++) {
        if (m_pairs_vector[index].first == key_upper) {
            return boost::optional<std::size_t>(index);
        }
    }
    return boost::optional<std::size_t>();
}

std::pair<std::string, std::string> HeadersV2::atIndex(std::size_t index)
{
    if (index >= m_pairs_vector.size()) {
        throw HeadersV2::Exception("index is out of bounds accessing headers");
    } else {
        return m_pairs_vector[index];
    }
}
boost::optional<std::string> HeadersV2::atKey(std::string k)
{
    boost::optional<std::size_t> index = this->findAtIndex(k);
    if(index) {
        return m_pairs_vector[index.get()].second;
    }
    return boost::optional<std::string>();
}
void HeadersV2::setAtKey(std::string k, std::string v)
{
    std::string key_upper = toupper_copy(k);
    boost::optional<std::size_t> index = this->findAtIndex(k);
    if(index) {
        m_pairs_vector[index.get()].second = v;
    } else {
        std::pair<std::string, std::string> pair(key_upper, v);
        m_pairs_vector.push_back(pair);
    }
}
void HeadersV2::eraseAtIndex(std::size_t position)
{
    if((position >= 0)&&(position < m_pairs_vector.size())) {
        for(std::size_t idx = position; idx < m_pairs_vector.size() - 1; idx ++ ) {
            m_pairs_vector[idx] = m_pairs_vector[idx+1];
        }
        m_pairs_vector.erase(m_pairs_vector.end());
    } else {
        return;
    }    
}
void HeadersV2::removeAtKey(std::string k)
{
    boost::optional<std::size_t> index = this->findAtIndex(k);
    if(index) {
        this->eraseAtIndex(index.get());
    }
}
bool HeadersV2::hasKey(std::string k)
{
    return !(!(this->atKey(k)));
}

} // namespace NewHeaders