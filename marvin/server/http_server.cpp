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

int HTTPServer::__numberOfThreads = 4;
int HTTPServer::__numberOfConnections = 2;
int HTTPServer::__heartbeat_interval_ms = 1000;

HTTPServer* HTTPServer::__instance = nullptr;

HTTPServer* HTTPServer::get_instance()
{
    return __instance;
}
void HTTPServer::configSet_NumberOfThreads(int n)
{
    __numberOfThreads = n;
}
void HTTPServer::configSet_NumberOfConnections(int n)
{
    __numberOfConnections = n;
}
void HTTPServer::configSet_HeartbeatInterval(int millisecs)
{
    __heartbeat_interval_ms = millisecs;
}
bool HTTPServer::verify()
{
    return true;
}



HTTPServer::HTTPServer(RequestHandlerFactory factory)
  : _factory(factory),
    _io(5),
    _signals(_io),
    _acceptor(_io),
    _serverStrand(_io),
    _numberOfConnections(__numberOfConnections),
    _numberOfThreads(__numberOfThreads),
    _heartbeat_interval_ms(__heartbeat_interval_ms),
    _connectionManager(_io, _serverStrand, __numberOfConnections),
    _heartbeat_timer(_io)
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
    _numberOfThreads = __numberOfThreads;
    // Register to handle the signals that indicate when the Server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    _signals.add(SIGINT);
    _signals.add(SIGTERM);
    #if defined(SIGQUIT)
    _signals.add(SIGQUIT);
    #endif // defined(SIGQUIT)
    __instance = this;
#if 0
    auto handler = [this]( const boost::system::error_code& error, int signal_number)
    {
      std::cout << "Handler Got signal " << signal_number << "; "
                   "stopping io_service." << std::endl;
      this->_io.stop();
    };
    _signals.async_wait(handler);
#endif
    
    waitForStop();
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), _port);
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    boost::system::error_code err;
    _acceptor.bind(endpoint, err);
    if( err) {
        LogError("error port: ",_port, err.message());
        exit(1);
    }
    LogDebug(err.message());

}
HTTPServer::~HTTPServer()
{
    LogTorTrace();
}
#pragma mark - listen, accept processing
//-------------------------------------------------------------------------------------
// listen - starts the accept cycle
//-------------------------------------------------------------------------------------
 void HTTPServer::listen(long port)
{
    _port = port;
    initialize();
    _acceptor.listen();
    
    // start the accept process on the _serverStrand
    auto hf = std::bind(&HTTPServer::startAccept, this);
    postOnStrand(hf);
    
#define MULTI_THREAD
#ifndef MULTI_THREAD
    _io.run();
#else
    long numThreads = _numberOfThreads;
    std::thread threads[15];
    
    boost::asio::io_service& tmp_io = _io;
    for(int t_count = 0; t_count < numThreads - 1; t_count++)
    {
        threads[t_count] = std::thread([&tmp_io](){
            LogDebug("thread");
            tmp_io.run();
        });
    }
    LogDebug("original thread");
    _io.run();
    for(int t_count = 0; t_count < numThreads - 1; t_count++)
    {
        threads[t_count-1].join();
    }
#endif
}
//-------------------------------------------------------------------------------------
// startAccept
//-------------------------------------------------------------------------------------
 void HTTPServer::startAccept()
{
    LogInfo("");
    ConnectionInterface* conptr = new TCPConnection(_io);
    
    ConnectionHandler* connectionHandler = new ConnectionHandler(_io, _connectionManager, conptr, _factory);

    auto hf = _serverStrand.wrap(
                    std::bind(&HTTPServer::handleAccept, this, connectionHandler, std::placeholders::_1)
                    );
    conptr->asyncAccept(_acceptor, hf);
}

//-------------------------------------------------------------------------------------
// handleAccept - called on _strand to handle a new client connection
//-------------------------------------------------------------------------------------
 void HTTPServer::handleAccept(ConnectionHandler* connHandler, const boost::system::error_code& err)
{
    LogInfo("", connHandler);
    if (! _acceptor.is_open()){
        delete connHandler;
        LogWarn("Accept is not open ???? WTF - lets TERM the server");
        return; // something is wrong
    }
    if (!err){
        LogInfo("got a connection", connHandler->nativeSocketFD());
        /// at this point the native socket fd is assigned
        /// so for debug purposes we can stash if in the
        /// fd_inuse list
       
        _connectionManager.registerConnectionHandler(connHandler);
        //
        // at this point we are running on _serveStrand start the connectionHandler with a post to
        // liberate it from the strand
        //
//        std::cout << "Server handleAccept " << std::hex << (long) this << " " << (long)connHandler << std::endl;
        auto hf = std::bind(&ConnectionHandler::serve, connHandler);
        _io.post(hf);
    }else{
        LogWarn("Accept error value:",err.value()," cat:", err.category().name(), "message: ",err.message());
        delete connHandler;
    }
    _connectionManager.allowAnotherConnection([this](){
        startAccept();
    });
}

//-------------------------------------------------------------------------------------
// postOnStrand - wraps a parameterless function in _strand and posts to _io
//-------------------------------------------------------------------------------------
 void HTTPServer::postOnStrand(std::function<void()> fn)
{
    auto wrappedFn = _serverStrand.wrap(fn);
    _io.post(wrappedFn);
}
#pragma mark - signal handling
//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
 void HTTPServer::waitForStop()
{
    LogDebug("");
    auto hf = _serverStrand.wrap(
                    std::bind(&HTTPServer::doStop, this, std::placeholders::_1)
                    );

  _signals.async_wait(hf);
}
 void HTTPServer::doStop(const Marvin::ErrorType& err)
{
    LogDebug("");
    std::cout << "doStop" << std::endl;
    _io.stop();
    _acceptor.close();
//  connection_manager_.stop_all();
}
void HTTPServer::on_heartbeat(const boost::system::error_code& ec)
{
    std::cout << __FUNCTION__ << std::endl;
    _heartbeat_timer.expires_from_now(boost::posix_time::milliseconds(_heartbeat_interval_ms));
    auto ds = boost::bind(&HTTPServer::on_heartbeat, this, boost::asio::placeholders::error);
    _heartbeat_timer.async_wait(ds);

}
