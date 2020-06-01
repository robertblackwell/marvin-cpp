#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/server/server_connection_manager.hpp>

#include <unistd.h>



namespace Marvin {

bool ServerConnectionManager::verify()
{
    return true;
}

ServerConnectionManager::ServerConnectionManager(boost::asio::io_service& io, TcpServer* tcp_server_ptr, int max_connections)
    : m_io(io),
    m_maxNumberOfConnections(max_connections),
    m_currentNumberConnections(0),
    m_parent_server_ptr(tcp_server_ptr)
{
   TROG_TRACE_CTOR();
    m_connection_count = 0;
    // instance = this;
}
ServerConnectionManager::~ServerConnectionManager()
{
    TROG_TRACE_CTOR();
    TROG_WARN("num connections: ", m_connections.size());
}
void ServerConnectionManager::allow_another_connection(ServerConnectionManager::AllowAnotherCallback cb)
{
    assert(m_allow_more_callback == nullptr);
   TROG_TRACE3(" num conn: ", m_connections.size(), " max: ", m_maxNumberOfConnections);
    if( m_connections.size() >= m_maxNumberOfConnections ) {
        TROG_WARN("max connections exceeded - waiting cb: ", (void*)(&cb));
        m_allow_more_callback = cb;
    } else {
        cb();
    }
}

TcpServer* ServerConnectionManager::get_tcp_server_ptr()
{
    return m_parent_server_ptr;
}

/**
 * This method is ALWAYS called from the server strand so do not have to post
 * it on that strand.
 */
void ServerConnectionManager::register_connection_handler(ConnectionHandler* connHandler)
{
    TROG_DEBUG("");
    TROG_TRACE3("register_connection_handler num connections: ", m_connections.size());
    long fd = connHandler->native_socket_fd();
    assert(m_fd_list.find(fd) == m_fd_list.end());
    assert(m_connections.find(connHandler) == m_connections.end()); // assert not already there
    assert(verify());
    m_connections[connHandler] = std::unique_ptr<ConnectionHandler>(connHandler);
    m_fd_list[fd] = fd;
    assert(verify());
}

/**
 * This is called from a ConnectionHandler running on an arbitary io_service.
 * need to post the real action on the server strand.
 */
void ServerConnectionManager::deregister(ConnectionHandler* ch)
{
    TROG_TRACE3("deregister nativeSocket:: ", ch->native_socket_fd());
    TROG_TRACE3("deregister num connections:: ", m_connections.size());
    p_deregister(ch);
    // TROG_TRACE3(" num conn: ", m_connection_count, " used FDS: ", getdtablesize());
    return;
}
/**
* Paranoid
*/
void ServerConnectionManager::p_deregister(ConnectionHandler* ch)
{
    TROG_DEBUG("");
//    std::cout << "_deregister: fd " << ch->native_socket_fd() << " " << std::hex << ch << std::endl;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    long fd = ch->native_socket_fd();
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

    TROG_TRACE3(" p_deregister num connections : ", m_connections.size() ," num FDs: ", getdtablesize(), " m_callback != null", (m_allow_more_callback != nullptr));
    if (m_allow_more_callback && (m_connections.size() < m_maxNumberOfConnections)) {
       TROG_TRACE3("p_deregister  allowing more connections from inside p_deregister");
        auto tmp = m_allow_more_callback;
       TROG_TRACE3("releasing cb: ", (void*)(&tmp) );
        m_allow_more_callback = nullptr;
        m_io.post(tmp);
    }
}

void ServerConnectionManager::stop_all()
{

}
} // namespace Marvin
