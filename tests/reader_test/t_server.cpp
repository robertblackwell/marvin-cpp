
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <gtest/gtest.h>
#include <pthread.h>
#include "boost_stuff.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)
#include "error.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include "tcp_connection.hpp"
#include "message_reader.hpp"
#include "t_server.hpp"

TServer::TServer(boost::asio::io_service& io, Testcase tc): _io(io), _tc(tc), _acceptor(_io)
{

}
void TServer::initialize()
{
    ///
    /// !! make sure this is big enough to handle the components with dedicated strands
    ///
    // Register to handle the signals that indicate when the Server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
//    _signals.add(SIGINT);
//    _signals.add(SIGTERM);
//    #if defined(SIGQUIT)
//    _signals.add(SIGQUIT);
//    #endif // defined(SIGQUIT)
//
//    waitForStop();
    
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), _port);
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);

}
TServer::~TServer()
{
    LogTorTrace();
}
void TServer::listen(long port, std::function<void(MessageReaderSPtr rdr)> cb)
{
    _port = port;
    initialize();
    _acceptor.listen();
    startAccept();
//    // start the accept process on the _serverStrand
//    auto hf = std::bind(&TServer::startAccept, this);
//    _io.post(hf);
}
//-------------------------------------------------------------------------------------
// startAccept
//-------------------------------------------------------------------------------------
void TServer::startAccept()
{
    LogInfo("");
    _conn_sptr = std::shared_ptr<TCPConnection>(new TCPConnection(_io));
//    IReadSocket* rd_sock_ifce = _conn_sptr.get();
    _rdr = std::shared_ptr<MessageReader>(new MessageReader(_io, _conn_sptr));
    auto hf = std::bind(&TServer::handleAccept, this, std::placeholders::_1);
    _conn_sptr->asyncAccept(_acceptor, hf);
}

//-------------------------------------------------------------------------------------
// handleAccept - called on _strand to handle a new client connection
//-------------------------------------------------------------------------------------
void TServer::handleAccept(const boost::system::error_code& err)
{
    LogDebug("");
    if (! _acceptor.is_open()){
        LogWarn("Accept is not open ???? WTF - lets TERM the server");
        return; // something is wrong
    }
    if (!err){
        LogInfo("got a client - start reading");
        _runner_sptr = std::shared_ptr<Testrunner>(new Testrunner(_io, _conn_sptr, _tc));
        if( true )
            _runner_sptr->run_FullMessageRead();
        else
            _runner_sptr->run_StreamingBodyRead();
    }else{
        LogWarn("Accept error value:",err.value()," cat:", err.category().name(), "message: ",err.message());
    }
    // dont start another accept - we process only one connection then stop
//    startAccept();
}
void TServer::read_full_message_test()
{

}
void TServer::read_headers_body_test()
{

}
