
#include <marvin/external_src/trog/trog.hpp>

Trog_SETLEVEL(LOG_LEVEL_WARN)

#include "connection.hpp"
#include "client_connection_manager.hpp"

ConnectionManager* globalConnectionManager = NULL;

ConnectionManager::ConnectionManager(boost::asio::io_service& io_service): io(io_service), resolver_(io_service)
{
    
}
ConnectionManager* ConnectionManager::getInstance(boost::asio::io_service& io)
{
    if( globalConnectionManager == NULL ){
        globalConnectionManager = new ConnectionManager(io);
    }
    return globalConnectionManager;
}
/**
 * get a connection to the scheme::server
 */
void ConnectionManager::asyncGetConnection(
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
    
    Connection* conn = new Connection(io, scheme, server, service);
    //
    // a bunch of logic here about find existing, add to connection table etc
    //
    conn->asyncConnect([this, conn, cb](Marvin::ErrorType& ec, Connection* conn){
        if( !ec ){
            postSuccess(cb, conn);
        }else{
            postFail(cb, ec);
        }
    });

}
void ConnectionManager::releaseConnection(Connection* conn)
{
    LogDebug("");
    delete conn;
}
void ConnectionManager::postSuccess(ConnectCallbackType cb, Connection* conn)
{
    Marvin::ErrorType merr = Marvin::make_error_ok();
    auto pf = std::bind(cb, merr, conn);
    io.post(pf);
}
void ConnectionManager::postFail(ConnectCallbackType cb, Marvin::ErrorType& ec)
{
    Marvin::ErrorType merr = ec;
    auto pf = std::bind(cb, merr, nullptr);
    io.post(pf);
}
