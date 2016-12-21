#include "rb_logger.hpp"
//RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
/*#include "connection_handler.hpp"
#include "server_connection_manager.hpp"
*/
template<class CONNECTION_HANDLER>
ServerConnectionManager<CONNECTION_HANDLER>::ServerConnectionManager()
{
}

template<class CONNECTION_HANDLER>
void ServerConnectionManager<CONNECTION_HANDLER>::registerConnectionHandler(CONNECTION_HANDLER* connHandler)
{
//    std::unique_ptr<ConnectionHandler> hp = std::unique_ptr<ConnectionHandler>(connHandler);
#ifdef CM_SMARTPOINTER
    _connections[connHandler] = std::unique_ptr<CONNECTION_HANDLER>(connHandler);
#else
    _connections.insert(connHandler);
#endif
}

template<class CONNECTION_HANDLER>
void ServerConnectionManager<CONNECTION_HANDLER>::stop(CONNECTION_HANDLER* ch)
{
    LogDebug("");
    _connections.erase(ch);
}

template<class CONNECTION_HANDLER>
void ServerConnectionManager<CONNECTION_HANDLER>::deregister(CONNECTION_HANDLER* ch)
{
    LogDebug("");
    _connections.erase(ch);
}
template<class CONNECTION_HANDLER>
void ServerConnectionManager<CONNECTION_HANDLER>::stop_all()
{
//    for (auto c: _connections)
//        c->stop();
//    _connections.clear();
}

