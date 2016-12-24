#include "rb_logger.hpp"
//RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
/*#include "connection_handler.hpp"
#include "server_connection_manager.hpp"
*/
template<class TConnectionHandler>
ServerConnectionManager<TConnectionHandler>::ServerConnectionManager()
{
}

template<class TConnectionHandler>
void ServerConnectionManager<TConnectionHandler>::registerConnectionHandler(TConnectionHandler* connHandler)
{
//    std::unique_ptr<ConnectionHandler> hp = std::unique_ptr<ConnectionHandler>(connHandler);
#ifdef CM_SMARTPOINTER
    _connections[connHandler] = std::unique_ptr<TConnectionHandler>(connHandler);
#else
    _connections.insert(connHandler);
#endif
}

template<class TConnectionHandler>
void ServerConnectionManager<TConnectionHandler>::stop(TConnectionHandler* ch)
{
    LogDebug("");
    _connections.erase(ch);
}

template<class TConnectionHandler>
void ServerConnectionManager<TConnectionHandler>::deregister(TConnectionHandler* ch)
{
    LogDebug("");
    _connections.erase(ch);
}
template<class TConnectionHandler>
void ServerConnectionManager<TConnectionHandler>::stop_all()
{
//    for (auto c: _connections)
//        c->stop();
//    _connections.clear();
}

