
#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "client_connection.hpp"
#include "client_connection_manager.hpp"

ClientConnectionManager* globalClientConnectionManager = NULL;

ClientConnectionManager::ClientConnectionManager(boost::asio::io_service& io_service): io(io_service), resolver_(io_service)
{
    
}
ClientConnectionManager* ClientConnectionManager::getInstance(boost::asio::io_service& io)
{
    if( globalClientConnectionManager == NULL ){
        globalClientConnectionManager = new ClientConnectionManager(io);
    }
    return globalClientConnectionManager;
}
/**
 * get a connection to the scheme::server
 */
void ClientConnectionManager::asyncGetClientConnection(
            std::string scheme, // http: or https:
            std::string server, // also called hostname
            std::string service,// http/https or port number
            ConnectCallbackType cb
)
{
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),
                                                server,
                                                service,
                                                tcp::resolver::query::canonical_name);
    
    ClientConnection* conn = new ClientConnection(io, scheme, server, service);
    //
    // a bunch of logic here about find existing, add to connection table etc
    //
    conn->asyncConnect([this, conn, cb](Marvin::ErrorType& ec, ClientConnection* conn){
        if( !ec ){
            postSuccess(cb, conn);
        }else{
            postFail(cb, ec);
        }
    });

}
void ClientConnectionManager::releaseClientConnection(ClientConnection* conn)
{
    LogDebug("");
    delete conn;
}
void ClientConnectionManager::postSuccess(ConnectCallbackType cb, ClientConnection* conn)
{
    Marvin::ErrorType merr = Marvin::make_error_ok();
    auto pf = std::bind(cb, merr, conn);
    io.post(pf);
}
void ClientConnectionManager::postFail(ConnectCallbackType cb, Marvin::ErrorType& ec)
{
    Marvin::ErrorType merr = ec;
    auto pf = std::bind(cb, merr, nullptr);
    io.post(pf);
}
