#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "server_connection_manager.hpp"


ServerConnectionManager::ServerConnectionManager()
{
}

void ServerConnectionManager::registerConnectionHandler(ConnectionHandler* connHandler)
{
//    std::unique_ptr<ConnectionHandler> hp = std::unique_ptr<ConnectionHandler>(connHandler);
#ifdef CM_SMARTPOINTER
    _connections[connHandler] = std::unique_ptr<ConnectionHandler>(connHandler);
#else
    _connections.insert(connHandler);
#endif
}

void ServerConnectionManager::stop(ConnectionHandler* ch)
{
    LogDebug("");
    _connections.erase(ch);
}

void ServerConnectionManager::stop_all()
{
//    for (auto c: _connections)
//        c->stop();
//    _connections.clear();
}

