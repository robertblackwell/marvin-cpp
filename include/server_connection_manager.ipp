#include "rb_logger.hpp"
//RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
/*#include "connection_handler.hpp"
#include "server_connection_manager.hpp"
*/

#define TMPL template<class TConnectionHandler>
#define TCLASS ServerConnectionManager<TConnectionHandler>

template<class TConnectionHandler>
ServerConnectionManager<TConnectionHandler>::ServerConnectionManager(boost::asio::io_service& io, boost::asio::strand& serverStrand)    : _io(io), _serverStrand(serverStrand)
{
}
template<class TConnectionHandler>
void ServerConnectionManager<TConnectionHandler>::registerConnectionHandler(TConnectionHandler* connHandler)
{
//    std::unique_ptr<ConnectionHandler> hp = std::unique_ptr<ConnectionHandler>(connHandler);
    _connections[connHandler] = std::unique_ptr<TConnectionHandler>(connHandler);
}

template<class TConnectionHandler>
void ServerConnectionManager<TConnectionHandler>::_deregister(TConnectionHandler* ch)
{
    LogDebug("");
//    _connections.erase(ch);
}

template<class TConnectionHandler>
void ServerConnectionManager<TConnectionHandler>::deregister(TConnectionHandler* ch)
{
    LogDebug("");
    auto pf = std::bind(
            &ServerConnectionManager<TConnectionHandler>::_deregister,
            this,
            ch);
    _io.post(pf);
}
template<class TConnectionHandler>
void ServerConnectionManager<TConnectionHandler>::stop_all()
{
//    for (auto c: _connections)
//        c->stop();
//    _connections.clear();
}

