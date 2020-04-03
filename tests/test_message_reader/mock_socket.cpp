
#include <iostream>
#include <iterator>
#include <algorithm>
#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <openssl/x509.h>
#include <cert/cert_certificate.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include "testcase.hpp"
#include "mock_socket.hpp"

MockReadSocket::MockReadSocket(boost::asio::io_service& io, Testcase tcObj): m_io(io), m_tcObj(tcObj)
{
    m_index = 0;
    m_rdBuf = (char*)malloc(100000);
    m_single_timer = new SingleTimer(m_io, TIMER_INTERVAL_MS);
}
MockReadSocket::~MockReadSocket()
{
    delete m_single_timer;
    LogDebug(""); 
    free((void*)m_rdBuf);
}
long MockReadSocket::nativeSocketFD(){ return 9876; };

boost::asio::io_service& MockReadSocket::getIO() {return m_io; }
long MockReadSocket::getReadTimeout(){return 99;}
void MockReadSocket::cancel(){}
void MockReadSocket::startRead(){}

void MockReadSocket::asyncRead(Marvin::MBufferSPtr mb, long timeout_ms, AsyncReadCallback cb){}
//
// New buffer strategy make the upper level provide the buffer. All we do is check the size
//
void MockReadSocket::asyncRead(Marvin::MBufferSPtr mb, AsyncReadCallback cb)
{
    LogDebug("");
    char* buf;
    std::size_t len;
    if( ! m_tcObj.finished() ) {
        // we still have some test data so simulate an io wait and then call the read callback
        // these triggers should be the last buffer
        std::string s = m_tcObj.next();
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
                LogError("MockReadSocket:asyncRead error buffer too small");
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
            LogDebug("::new buffer: [", x, "] len:", len);
            m_index++;
            
            // at this point we have moved len bytes into the buffer mb
    //            st = new SingleTimer(m_io, 500);
            m_single_timer->start([this, buf, len, cb](const boost::system::error_code& ec)->bool{
                
                auto f = [this, buf, len, cb](Marvin::ErrorType& er, std::size_t bytes){
                    cb(er, bytes);
                };
                
                auto pf = std::bind(f, Marvin::make_error_ok(), (std::size_t)len);
                m_io.post(pf);
                return true;
            });
        }
    } else {
        LogDebug("test case finished");
        //  we have run out of test data so simulate an end of input - read of length zero
        ((char*)mb->data())[0] = (char)0;
        auto pf = std::bind(cb, Marvin::make_error_eom(), (std::size_t) 0);
        m_io.post(pf);
        return;
    }
}
void MockReadSocket::asyncWrite(std::string& str, AsyncWriteCallbackType cb){ assert(false);}
void MockReadSocket::asyncWrite(Marvin::MBuffer& fb, AsyncWriteCallback){ assert(false);}
void MockReadSocket::asyncWrite(Marvin::BufferChainSPtr chain_sptr, AsyncWriteCallback){ assert(false);}
void MockReadSocket::asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb){ assert(false);}
void MockReadSocket::asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback){ assert(false);}
void MockReadSocket::asyncConnect(ConnectCallbackType cb){ assert(false);}
void MockReadSocket::asyncAccept(
    boost::asio::ip::tcp::acceptor& acceptor,
    std::function<void(const boost::system::error_code& err)> cb
){ assert(false);}
void MockReadSocket::becomeSecureClient(X509_STORE* certificate_store_ptr){ assert(false);}
void MockReadSocket::becomeSecureServer(Cert::Identity server_identity){ assert(false);}
void MockReadSocket::asyncHandshake(std::function<void(const boost::system::error_code& err)> cb){ assert(false);}
Cert::Certificate MockReadSocket::getServerCertificate()
{ 
    Cert::Certificate tmp{};
    assert(false);
    return tmp;
}

void MockReadSocket::setReadTimeout(long interval){assert(false);}

void MockReadSocket::shutdown(ISocket::ShutdownType type){ assert(false);}
void MockReadSocket::close(){ assert(false);}

