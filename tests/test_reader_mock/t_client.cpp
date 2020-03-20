
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "error.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include <marvin/connection/socket_factory.hpp>
#include <marvin/message/message_reader.hpp>
#include "t_client.hpp"


TClient::TClient(boost::asio::io_service& io, std::string scheme, std::string server, std::string port, Testcase tc)
: m_io(io), m_scheme(scheme), m_server(server), m_port(port), m_testcase(tc), m_timer(m_io)
{
    m_conn_sptr = socketFactory(m_io, m_scheme,m_server,m_port);
}

void TClient::send_testcase_buffers(SysErrorCb cb)
{
    LogDebug("");
    m_buffer_index = 0;
    m_test_cb = cb;
    connect();
}

void TClient::connect()
{
    LogDebug("");
    m_conn_sptr->asyncConnect([this](Marvin::ErrorType& err, ISocket* conn) {
        LogDebug("connected");
        if( ! err ){
            auto wbf = std::bind(&TClient::wait_before_write, this);
            m_io.post(wbf);
        } else {
            Marvin::ErrorType me = err;
            LogError("error_value", err.value(), " message: ", err.message());
            m_test_cb(me);
        }
    });

}

void TClient::write_line()
{
    LogDebug("");
    std::string line = m_testcase.lineAt(m_buffer_index);
    LogDebug(" line: ", line);
    if (line == "eof" ) {
        m_conn_sptr->shutdown();
    } else if (line == "close") {
        m_conn_sptr->close();
    } else {
        auto hf = std::bind(&TClient::handle_write_complete, this, std::placeholders::_1, std::placeholders::_2);
        auto buf = boost::asio::buffer(line.c_str(), line.size());
        
        m_conn_sptr->asyncWrite(line, hf);
    }
}
void TClient::handle_write_complete(Marvin::ErrorType& err, std::size_t bytes_transfered)
{
    LogDebug("");
    if( !err) {
        m_buffer_index++;
        if(m_buffer_index >= m_testcase.buffers().size()) {
            Marvin::ErrorType err_val = err;
            m_test_cb(err_val);
        } else {
            write_line();
        }
    } else {
        Marvin::ErrorType err_val = err;
        m_test_cb(err_val);
    }
}
void TClient::wait_before_write()
{
    LogDebug("");
    m_timer.expires_from_now(boost::posix_time::milliseconds(100));
//    auto ds = boost::bind(&TClient::write_line, this, std::placeholders::_1);
    m_timer.async_wait([this](const boost::system::error_code& err) {
        write_line();
    });

}


