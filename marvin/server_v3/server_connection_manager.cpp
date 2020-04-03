#include <unistd.h>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN|LOG_LEVEL_TORTRACE|LOG_LEVEL_TRACE)
#include <marvin/server_v3/server_connection_manager.hpp>
using namespace Marvin;

ServerConnectionManager* ServerConnectionManager::instance;

ServerConnectionManager* ServerConnectionManager::get_instance()
{
    return instance;
}
bool ServerConnectionManager::verify()
{
    return true;
}

ServerConnectionManager::ServerConnectionManager(boost::asio::io_service& io, int max_connections)
    : m_io(io),
    m_maxNumberOfConnections(max_connections),
    m_currentNumberConnections(0)
{
    LogTorTrace();
    m_connection_count = 0;
    instance = this;
}

void ServerConnectionManager::allowAnotherConnection(ServerConnectionManager::AllowAnotherCallback cb)
{
    assert(m_allow_more_callback == nullptr);
    LogTrace(" num conn: ", m_connections.size(), " max: ", m_maxNumberOfConnections);
    if( m_connections.size() >= m_maxNumberOfConnections ) {
        LogWarn("max connections exceeded - waiting cb: ", (void*)(&cb));
        m_allow_more_callback = cb;
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
#ifdef DISABLE_SCMGR
    LogTrace(" num conn: ", m_connection_count);
    return;
#else
    LogTrace("registerConnectionHandler num connections: ", m_connections.size());
    long fd = connHandler->nativeSocketFD();
    assert(m_fd_list.find(fd) == m_fd_list.end());
    assert(m_connections.find(connHandler) == m_connections.end()); // assert not already there
    assert(verify());
    m_connections[connHandler] = std::unique_ptr<ConnectionHandler>(connHandler);
    m_fd_list[fd] = fd;
    assert(verify());
#endif
}

/**
 * This is called from a ConnectionHandler running on an arbitary io_service.
 * need to post the real action on the server strand.
 */
void ServerConnectionManager::deregister(ConnectionHandler* ch)
{
    LogTrace("deregister nativeSocket:: ", ch->nativeSocketFD());
    LogTrace("deregister num connections:: ", m_connections.size());
    #if 0
    auto pf = (std::bind(&ServerConnectionManager::p_deregister, this, ch));
    m_io.post(pf);
    #else
    p_deregister(ch);
    #endif
    LogTrace(" num conn: ", m_connection_count, " used FDS: ", getdtablesize());
    return;
}
/**
* Paranoid
*/
void ServerConnectionManager::p_deregister(ConnectionHandler* ch)
{
    LogDebug("");
//    std::cout << "_deregister: fd " << ch->nativeSocketFD() << " " << std::hex << ch << std::endl;
#if 1
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    long fd = ch->nativeSocketFD();
    auto fd1 = m_fd_list.find(fd);
    auto fd2 = m_fd_list.end();
#pragma clang diagnostic pop
    assert(m_fd_list.find(fd) != m_fd_list.end());
    if(m_fd_list.find(fd) == m_fd_list.end()) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
        auto fd1 = m_fd_list.find(fd);
        auto fd2 = m_fd_list.end();
#pragma clang diagnostic pop
        assert(m_fd_list.find(fd) != m_fd_list.end());
    }
#endif
    assert(m_connections.find(ch) != m_connections.end()); // assert is there
    assert(verify());
    
    m_connections.erase(ch);
#if 1
    m_fd_list.erase(fd);
    long num_fds = (long)getdtablesize();
#endif
    LogTrace(" p_deregister num connections : ", m_connections.size() ," num FDs: ", getdtablesize(), " m_callback != null", (m_allow_more_callback != nullptr));
    if (m_allow_more_callback && (m_connections.size() < m_maxNumberOfConnections)) {
        LogTrace("p_deregister  allowing more connections from inside p_deregister");
        auto tmp = m_allow_more_callback;
        LogTrace("releasing cb: ", (void*)(&tmp) );
        m_allow_more_callback = nullptr;
        m_io.post(tmp);
    }
}

void ServerConnectionManager::stop_all()
{
//    for (auto c: _connections)
//        c->stop();
//    _connections.clear();
}

