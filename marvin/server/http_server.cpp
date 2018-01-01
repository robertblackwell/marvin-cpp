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
#include "http_server.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

int HTTPServer::s_numberOfThreads = 4;
int HTTPServer::s_numberOfConnections = 2;
int HTTPServer::s_heartbeat_interval_ms = 1000;

HTTPServer* HTTPServer::s_instance = nullptr;

HTTPServer* HTTPServer::get_instance()
{
    return s_instance;
}
void HTTPServer::configSet_NumberOfThreads(int n)
{
    s_numberOfThreads = n;
}
void HTTPServer::configSet_NumberOfConnections(int n)
{
    s_numberOfConnections = n;
}
void HTTPServer::configSet_HeartbeatInterval(int millisecs)
{
    s_heartbeat_interval_ms = millisecs;
}
bool HTTPServer::verify()
{
    return true;
}



HTTPServer::HTTPServer(RequestHandlerFactory factory)
  : m_factory(factory),
    m_io(5),
    m_signals(m_io),
    m_acceptor(m_io),
    m_serverStrand(m_io),
    m_numberOfConnections(s_numberOfConnections),
    m_numberOfThreads(s_numberOfThreads),
    m_heartbeat_interval_ms(s_heartbeat_interval_ms),
    m_connectionManager(m_io, m_serverStrand, m_numberOfConnections),
    m_heartbeat_timer(m_io),
    m_terminate_requested(false)
{
    LogTorTrace();
}
/**
** init
*/
 void HTTPServer::initialize()
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
    
    waitForStop();
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_port);
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    boost::system::error_code err;
    m_acceptor.bind(endpoint, err);
    if( err) {
        LogError("error port: ",m_port, err.message());
        exit(1);
    }
    start_heartbeat();

    LogDebug(err.message());
}
HTTPServer::~HTTPServer()
{
    LogTorTrace();
}
void HTTPServer::terminate()
{
    m_terminate_requested = true;
}
#pragma mark - listen, accept processing
//-------------------------------------------------------------------------------------
// listen - starts the accept cycle
//-------------------------------------------------------------------------------------
 void HTTPServer::listen(long port)
{
    m_port = port;
    initialize();
    m_acceptor.listen();
    
    // start the accept process on the _serverStrand
    auto hf = std::bind(&HTTPServer::startAccept, this);
    postOnStrand(hf);
    
#define MULTI_THREAD
#ifndef MULTI_THREAD
    m_io.run();
#else
    long numThreads = m_numberOfThreads;
    std::thread threads[15];
    
    boost::asio::io_service& tmp_io = m_io;
    for(int t_count = 0; t_count < numThreads - 1; t_count++)
    {
        threads[t_count] = std::thread([&tmp_io](){
            LogDebug("thread");
            tmp_io.run();
        });
    }
    LogDebug("original thread");
    m_io.run();
//    std::cout << __FUNCTION__ << " after io.run() " << std::endl;
    for(int t_count = 0; t_count < numThreads - 1; t_count++)
    {
        threads[t_count].join();
    }
//    std::cout << __FUNCTION__<<" after join" << std::endl;
#endif
}
//-------------------------------------------------------------------------------------
// startAccept
//-------------------------------------------------------------------------------------
 void HTTPServer::startAccept()
{
    LogInfo("");
    ISocket* conptr = new TCPConnection(m_io);
    
    ConnectionHandler* connectionHandler = new ConnectionHandler(m_io, m_connectionManager, conptr, m_factory);

    auto hf = m_serverStrand.wrap(std::bind(&HTTPServer::handleAccept, this, connectionHandler, std::placeholders::_1));
    conptr->asyncAccept(m_acceptor, hf);
//    _acceptor.async_accept(_boost_socket, hf);

}

//-------------------------------------------------------------------------------------
// handleAccept - called on _strand to handle a new client connection
//-------------------------------------------------------------------------------------
 void HTTPServer::handleAccept(ConnectionHandler* connHandler, const boost::system::error_code& err)
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
        /// so for debug purposes we can stash if in the
        /// fd_inuse list
       
        m_connectionManager.registerConnectionHandler(connHandler);
        //
        // at this point we are running on _serveStrand start the connectionHandler with a post to
        // liberate it from the strand
        //
//        std::cout << "Server handleAccept " << std::hex << (long) this << " " << (long)connHandler << std::endl;
        auto hf = std::bind(&ConnectionHandler::serve, connHandler);
        m_io.post(hf);
    }else{
//        std::cout << __FUNCTION__ << " error : " << err.message() << std::endl;
        LogWarn("Accept error value:",err.value()," cat:", err.category().name(), "message: ",err.message());
        m_io.stop();
        return;
//        delete connHandler;
    }
    m_connectionManager.allowAnotherConnection([this](){
        startAccept();
    });
}

//-------------------------------------------------------------------------------------
// postOnStrand - wraps a parameterless function in _strand and posts to _io
//-------------------------------------------------------------------------------------
 void HTTPServer::postOnStrand(std::function<void()> fn)
{
    auto wrappedFn = m_serverStrand.wrap(fn);
    m_io.post(wrappedFn);
}
#pragma mark - signal handling
//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
 void HTTPServer::waitForStop()
{
    LogDebug("");
    auto hf = m_serverStrand.wrap(
                    std::bind(&HTTPServer::doStop, this, std::placeholders::_1)
                    );

  m_signals.async_wait(hf);
}
 void HTTPServer::doStop(const Marvin::ErrorType& err)
{
    LogDebug("");
//    std::cout << "doStop" << std::endl;
    m_io.stop();
    m_acceptor.close();
//  connection_manager_.stop_all();
}
void HTTPServer::on_heartbeat(const boost::system::error_code& ec)
{
//    std::cout << __FUNCTION__ << std::endl;
    if(m_terminate_requested) {
        m_connectionManager.stop_all();
        m_acceptor.cancel(); // let the accept handler close the server down
                            // all we want here is to cancel the accept handlers waiting
                            // and cancel all connections
//        _heartbeat_timer.cancel();
//        _io.stop();
        return;
    }
    start_heartbeat();

}
void HTTPServer::start_heartbeat()
{
    m_heartbeat_timer.expires_from_now(boost::posix_time::milliseconds(m_heartbeat_interval_ms));
    auto ds = m_serverStrand.wrap(boost::bind(&HTTPServer::on_heartbeat, this, boost::asio::placeholders::error));
    m_heartbeat_timer.async_wait(ds);
}
