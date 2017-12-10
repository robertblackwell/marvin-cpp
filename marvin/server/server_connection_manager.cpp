#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "server_connection_manager.hpp"

ServerConnectionManager* ServerConnectionManager::instance;

ServerConnectionManager* ServerConnectionManager::get_instance()
{
    return instance;
}
bool ServerConnectionManager::verify()
{
    return true;
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
    LogDebug("nativeSocket:: ", connHandler->nativeSocketFD());
    auto pf = _serverStrand.wrap(std::bind(&ServerConnectionManager::_register, this, connHandler));
    _io.post(pf);
//    _connections[connHandler] = std::unique_ptr<ConnectionHandler>(connHandler);
}

void ServerConnectionManager::_register(ConnectionHandler* connHandler)
{
    long fd = connHandler->nativeSocketFD();
    assert(_fd_list.find(fd) == _fd_list.end());
    assert(_connections.find(connHandler) == _connections.end()); // assert not already there
    assert(verify());
    _connections[connHandler] = std::unique_ptr<ConnectionHandler>(connHandler);
    _fd_list[fd] = fd;
    assert(verify());
}


void ServerConnectionManager::deregister(ConnectionHandler* ch)
{
    LogDebug("nativeSocket:: ", ch->nativeSocketFD());
    auto pf = _serverStrand.wrap(std::bind(&ServerConnectionManager::_deregister, this, ch));
    _io.post(pf);
}
/**
* Paranoid
*/
void ServerConnectionManager::_deregister(ConnectionHandler* ch)
{
    LogDebug("");
    long fd = ch->nativeSocketFD();
    assert(_fd_list.find(fd) != _fd_list.end());
    if(_fd_list.find(fd) == _fd_list.end()) {
        auto fd1 = _fd_list.find(fd);
        auto fd2 = _fd_list.end();
        assert(_fd_list.find(fd) != _fd_list.end());
    }
    assert(_connections.find(ch) != _connections.end()); // assert is there
    assert(verify());
    
    _connections.erase(ch);
    _fd_list.erase(fd);
    if (_callback && (_connections.size() < _maxNumberOfConnections)) {
        LogDebug("");
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

