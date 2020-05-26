#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include "mock_socket.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <marvin/boost_stuff.hpp>
#include <marvin/configure_trog.hpp>
#include <openssl/x509.h>
#include <cert/cert_certificate.hpp>
#include "testcase.hpp"

namespace Marvin {
namespace Tests {
namespace Message {

MockReadSocket::MockReadSocket(boost::asio::io_service& io, DataSource& data_source)
    : m_io(io), 
    m_ssl_context(boost::asio::ssl::context::method::sslv23),
    m_data_source(data_source)
{
    m_rdBuf = (char*)malloc(100000);
    m_single_timer = new SingleTimer(m_io, TIMER_INTERVAL_MS);
}
// MockReadSocket::~MockReadSocket()
// {
//     delete m_single_timer;
//     TROG_DEBUG(""); 
//     free((void*)m_rdBuf);
// }
long MockReadSocket::native_socket_fd(){ return 9876; };

boost::asio::io_service& MockReadSocket::get_io_context() {return m_io; }
boost::asio::ssl::context& MockReadSocket::get_ssl_context(){return m_ssl_context;}

long MockReadSocket::get_read_timeout(){return 99;}
void MockReadSocket::cancel(){}
void MockReadSocket::startRead(){}

void MockReadSocket::async_read(Marvin::ContigBuffer::SPtr mb, long timeout_ms, ISocket::ReadHandler cb)
{throw("read with timeout into ContigBuffer not implemented");}
void MockReadSocket::async_read(void* buffer, std::size_t buffer_length, long timeout_ms, ISocket::ReadHandler cb)
{throw("read with timeout into void* buffer not implemented");}
//
// New buffer strategy make the upper level provide the buffer. All we do is check the size
//
void MockReadSocket::async_read(boost::asio::streambuf& streambuffer, ISocket::ReadHandler cb)
{
    boost::asio::mutable_buffer mutablebuffer = streambuffer.prepare(1000);
    async_read(mutablebuffer, [&streambuffer, cb](Marvin::ErrorType err, std::size_t bytes_read)
    {
        streambuffer.commit(bytes_read);
        cb(err, bytes_read);
    });
}
void MockReadSocket::async_read(boost::asio::mutable_buffer mutablebuffer, ISocket::ReadHandler cb)
{
    void* bptr = mutablebuffer.data();
    std::size_t len = mutablebuffer.size();
    async_read(bptr, len, [cb](Marvin::ErrorType err, std::size_t bytes_read)
    {
        cb(err, bytes_read);
    });
}
void MockReadSocket::async_read(void* buffer, std::size_t buffer_length, ISocket::ReadHandler cb)
{   
    TROG_DEBUG("");
    char* buf;
    std::size_t len;
    if(m_data_source.finished() ) {
        TROG_DEBUG("test case finished");
        //  we have run out of test data so simulate an end of input - read of length zero
        *((char*)buffer) = (char)0;
        auto pf = std::bind(cb, Marvin::make_error_eom(), (std::size_t) 0);
        m_io.post(pf);
        return;
    } else {
        // we still have some test data so simulate an io wait and then call the read callback
        // these triggers should be the last buffer
        std::string s = m_data_source.next();
        if( (s == "eof")||(s=="close")||(s=="shutdown")) {
            // start a timer and at expiry call cb with error code
            // sending code of network reset
            m_single_timer->start([this, s, cb](const boost::system::error_code& ec)->bool
            {
                m_io.post([this, s, cb]()
                {
                    boost::system::error_code net_reset = boost::system::errc::make_error_code(boost::system::errc::network_reset);
                    cb(net_reset, 0);
                });
                return true;
            });
        } else {
            // s is a string containing the next buffer of data to be processed
            buf = (char*)s.c_str();
            len = strlen(buf);
            std::size_t buf_max = buffer_length;
            if( buf_max < len + 1){
                TROG_ERROR("MockReadSocket:asyncRead error  - read buffer too small");
                throw "MockReadSocket:asyncRead error - read buffer too small";
            }
            // copy the new data iinto the output buffer
            void* rawPtr = buffer;
            memcpy(buffer, buf, len);
            // terminate the buffer with null
            ((char*)buffer)[len] = (char)0;

            std::string bb = std::string((char*)rawPtr, len);
            
            std::size_t startIndex = bb.find("\r\n");
            std::string x;
            // found a match ?
            if( startIndex != bb.npos ){
                x = bb.replace(startIndex, std::string("\r\n").length(), "\\r\\n");
            }else{
                x = bb;
            }
            TROG_DEBUG("::new buffer: [", x, "] len:", len);

            // at this point we have moved len bytes into the buffer mb
            // st = new SingleTimer(m_io, 500);
            m_single_timer->start([this, buf, len, cb](const boost::system::error_code& ec)->bool{
                m_io.post([this, len, cb]()
                {
                    auto err = Marvin::make_error_ok();
                    cb(err, len);
                });
                return true;
            });
        }
    } 
}
void MockReadSocket::async_read(Marvin::ContigBuffer::SPtr mb, ISocket::ReadHandler cb)
{
    TROG_DEBUG("");
    char* buf;
    std::size_t len;
    if( m_data_source.finished()) {
        TROG_DEBUG("test case finished");
        //  we have run out of test data so simulate an end of input - read of length zero
        ((char*)mb->data())[0] = (char)0;
        auto pf = std::bind(cb, Marvin::make_error_eom(), (std::size_t) 0);
        m_io.post(pf);
        return;
    } else {
        // we still have some test data so simulate an io wait and then call the read callback
        // these triggers should be the last buffer
        std::string s = m_data_source.next();
        if( (s == "eof")||(s=="close")||(s=="shutdown")) {
            // start a timer and at expiry call cb with error code for eof
            m_single_timer->start([this, s, cb](const boost::system::error_code& ec)->bool{
                
                auto f = [this, s, cb](Marvin::ErrorType& er, std::size_t bytes){
                    cb(er, bytes);
                };
                
                auto pf = std::bind(f, Marvin::make_error_eof(), (std::size_t)0);
                m_io.post(pf);
                return true;
            });

        } else {
            buf = (char*)s.c_str();
            len = strlen(buf);
            std::size_t buf_max = mb->capacity();
            if( buf_max < len + 1){
                TROG_ERROR("MockReadSocket:asyncRead error buffer too small");
                throw "MockReadSocket:asyncRead error buffer too small";
            }
            void* rawPtr = mb->data();
            memcpy(rawPtr, buf, len);
            ((char*)rawPtr)[len] = (char)0;
            
    //        char* b = (char*)rawPtr;
            std::string bb = std::string((char*)rawPtr, len);
            
            std::size_t startIndex = bb.find("\r\n");
            std::string x;
            if( startIndex != bb.npos ){
                x = bb.replace(startIndex, std::string("\r\n").length(), "\\r\\n");
            }else{
                x = bb;
            }
            TROG_DEBUG("::new buffer: [", x, "] len:", len);

            // at this point we have moved len bytes into the buffer mb
            // st = new SingleTimer(m_io, 500);
            m_single_timer->start([this, buf, len, cb](const boost::system::error_code& ec)->bool{
                
                auto f = [this, buf, len, cb](Marvin::ErrorType& er, std::size_t bytes){
                    cb(er, bytes);
                };
                
                auto pf = std::bind(f, Marvin::make_error_ok(), (std::size_t)len);
                m_io.post(pf);
                return true;
            });
        }

    }
}
void MockReadSocket::async_write(std::string& str, Marvin::ISocket::WriteHandler cb){ assert(false);}
void MockReadSocket::async_write(Marvin::ContigBuffer& fb, Marvin::AsyncWriteCallback){ assert(false);}
void MockReadSocket::async_write(Marvin::BufferChain::SPtr chain_sptr, Marvin::AsyncWriteCallback){ assert(false);}
void MockReadSocket::async_write(boost::asio::const_buffer buf, Marvin::AsyncWriteCallback cb){ assert(false);}
void MockReadSocket::async_write(boost::asio::streambuf& sb, Marvin::AsyncWriteCallback){ assert(false);}
void MockReadSocket::async_write(void* buffer, std::size_t buffer_length, Marvin::AsyncWriteCallback){ assert(false);}

void MockReadSocket::async_connect(ISocket::ConnectHandler cb){ assert(false);}
void MockReadSocket::async_accept(
    boost::asio::ip::tcp::acceptor& acceptor,
    std::function<void(const boost::system::error_code& err)> cb
){ assert(false);}
void MockReadSocket::become_secure_client(X509_STORE* certificate_store_ptr){ assert(false);}
void MockReadSocket::become_secure_server(Cert::Identity server_identity){ assert(false);}
void MockReadSocket::async_handshake(std::function<void(const boost::system::error_code& err)> cb){ assert(false);}
Cert::Certificate MockReadSocket::get_server_certificate()
{ 
    Cert::Certificate tmp{};
    assert(false);
    return tmp;
}

void MockReadSocket::set_read_timeout(long interval){assert(false);}

void MockReadSocket::shutdown(ISocket::ShutdownType type){ assert(false);}
void MockReadSocket::close(){ assert(false);}
} // namespace Message
} // namespace Tests
} // namespace Marvin

