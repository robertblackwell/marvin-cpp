#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/server/tcp_server.hpp>

#include <thread>

#include <marvin/error_handler/error_handler.hpp>
#include <marvin/connection/socket_factory.hpp>


namespace Marvin {

int TcpServer::s_numberOfConnections = 35;
int TcpServer::s_heartbeat_interval_ms = 1000;

void TcpServer::configSet_NumberOfConnections(int n)
{
    s_numberOfConnections = n;
}
void TcpServer::configSet_HeartbeatInterval(int millisecs)
{
    s_heartbeat_interval_ms = millisecs;
}
bool TcpServer::verify()
{
    return true;
}

TcpServer::TcpServer(RequestHandlerUPtrFactory factory):
    m_heartbeat_interval_ms(s_heartbeat_interval_ms),
    m_numberOfConnections(s_numberOfConnections),
    m_io(1),
    m_signals(m_io),
    m_acceptor(m_io),
    m_connectionManager(m_io, this, m_numberOfConnections),
    m_factory(factory),
    m_heartbeat_timer(m_io),
    m_terminate_requested(false)
{
   TROG_TRACE_CTOR();
}


/**
** init
*/
 void TcpServer::p_initialize()
{
    // Register to handle the signals that indicate when the Server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
    #if defined(SIGQUIT)
    m_signals.add(SIGQUIT);
    #endif // defined(SIGQUIT)
    // s_instance = this;
    
    p_wait_for_stop();
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_port);
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    boost::system::error_code err;
    m_acceptor.bind(endpoint, err);
    if( err) {
        TROG_ERROR("error port: ",m_port, err.message());
        MARVIN_THROW(std::string("error binding server port ")+err.message());
    }
    p_start_heartbeat();

    TROG_DEBUG(err.message());
}
TcpServer::~TcpServer()
{
   TROG_TRACE_CTOR();
}
boost::asio::io_context& TcpServer::get_io_context()
{
     return m_io;
}
void TcpServer::terminate()
{
    m_terminate_requested = true;
    m_heartbeat_timer.cancel();
}
#pragma mark - listen, accept processing
//-------------------------------------------------------------------------------------
// listen - starts the accept cycle
//-------------------------------------------------------------------------------------
 void TcpServer::listen(long port, std::function<void()> ready_cb)
{
    m_port = port;
    p_initialize();
    m_acceptor.listen();
    
    auto hf = std::bind(&TcpServer::p_start_accept, this);
    m_io.post(hf);
    if (ready_cb != nullptr) {
        m_io.post(ready_cb);
    }
    m_io.run();
}
//-------------------------------------------------------------------------------------
// startAccept
//-------------------------------------------------------------------------------------
 void TcpServer::p_start_accept()
{

   TROG_TRACE3("");
    ISocketSPtr conn_sptr = socket_factory(m_io);
    
    ConnectionHandler* waiting_conn_handler_ptr = new ConnectionHandler(m_io, m_connectionManager, conn_sptr, m_factory);

    conn_sptr->async_accept(m_acceptor, [this, waiting_conn_handler_ptr](const boost::system::error_code &err)
    {
        p_handle_accept(waiting_conn_handler_ptr, err);
    });
}

//-------------------------------------------------------------------------------------
// handleAccept
//-------------------------------------------------------------------------------------
 void TcpServer::p_handle_accept(ConnectionHandler* waiting_conn_handler_ptr, const boost::system::error_code& err)
{
    TROG_INFO("", waiting_conn_handler_ptr);
    if (! m_acceptor.is_open()){
        delete waiting_conn_handler_ptr;
        TROG_WARN("Accept is not open ???? WTF - lets TERM the server");
        return; // something is wrong
    }
    if (!err){
       TROG_TRACE3("got a connection", waiting_conn_handler_ptr->native_socket_fd());
        /// at this point the native socket fd is assigned
        /// so for debug purposes we can stash it in the
        /// fd_inuse list

        m_connectionManager.register_connection_handler(waiting_conn_handler_ptr);
        // important sequence - if a call to p_start_accept() is defered
        // by allow_another_connection() we need to be sure there is still
        // an outstanding connection that will start an accept in the future.
        m_connectionManager.allow_another_connection([this]()
                                                     {
                                                         TROG_TRACE3("allowAnother Callback");
                                                         p_start_accept();
                                                     });
        waiting_conn_handler_ptr->serve();
    }else{
        TROG_WARN("Accept error value:",err.value()," cat:", err.category().name(), "message: ",err.message());
        delete waiting_conn_handler_ptr;
        m_io.stop();
        return;
    }
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
 void TcpServer::p_wait_for_stop()
{
    TROG_DEBUG("");
    auto hf = (std::bind(&TcpServer::p_do_stop, this, std::placeholders::_1));
    m_signals.async_wait(hf);
}
 void TcpServer::p_do_stop(const Marvin::ErrorType& err)
{
    TROG_DEBUG("");
    m_terminate_requested = true;
    m_heartbeat_timer.cancel();
    return;
    m_acceptor.close();
    m_io.stop();
}
void TcpServer::p_on_heartbeat(const boost::system::error_code& ec)
{
    if(m_terminate_requested) {
        m_connectionManager.stop_all();
        m_acceptor.cancel();
        return;
    }
    p_start_heartbeat();
}
void TcpServer::p_start_heartbeat()
{
    m_heartbeat_timer.expires_from_now(boost::posix_time::milliseconds(m_heartbeat_interval_ms));
    auto ds = (boost::bind(&TcpServer::p_on_heartbeat, this, boost::asio::placeholders::error));
    m_heartbeat_timer.async_wait(ds);
}
} // namespace Marvin