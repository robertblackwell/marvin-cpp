//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <thread>
#include <regex>
#include <memory>
#include <boost/process.hpp>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/collector/pipe_collector.hpp>
#include <marvin/collector/collector_base.hpp>
#include <marvin/server_v3/mitm_app.hpp>
#include <marvin/http/message_factory.hpp>
namespace Marvin {

using namespace ::boost::filesystem;

typedef boost::asio::ip::tcp::socket socket_type;
typedef boost::asio::ssl::stream<socket_type> stream_type;

class SslApp : public RequestHandlerInterface 
{
    public:
    SslApp(boost::asio::io_service& io ): m_io(io)
    {

    }
    void handle(
        ServerContext&            server_context,
        ISocketSPtr               clientConnectionSPtr,
        HandlerDoneCallbackType   done
    ){
        m_downstream_socket_sptr = clientConnectionSPtr;
        m_done_callback          = done;
        m_rdr_sptr = std::make_shared<MessageReader>(m_downstream_socket_sptr); 
        m_wrtr_sptr = std::make_shared<MessageWriter>(m_downstream_socket_sptr); 
        become_secure();

        m_downstream_socket_sptr->asyncHandshake([this](const boost::system::error_code& err)
        {
            if(err) {
                auto x = make_error_description(err);
                std::cout << "handshake failed" << x << std::endl;
            } else {

                m_rdr_sptr->readMessage([this](ErrorType err)
                {
                    if(err) {
                        auto x = make_error_description(err);
                        std::cout << "read failed" << x << std::endl;
                    } else {
                        m_response_msg_sptr = std::make_shared<MessageBase>();
                        makeResponse200OKConnected(*m_response_msg_sptr);
                        m_response_msg_sptr->header(HeadersV2::Connection, "Close");
                        std::string body{"This is a legitimate response"};
                        m_response_msg_sptr->setContent(body);
                        BufferChain::SPtr buffer_sptr = makeBufferChainSPtr(body);
                        m_wrtr_sptr->async_write(m_response_msg_sptr, body, [this](ErrorType& err)
                        {
                            if(err) {
                                auto x = make_error_description(err);
                                // m_downstream_socket_sptr->close();
                                m_done_callback();
                            } else {
                                m_done_callback();
                            }
                        });
                    }
                });
            }
        });
    }
    void become_secure()
    {
        // ISocketSPtr sockptr = m_downstream_socket_sptr;
        // m_conn_sptr = std::dynamic_pointer_cast<Connection>(sockptr);
        // ssl::context& my_ssl_context = sockptr->getSslContext();
        // SSL_CTX* ssl_ctx_ptr = my_ssl_context.native_handle();
        path cert_path{"/home/robert/Projects/marvin++/tests/test_ssl_server/server.pem"};
        path key_path{"/home/robert/Projects/marvin++/tests/test_ssl_server/server.pem"};
        Cert::Certificate server_cert{cert_path};
        EVP_PKEY* pkey_ptr = Cert::x509::PKey_ReadPrivateKeyFrom(key_path.string(), "test");
        Cert::Identity identity{server_cert.native(), pkey_ptr};
        m_downstream_socket_sptr->becomeSecureServer(identity);
        // SSL_CTX_use_PrivateKey(ssl_ctx_ptr, pkey_ptr);
        // SSL_CTX_use_certificate(ssl_ctx_ptr, server_cert.native());
        // m_conn_sptr->m_mode = Connection::Mode::SECURE_SERVER;
        // m_ssl_stream_ptr = std::make_shared<stream_type>(m_io, ssl_ctx_ptr);
    }
    void make_server_certificate()
    {

    }
    boost::asio::io_service&    m_io;
    ISocketSPtr                 m_downstream_socket_sptr;
    ConnectionSPtr              m_conn_sptr;
    HandlerDoneCallbackType     m_done_callback;
    MessageReaderSPtr           m_rdr_sptr;
    MessageWriterSPtr           m_wrtr_sptr;
    MessageBaseSPtr             m_response_msg_sptr;
    std::shared_ptr<stream_type>   m_ssl_stream_ptr;
    // boost::asio::ssl::context&  m_ssl_context;
};
typedef std::unique_ptr<SslApp> SslAppUPtr;
} // namespace

int main( int argc, char* argv[] )
{
    using namespace Marvin;

    std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
    std::vector<int> ports{443, 9443};
    MitmApp::configSet_HttpsPorts(ports);
    MitmApp::configSet_HttpsHosts(re);

    TcpServer* server_ptr;

    std::function<void(void*)> proxy_thread_func = [&server_ptr](void* param) {
        server_ptr = new Marvin::TcpServer([](boost::asio::io_service& io) {
            SslAppUPtr app_uptr = std::make_unique<SslApp>(io);
            return app_uptr;
        });
        server_ptr->listen(9992);
    };
    std::thread proxy_thread(proxy_thread_func, nullptr);

    proxy_thread.join();
}
