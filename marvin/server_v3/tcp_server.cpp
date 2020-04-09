#include <marvin/server_v3/tcp_server.hpp>

#include <thread>

#include <marvin/helpers/macros.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/external_src/trog/trog.hpp>
Trog_SETLEVEL(LOG_LEVEL_WARN|LOG_LEVEL_TRACE|LOG_LEVEL_TORTRACE)

namespace Marvin {

int TcpServer::s_numberOfThreads = 4;
int TcpServer::s_numberOfConnections = 35;
int TcpServer::s_heartbeat_interval_ms = 1000;

TcpServer* TcpServer::s_instance = nullptr;

TcpServer* TcpServer::get_instance()
{
    return s_instance;
}
void TcpServer::configSet_NumberOfThreads(int n)
{
    s_numberOfThreads = n;
}
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
    m_numberOfThreads(s_numberOfThreads),
    m_numberOfConnections(s_numberOfConnections),
    m_io(1),
    m_signals(m_io),
    m_acceptor(m_io),
    m_connectionManager(m_io, m_numberOfConnections),
    m_factory(factory),
    m_heartbeat_timer(m_io),
    m_terminate_requested(false)
{
    LogTorTrace();
}
/**
** init
*/
 void TcpServer::p_initialize()
{
    ///
    /// !! make sure this is big enough to handle the components with dedicated strands
    ///
    m_numberOfThreads = s_numberOfThreads;
    // Register to handle the signals that indicate when the Server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
    #if defined(SIGQUIT)
    m_signals.add(SIGQUIT);
    #endif // defined(SIGQUIT)
    s_instance = this;
#if 0
    auto handler = [this]( const boost::system::error_code& error, int signal_number)
    {
//      std::cout << "Handler Got signal " << signal_number << "; "
//                   "stopping io_service." << std::endl;
      this->_io.stop();
    };
    _signals.async_wait(handler);
#endif
    
    p_wait_for_stop();
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_port);
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    boost::system::error_code err;
    m_acceptor.bind(endpoint, err);
    if( err) {
        LogError("error port: ",m_port, err.message());
        MTHROW(std::string("error binding server port ")+err.message());
    }
    p_start_heartbeat();

    LogDebug(err.message());
}
TcpServer::~TcpServer()
{
    LogTorTrace();
}
void TcpServer::terminate()
{
    m_terminate_requested = true;
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
    
    // start the accept process on the _serverStrand
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

    LogTrace("");
    ISocketSPtr conn_sptr = socketFactory(m_io);
    
    ConnectionHandler* connectionHandler = new ConnectionHandler(m_io, m_connectionManager, conn_sptr, m_factory);

    auto hf = (std::bind(&TcpServer::p_handle_accept, this, connectionHandler, std::placeholders::_1));
    conn_sptr->asyncAccept(m_acceptor, hf);
//    _acceptor.async_accept(_boost_socket, hf);

}

//-------------------------------------------------------------------------------------
// handleAccept - called on _strand to handle a new client connection
//-------------------------------------------------------------------------------------
 void TcpServer::p_handle_accept(ConnectionHandler* connHandler, const boost::system::error_code& err)
{
    LogInfo("", connHandler);
    if (! m_acceptor.is_open()){
        delete connHandler;
        LogWarn("Accept is not open ???? WTF - lets TERM the server");
        return; // something is wrong
    }
    if (!err){
        LogTrace("got a connection", connHandler->nativeSocketFD());
        /// at this point the native socket fd is assigned
        /// so for debug purposes we can stash it in the
        /// fd_inuse list
       
        m_connectionManager.registerConnectionHandler(connHandler);
        // important sequence - if a call to p_start_accept() is defered
        // by allowAnotherConnection() we need to be sure there is still
        // an outstanding connection that will start an accept in the future.
        m_connectionManager.allowAnotherConnection([this](){
            LogTrace("allowAnother Callback");
            p_start_accept();
        });
        #if 0
        auto hf = std::bind(&ConnectionHandler::serve, connHandler);
        m_io.post(hf);
        #else
        connHandler->serve();
        #endif
    }else{
//        std::cout << __FUNCTION__ << " error : " << err.message() << std::endl;
        LogTrace("Accept error value:",err.value()," cat:", err.category().name(), "message: ",err.message());
        m_io.stop();
        return;
//        delete connHandler;
    }
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
 void TcpServer::p_wait_for_stop()
{
    LogDebug("");
    auto hf = (std::bind(&TcpServer::p_do_stop, this, std::placeholders::_1));

  m_signals.async_wait(hf);
}
 void TcpServer::p_do_stop(const Marvin::ErrorType& err)
{
    LogDebug("");
    m_io.stop();
    m_acceptor.close();
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