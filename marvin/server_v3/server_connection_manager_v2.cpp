#include <unistd.h>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include <marvin/server_v2/server_connection_manager_v2.hpp>
using namespace Marvin;

ServerConnectionManagerV2* ServerConnectionManagerV2::instance;

ServerConnectionManagerV2* ServerConnectionManagerV2::get_instance()
{
    return instance;
}
bool ServerConnectionManagerV2::verify()
{
    return true;
}

ServerConnectionManagerV2::ServerConnectionManagerV2(boost::asio::io_service& io, int max_connections)
    : m_io(io),
    m_maxNumberOfConnections(max_connections),
    m_currentNumberConnections(0)
{
    LogTorTrace();
    m_connection_count = 0;
    instance = this;
}

void ServerConnectionManagerV2::allowAnotherConnection(ServerConnectionManagerV2::AllowAnotherCallback cb)
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
void ServerConnectionManagerV2::registerConnectionHandler(ConnectionHandlerV2* connHandler)
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
    long fd = connHandler->nativeSocketFD();
    assert(m_fd_list.find(fd) == m_fd_list.end());
    assert(m_connections.find(connHandler) == m_connections.end()); // assert not already there
    assert(verify());
    m_connections[connHandler] = std::unique_ptr<ConnectionHandlerV2>(connHandler);
    m_fd_list[fd] = fd;
    assert(verify());
#endif
}

/**
 * This is called from a ConnectionHandler running on an arbitary io_service.
 * need to post the real action on the server strand.
 */
void ServerConnectionManagerV2::deregister(ConnectionHandlerV2* ch)
{
    LogDebug("nativeSocket:: ", ch->nativeSocketFD());
    LogDebug("num connections:: ", m_connections.size());
    auto pf = (std::bind(&ServerConnectionManagerV2::p_deregister, this, ch));
    m_io.post(pf);
    LogTrace(" num conn: ", m_connection_count, " used FDS: ", getdtablesize());
    return;
}
/**
* Paranoid
*/
void ServerConnectionManagerV2::p_deregister(ConnectionHandlerV2* ch)
{
    LogDebug("");
//    std::cout << "_deregister: fd " << ch->nativeSocketFD() << " " << std::hex << ch << std::endl;
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
    assert(m_connections.find(ch) != m_connections.end()); // assert is there
    assert(verify());
    
    m_connections.erase(ch);
    m_fd_list.erase(fd);
    long num_fds = (long)getdtablesize();
    LogTrace(" num connections : ", m_connections.size() ," num FDs: ", getdtablesize(), " m_callback != null", (m_allow_more_callback != nullptr));
    if (m_allow_more_callback && (m_connections.size() < m_maxNumberOfConnections)) {
        auto tmp = m_allow_more_callback;
        LogTrace("releasing cb: ", (void*)(&tmp) );
        m_allow_more_callback = nullptr;
        m_io.post(tmp);
    }
}

void ServerConnectionManagerV2::stop_all()
{
//    for (auto c: _connections)
//        c->stop();
//    _connections.clear();
}

