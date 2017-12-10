#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "server_connection_manager.hpp"

ServerConnectionManager* ServerConnectionManager::instance;

ServerConnectionManager* ServerConnectionManager::get_instance()
{
    return instance;
}


ServerConnectionManager::ServerConnectionManager(boost::asio::io_service& io, boost::asio::strand& serverStrand, int max_connections)
    : _io(io), _serverStrand(serverStrand),
    _maxNumberOfConnections(max_connections),
    _currentNumberConnections(0)
{
    LogTorTrace();
    instance = this;
}

void ServerConnectionManager::allowAnotherConnection(ServerConnectionManager::AllowAnotherCallback cb)
{
    assert(_callback == nullptr);
    if( _connections.size() > _maxNumberOfConnections ) {
        _callback = cb;
        std::cout << "ServerConnectionManager::waiting server" << std::endl;
    } else {
        cb();
    }
}



void ServerConnectionManager::registerConnectionHandler(ConnectionHandler* connHandler)
{
    auto pf = _serverStrand.wrap(std::bind(&ServerConnectionManager::_register, this, connHandler));
    _io.post(pf);
//    _connections[connHandler] = std::unique_ptr<ConnectionHandler>(connHandler);
}

void ServerConnectionManager::_register(ConnectionHandler* connHandler)
{
    _connections[connHandler] = std::unique_ptr<ConnectionHandler>(connHandler);
}


void ServerConnectionManager::deregister(ConnectionHandler* ch)
{
    LogDebug("");
    auto pf = _serverStrand.wrap(std::bind(&ServerConnectionManager::_deregister, this, ch));
    _io.post(pf);
}

void ServerConnectionManager::_deregister(ConnectionHandler* ch)
{
    LogDebug("");
    _connections.erase(ch);
    if (_callback && (_connections.size() < _maxNumberOfConnections)) {
        std::cout << "ServerConnectionManager::releasing callback" << std::endl;
        auto tmp = _callback;
        _callback = nullptr;
        _io.post(tmp);
    }
}

void ServerConnectionManager::stop_all()
{
//    for (auto c: _connections)
//        c->stop();
//    _connections.clear();
}

