#include <marvin/http/message_base.hpp>

#include <string>
#include <map>
#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include<http-parser/http_parser.h>
#include <marvin/error_handler/error_handler.hpp>

namespace Marvin{

std::string trace_message(MessageBase& msg)
{
    std::stringstream ss;
    ss << std::endl << "MSG[" << std::hex << &msg << std::dec << "]: ";
    
    if( msg.isRequest() ){
        ss << msg.method_string() << " " << msg.target() << " HTTP/1." << msg.version_minor();
    }else{
        ss << "HTTP/1." << msg.version_minor() << " " << msg.status_code() << " " << msg.reason()<< " ";
    }
    
    ss << (msg.headers().str());
    std::string s = ss.str();
    return ss.str();
}
#if 0
void serialize_headers(MessageBase& msg, Marvin::ContigBuffer& mb)
{
    boost::asio::streambuf b;
    std::ostream os(&b);

    std::string vers = "HTTP/" + std::to_string(msg.version_major()) + "." + std::to_string(msg.version_minor());
    if( msg.isRequest() ){
        std::string m = msg.method_string();
        std::string u = msg.target();
        os << m << " " << u << " " << vers << "\r\n";
    } else{
        os << vers << " " << msg.m_status_code << " " << msg.m_reason <<  "\r\n";
    }
    
    for(auto const& h : msg.m_headers) {
        os << h.key << ": " << h.value << "\r\n";
    }
    // end of headers
    os << "\r\n";
    // debugging only
    boost::asio::streambuf::const_buffers_type bufs = b.data();

    std::string ss(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + b.size() );
    mb.append((void*) ss.c_str(), ss.size());
//    return b;
//    std::string sss = ss;
//    std::cout << __FUNCTION__ << ":" << ss << std::endl;
    
}
Marvin::ContigBuffer::SPtr serialize_headers(MessageBase& msg)
{
    std::string h = "";
    serialize_headers(msg, h);
    return Marvin::makeContigBufferSPtr(h);
}
std::string serialize_headers(MessageBase& msg)
{
    std::string h{""};
    serialize_headers(msg, h);
    return h;
}
#endif
void serialize_headers(MessageBase& msg, std::string& str)
{
    str = "";
    std::ostringstream os;
    os.str(str);
    std::string vers = "HTTP/" + std::to_string(msg.version_major()) + "." + std::to_string(msg.version_minor());
    if( msg.isRequest() ) {
        std::string m = msg.method_string();
        std::string u = msg.target();
        os << m << " " << u << " " << vers << "\r\n";
    } else{
        os << vers << " " << msg.m_status_code << " " << msg.m_reason <<  "\r\n";
    }
    for(auto const& h : msg.m_headers) {
        os << h.key << ": " << h.value << "\r\n";
    }
    // end of headers
    os << "\r\n";
    str = os.str();
}

void serialize_header_lines(Marvin::HeaderFields& hdrs, ContigBuffer::SPtr mb)
{
    for(auto const& h : hdrs) {
        mb->append((void*)h.key.c_str(), h.key.size());
        mb->append((void*)(char*)": ", 2);
        mb->append((void*)h.value.c_str(), h.value.size());
        mb->append((void*)(char*)"\r\n", 2);
    }
}
void serialize_headers(MessageBase& msg, ContigBuffer::SPtr mb)
{
    static std::string const req_vers1_0 = " HTTP/1.0\r\n";
    static std::string const req_vers1_1 = " HTTP/1.1\r\n";
    static std::string const resp_vers1_0 = "HTTP/1.0 ";
    static std::string const resp_vers1_1 = "HTTP/1.1 ";
    static std::string const lfcr = "\n\r";
    int num_bytes = 0;
    if( msg.isRequest() ) {
        mb->append(msg.method_string());
        mb->append(" ");
        mb->append((void*)msg.target().c_str(), msg.target().size());
        if (msg.version_minor() == 0) {
            mb->append(req_vers1_0);
        } else {
            mb->append(req_vers1_1);
        }
    } else{
        if (msg.version_minor() == 0) {
            mb->append(resp_vers1_0);
        } else {
            mb->append(resp_vers1_1);
        }
        std::string remainder_of_line = std::to_string(msg.m_status_code) + " " + msg.m_reason + "\r\n";
        mb->append(remainder_of_line);
    }
    serialize_header_lines(msg.m_headers, mb);
    mb->append((void*)(char*)"\r\n", 2);
}
ContigBuffer::SPtr serialize_headers(MessageBase& msg)
{
    ContigBuffer::SPtr mb = makeContigBufferSPtr(256*4*8);
    serialize_headers(msg, mb);
    return mb;
}

bool is_connection_keep_alive(Marvin::MessageBase& msg)
{
    auto hopt = msg.header(HeaderFields::Connection);
    if (hopt) {
        if (is_connection_keep_alive(hopt.get())) {
            return true;
        } else if (is_connection_close(hopt.get())) {
            return false;
        }
    }
    if (msg.version_minor() == 1) {
        return true;
    } else {
        return false;
    }
}

#pragma - http message base impl

MessageBase::MessageBase()
{
    this->m_is_request = true;
    this->m_http_major = 1;
    this->m_http_minor = 1;
    this->m_body_chain_sptr = makeBufferChainSPtr();
}
MessageBase::MessageBase(MessageBase& other)
{
    *this = other;
    m_body_chain_sptr = std::make_shared<BufferChain>(*other.m_body_chain_sptr);
}
MessageBase::MessageBase(MessageBase&& other)
{
    *this = std::move(other);
}
MessageBase& MessageBase::operator =(MessageBase& other)
{
    if (this == &other) {
        return *this;
    }
    m_is_request    = other.m_is_request;
    m_method        = other.m_method;
    m_method_str     = other.m_method_str;
    m_target           = other.m_target;
    m_status_code   = other.m_status_code;
    m_reason        = other.m_reason;
    m_http_major    = other.m_http_major;
    m_http_minor    = other.m_http_minor;
    m_headers       = other.m_headers;
    m_trailers      = other.m_trailers; 
    m_body_chain_sptr = std::make_shared<BufferChain>(*other.m_body_chain_sptr);
    return *this;
}
MessageBase& MessageBase::operator =(MessageBase&& other)
{
    if (this == &other) {
        return *this;
}
    m_is_request    = other.m_is_request;
    m_method        = other.m_method;
    m_method_str     = other.m_method_str;
    m_target        = other.m_target; other.m_target = "";
    m_status_code   = other.m_status_code;
    m_reason        = other.m_reason;
    m_http_major    = other.m_http_major;
    m_http_minor    = other.m_http_minor;
    m_headers       = std::move(other.m_headers);
    m_trailers      = std::move(other.m_trailers);
    m_body_chain_sptr = other.m_body_chain_sptr;
    other.m_body_chain_sptr = makeBufferChainSPtr();
    return *this;
}

MessageBase::~MessageBase(){}

bool MessageBase::is_request() {return m_is_request;}

void MessageBase::status_code(int sc){ m_status_code = sc; m_is_request = false; }
int MessageBase::status_code() {return m_status_code; };

void MessageBase::reason(std::string st){ m_reason = st; m_is_request = false; }
std::string MessageBase::reason(){return m_reason;}

void MessageBase::method(HttpMethod m){ m_method = (enum http_method)m;  m_is_request = true; }
void MessageBase::method(enum http_method m){ m_method = m; m_is_request = true; }
HttpMethod MessageBase::method()
{
    HttpMethod tmp = (HttpMethod) m_method;
    return tmp;
}
std::string MessageBase::method_string() {return http_method_string(m_method);}

std::string MessageBase::target(){return m_target;}
void MessageBase::target(std::string target){m_target = target;}

void MessageBase::version(int major, int minor){m_http_major = major; m_http_minor = minor;};
void MessageBase::version_major(int major){m_http_major = major;}
void MessageBase::version_minor(int minor){m_http_minor = minor;}
int  MessageBase::version_major(){return m_http_major;}
int  MessageBase::version_minor(){return m_http_minor;}

HeaderFields& MessageBase::headers(){return m_headers;}
void MessageBase::header(std::string key, std::string value)
{
    std::string v(value);
    std::string v2 = boost::algorithm::trim_copy(v);
    m_headers.set_at_key(key, v2);
}
void MessageBase::header(std::string* k, std::string* v)
{
    m_headers.set_at_key(k, v);
}
boost::optional<std::string>
MessageBase::header(std::string key) 
{
    boost::optional<std::string> res{m_headers.at_key(key)};
    return res;
}

HeaderFields& MessageBase::trailers(){return m_trailers;}
void MessageBase::trailer(std::string key, std::string value)
{
    std::string v(value);
    std::string v2 = boost::algorithm::trim_copy(v);
    m_headers.set_at_key(key, v2);
};
boost::optional<std::string>
MessageBase::trailer(std::string key) 
{
    boost::optional<std::string> res{m_headers.at_key(key)};
    return res;
}

bool MessageBase::isRequest(){ return m_is_request; }

void MessageBase::set_is_request(bool flag){ m_is_request = flag;}

void MessageBase::remove_header( std::string keyIn)
{
    m_headers.remove_at_key(keyIn);
}

boost::optional<std::size_t>
MessageBase::content_length()
{
    auto s = header(HeaderFields::ContentLength);
    if(s) {
        int v = std::atoi(s.get().c_str());
        return (std::size_t)v;
    } else {
        return boost::none;
    }
}
void MessageBase::serialize_headers(ContigBuffer::SPtr mb)
{
    // force use of friend
    Marvin::serialize_headers(*this, mb);
}
ContigBuffer::SPtr MessageBase::serialize_headers()
{
    // force use of friend
    return Marvin::serialize_headers(*this);
}

 std::string MessageBase::to_string()
 {
     return Marvin::serialize_headers(*this)->toString();
 }
Marvin::BufferChain::SPtr MessageBase::get_body_buffer_chain()
{
    return m_body_chain_sptr;
}
void MessageBase::set_body_buffer_chain(Marvin::BufferChain::SPtr bufSPtr)
{
    m_body_chain_sptr = bufSPtr;
}
Marvin::BufferChain::SPtr MessageBase::get_body()
{
    return m_body_chain_sptr;
}
void MessageBase::set_body(Marvin::BufferChain::SPtr bufSPtr)
{
    m_body_chain_sptr = bufSPtr;
    remove_header(Marvin::HeaderFields::TransferEncoding);
    header(Marvin::HeaderFields::ContentLength, std::to_string(bufSPtr->size()));
}
void MessageBase::set_body(std::string content)
{
    m_body_chain_sptr = Marvin::makeBufferChainSPtr(content);
    remove_header(Marvin::HeaderFields::TransferEncoding);
    header(Marvin::HeaderFields::ContentLength, std::to_string(m_body_chain_sptr->size()));
}

void MessageBase::dump_headers(std::ostream& os)
{
    HeaderFields::Iterator it = m_headers.begin();
    while(it != m_headers.end()) {
        os<<it->key <<" : "<<it->value<<std::endl;
        it++;
    }
}
    
std::ostream &operator<< (std::ostream &os, MessageBase &msg)
{
    ContigBuffer::SPtr mb_h = msg.serialize_headers();
    os << mb_h->toString() ;
    return os;
}

} //namespace Marvin
