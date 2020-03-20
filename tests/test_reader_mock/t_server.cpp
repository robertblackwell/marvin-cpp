
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <doctest/doctest.h>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "error.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include <marvin/connection/socket_factory.hpp>
#include <marvin/message/message_reader.hpp>
#include "t_server.hpp"

TServer::TServer(boost::asio::io_service& io, Testcase tc): m_io(io), m_tc(tc), m_acceptor(m_io)
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
    LogDebug("");
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_port);
    m_acceptor.open(endpoint.protocol());
    boost::asio::socket_base::reuse_address option(true);
    m_acceptor.set_option(option);
    try {
        m_acceptor.bind(endpoint);
    } catch(boost::system::system_error &e) {
        std::cout << "Bind failed in TServer initialize" << std::endl;
        throw e;
    }
}
TServer::~TServer()
{
    LogTorTrace();
}
void TServer::listen(long port, std::function<void(MessageReaderSPtr rdr)> cb)
{
    LogDebug("");
    m_port = port;
    initialize();
    m_acceptor.listen();
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
    LogDebug("");
    m_conn_sptr = socketFactory(m_io);
    m_rdr = std::shared_ptr<MessageReader>(new MessageReader(m_io, m_conn_sptr));
    auto hf = std::bind(&TServer::handleAccept, this, std::placeholders::_1);
    m_conn_sptr->asyncAccept(m_acceptor, hf);
}

//-------------------------------------------------------------------------------------
// handleAccept - called on _strand to handle a new client connection
//-------------------------------------------------------------------------------------
void TServer::handleAccept(const boost::system::error_code& err)
{
    LogDebug("");
    if (! m_acceptor.is_open()){
        LogWarn("Accept is not open ???? WTF - lets TERM the server");
        return; // something is wrong
    }
    if (!err){
        LogInfo("got a client - start reading");
        m_runner_sptr = std::shared_ptr<Testrunner>(new Testrunner(m_io, m_conn_sptr, m_tc));
        if( true )
            m_runner_sptr->run_FullMessageRead();
        else
            m_runner_sptr->run_StreamingBodyRead();
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
