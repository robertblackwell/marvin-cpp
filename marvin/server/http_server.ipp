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

template<class TRequestHandler>
int HTTPServer<TRequestHandler>::__numberOfThreads = 4;

template<class TRequestHandler>
void HTTPServer<TRequestHandler>::configSet_NumberOfThreads(int n)
{
    __numberOfThreads = n;
}



template<class TRequestHandler>
HTTPServer<TRequestHandler>::HTTPServer()
  : _io(5),
    _signals(_io),
    _acceptor(_io),
    _serverStrand(_io),
    _connectionManager(_io, _serverStrand)
{
    LogTorTrace();

//    _port = port;
//    initialize();
    // Register to handle the signals that indicate when the Server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
#ifdef IINNIITT
    _signals.add(SIGINT);
    _signals.add(SIGTERM);
    #if defined(SIGQUIT)
    _signals.add(SIGQUIT);
    #endif // defined(SIGQUIT)

    waitForStop();
    
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), _port);
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
#endif
}
/**
** init
*/
template<class TRequestHandler> void HTTPServer<TRequestHandler>::initialize()
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

    waitForStop();
    
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), _port);
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);

}
template<class TRequesstHandler>
HTTPServer<TRequesstHandler>::~HTTPServer()
{
    LogTorTrace();
}
#pragma mark - listen, accept processing
//-------------------------------------------------------------------------------------
// listen - starts the accept cycle
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void HTTPServer<TRequestHandler>::listen(long port)
{
    _port = port;
    initialize();
    _acceptor.listen();
    
    // start the accept process on the _serverStrand
    auto hf = std::bind(&HTTPServer<TRequestHandler>::startAccept, this);
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
template<class TRequestHandler> void HTTPServer<TRequestHandler>::startAccept()
{
    LogInfo("");
    ConnectionInterface* conptr = new TCPConnection(_io);
    
    RequestHandlerBase* hi = new TRequestHandler(_io);
    
    ConnectionHandler<TRequestHandler>* connectionHandler =
        new ConnectionHandler<TRequestHandler>(_io, _connectionManager, conptr);

    auto hf = _serverStrand.wrap(
                    std::bind(&HTTPServer::handleAccept, this, connectionHandler, std::placeholders::_1)
                    );
    conptr->asyncAccept(_acceptor, hf);
}

//-------------------------------------------------------------------------------------
// handleAccept - called on _strand to handle a new client connection
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void HTTPServer<TRequestHandler>::handleAccept(
                                                                ConnectionHandler<TRequestHandler>* connHandler,
                                                                const boost::system::error_code& err)
{
    LogInfo("", connHandler);
    if (! _acceptor.is_open()){
        delete connHandler;
        LogWarn("Accept is not open ???? WTF - lets TERM the server");
        return; // something is wrong
    }
    if (!err){
        LogInfo("got a connection", connHandler->nativeSocketFD());
        
        _connectionManager.registerConnectionHandler(connHandler);
        //
        // at this point we are running on _serveStrand start the connectionHandler with a post to
        // liberate it from the strand
        //
        auto hf = std::bind(&ConnectionHandler<TRequestHandler>::serve, connHandler);
        _io.post(hf);
    }else{
        LogWarn("Accept error value:",err.value()," cat:", err.category().name(), "message: ",err.message());
        delete connHandler;
    }
    startAccept();
    
}

//-------------------------------------------------------------------------------------
// postOnStrand - wraps a parameterless function in _strand and posts to _io
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void HTTPServer<TRequestHandler>::postOnStrand(std::function<void()> fn)
{
    auto wrappedFn = _serverStrand.wrap(fn);
    _io.post(wrappedFn);
}
#pragma mark - signal handling
//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
template<class TRequestHandler> void HTTPServer<TRequestHandler>::waitForStop()
{
    LogDebug("");
    auto hf = _serverStrand.wrap(
                    std::bind(&HTTPServer::doStop, this, std::placeholders::_1)
                    );

  _signals.async_wait(hf);
}
template<class TRequestHandler> void HTTPServer<TRequestHandler>::doStop(const Marvin::ErrorType& err)
{
    LogDebug("");
    _acceptor.close();
//  connection_manager_.stop_all();
}

