//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <thread>
#include <marvin/helpers/macros.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/server_v3/http_server.hpp>

RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

using namespace Marvin;

int HttpServer::s_numberOfThreads = 4;
int HttpServer::s_numberOfConnections = 35;
int HttpServer::s_heartbeat_interval_ms = 1000;

HttpServer* HttpServer::s_instance = nullptr;

HttpServer* HttpServer::get_instance()
{
    return s_instance;
}
void HttpServer::configSet_NumberOfThreads(int n)
{
    s_numberOfThreads = n;
}
void HttpServer::configSet_NumberOfConnections(int n)
{
    s_numberOfConnections = n;
}
void HttpServer::configSet_HeartbeatInterval(int millisecs)
{
    s_heartbeat_interval_ms = millisecs;
}
bool HttpServer::verify()
{
    return true;
}



HttpServer::HttpServer(RequestHandlerFactory factory):
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
 void HttpServer::p_initialize()
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
HttpServer::~HttpServer()
{
    LogTorTrace();
}
void HttpServer::terminate()
{
    m_terminate_requested = true;
}
#pragma mark - listen, accept processing
//-------------------------------------------------------------------------------------
// listen - starts the accept cycle
//-------------------------------------------------------------------------------------
 void HttpServer::listen(long port, std::function<void()> ready_cb)
{
    m_port = port;
    p_initialize();
    m_acceptor.listen();
    
    // start the accept process on the _serverStrand
    auto hf = std::bind(&HttpServer::p_start_accept, this);
    m_io.post(hf);
    if (ready_cb != nullptr) {
        m_io.post(ready_cb);
    }
    m_io.run();
}
//-------------------------------------------------------------------------------------
// startAccept
//-------------------------------------------------------------------------------------
 void HttpServer::p_start_accept()
{

    LogInfo("");
    ::ISocketSPtr conn_sptr = socketFactory(m_io);
    
    ConnectionHandler* connectionHandler = new ConnectionHandler(m_io, m_connectionManager, conn_sptr, m_factory);

    auto hf = (std::bind(&HttpServer::p_handle_accept, this, connectionHandler, std::placeholders::_1));
    conn_sptr->asyncAccept(m_acceptor, hf);
//    _acceptor.async_accept(_boost_socket, hf);

}

//-------------------------------------------------------------------------------------
// handleAccept - called on _strand to handle a new client connection
//-------------------------------------------------------------------------------------
 void HttpServer::p_handle_accept(ConnectionHandler* connHandler, const boost::system::error_code& err)
{
    LogInfo("", connHandler);
    if (! m_acceptor.is_open()){
        delete connHandler;
        LogWarn("Accept is not open ???? WTF - lets TERM the server");
        return; // something is wrong
    }
    if (!err){
        LogInfo("got a connection", connHandler->nativeSocketFD());
        /// at this point the native socket fd is assigned
        /// so for debug purposes we can stash it in the
        /// fd_inuse list
       
        m_connectionManager.registerConnectionHandler(connHandler);
        // important sequence - if a call to p_start_accept() is defered
        // by allowAnotherConnection() we need to be sure there is still
        // an outstanding connection that will start an accept in the future.
        m_connectionManager.allowAnotherConnection([this](){
            LogWarn("allowAnother Callback");
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
        LogWarn("Accept error value:",err.value()," cat:", err.category().name(), "message: ",err.message());
        m_io.stop();
        return;
//        delete connHandler;
    }
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
 void HttpServer::p_wait_for_stop()
{
    LogDebug("");
    auto hf = (std::bind(&HttpServer::p_do_stop, this, std::placeholders::_1));

  m_signals.async_wait(hf);
}
 void HttpServer::p_do_stop(const Marvin::ErrorType& err)
{
    LogDebug("");
    m_io.stop();
    m_acceptor.close();
}
void HttpServer::p_on_heartbeat(const boost::system::error_code& ec)
{
    if(m_terminate_requested) {
        m_connectionManager.stop_all();
        m_acceptor.cancel();
        return;
    }
    p_start_heartbeat();
}
void HttpServer::p_start_heartbeat()
{
    m_heartbeat_timer.expires_from_now(boost::posix_time::milliseconds(m_heartbeat_interval_ms));
    auto ds = (boost::bind(&HttpServer::p_on_heartbeat, this, boost::asio::placeholders::error));
    m_heartbeat_timer.async_wait(ds);
}
