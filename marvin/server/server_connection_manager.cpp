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


/**
 * This method is ALWAYS called from the server strand so do not have to post
 * it on that strand.
 */
void ServerConnectionManager::registerConnectionHandler(ConnectionHandler* connHandler)
{
    LogDebug("");
//    std::cout << "register: fd_list.size() " << _fd_list.size() << " "  << std::endl;
//    std::cout << "register: _connections.size() " << _connections.size() << " "  << std::endl;
//    std::cout << "_register: fd_list.size() " << _fd_list.size() << " "  << std::endl;
//    std::cout << "_register: _connections.size() " << _connections.size() << " "  << std::endl;

    long fd = connHandler->nativeSocketFD();
    assert(_fd_list.find(fd) == _fd_list.end());
    assert(_connections.find(connHandler) == _connections.end()); // assert not already there
    assert(verify());
    _connections[connHandler] = std::unique_ptr<ConnectionHandler>(connHandler);
    _fd_list[fd] = fd;
    assert(verify());
}

/**
 * This is called from a ConnectionHandler running on an arbitary io_service.
 * need to post the real action on the server strand.
 */
void ServerConnectionManager::deregister(ConnectionHandler* ch)
{
    LogDebug("nativeSocket:: ", ch->nativeSocketFD());
//    std::cout << "deregister: fd_list.size() " << _fd_list.size() << " "  << std::endl;
//    std::cout << "deregister: _connections.size() " << _connections.size() << " "  << std::endl;
//    std::cout << "deregister: fd " << ch->nativeSocketFD() << " " << std::hex << ch << std::endl;
    auto pf = _serverStrand.wrap(std::bind(&ServerConnectionManager::_deregister, this, ch));
    _io.post(pf);
}
/**
* Paranoid
*/
void ServerConnectionManager::_deregister(ConnectionHandler* ch)
{
    LogDebug("");
//    std::cout << "_deregister: fd " << ch->nativeSocketFD() << " " << std::hex << ch << std::endl;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    long fd = ch->nativeSocketFD();
    auto fd1 = _fd_list.find(fd);
    auto fd2 = _fd_list.end();
#pragma clang diagnostic pop
//    std::cout << "_deregister: fd_list.size() " << _fd_list.size() << " "  << std::endl;
//    std::cout << "_deregister: _connections.size() " << _connections.size() << " "  << std::endl;
//    for(auto const & f : _fd_list) {
//        std::cout << f.first << " " << f.second << std::endl;
//    }
//    std::cout << "XXX fds : " << fd1->first << " " << fd2->first << std::endl;
//    LogDebug("assert fd ", fd1, " ", fd2 );
    assert(_fd_list.find(fd) != _fd_list.end());
    if(_fd_list.find(fd) == _fd_list.end()) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
        auto fd1 = _fd_list.find(fd);
        auto fd2 = _fd_list.end();
#pragma clang diagnostic pop
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

