
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "boost_stuff.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "error.hpp"
#include "repeating_timer.hpp"
#include "testcase.hpp"
#include "tcp_connection.hpp"
#include "message_reader.hpp"
#include "t_client.hpp"


TClient::TClient(boost::asio::io_service& io, std::string scheme, std::string server, std::string port, Testcase tc)
: _io(io), _scheme(scheme), _server(server), _port(port), _testcase(tc), _timer(_io)
{
    _conn_sptr = std::shared_ptr<TCPConnection>(new TCPConnection(_io, _scheme, _server, _port));
}

void TClient::send_testcase_buffers(SysErrorCb cb)
{
    LogDebug("");
    _buffer_index = 0;
    _test_cb = cb;
    connect();
}

void TClient::connect()
{
    LogDebug("");
    _conn_sptr->asyncConnect([this](Marvin::ErrorType& err, ISocket* conn) {
        LogDebug("connected");
        if( ! err ){
            auto wbf = std::bind(&TClient::wait_before_write, this);
            _io.post(wbf);
        } else {
            Marvin::ErrorType me = err;
            LogError("error_value", err.value(), " message: ", err.message());
            _test_cb(me);
        }
    });

}

void TClient::write_line()
{
    LogDebug("");
    std::string line = _testcase.lineAt(_buffer_index);
    LogDebug(" line: ", line);
    if (line == "eof" ) {
        _conn_sptr->shutdown();
    } else if (line == "close") {
        _conn_sptr->close();
    } else {
        auto hf = std::bind(&TClient::handle_write_complete, this, std::placeholders::_1, std::placeholders::_2);
        auto buf = boost::asio::buffer(line.c_str(), line.size());
        
        _conn_sptr->asyncWrite(line, hf);
    }
}
void TClient::handle_write_complete(Marvin::ErrorType& err, std::size_t bytes_transfered)
{
    LogDebug("");
    if( !err) {
        _buffer_index++;
        if(_buffer_index >= _testcase.buffers().size()) {
            Marvin::ErrorType err_val = err;
            _test_cb(err_val);
        } else {
            write_line();
        }
    } else {
        Marvin::ErrorType err_val = err;
        _test_cb(err_val);
    }
}
void TClient::wait_before_write()
{
    LogDebug("");
    _timer.expires_from_now(boost::posix_time::milliseconds(100));
//    auto ds = boost::bind(&TClient::write_line, this, std::placeholders::_1);
    _timer.async_wait([this](const boost::system::error_code& err) {
        write_line();
    });

}


