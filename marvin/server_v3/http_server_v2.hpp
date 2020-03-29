
#ifndef HTTP_SERVER_v2_HPP
#define HTTP_SERVER_v2_HPP

#include <string>
#include <signal.h>
#include <utility>

#include <marvin/boost_stuff.hpp>
#include <marvin/error/marvin_error.hpp>

#include <marvin/connection/socket_interface.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>

#include <marvin/server_v2/server_connection_manager_v2.hpp>
#include <marvin/server_v2/request_handler_base_v2.hpp>
#include <marvin/server_v2/connection_handler_v2.hpp>

namespace Marvin {

/**
* @brief Http server class.
* @discussion TRequestHandler is a template argument that must conform to RequestHandlerBase
* for a class that will handle an http request and send the necessary response.
*
* Discussion of structure
*
*   TRequestHandler is a "user" provided class instance that actually handles a one or more request
*   on a single connection.
*
*   connection_handler (ConnectionHandler) wraps a single instance of TRequestHandler and manages
*   the life cycle of that TRequestHandler instance, and thus represents the life time of a single client
*   connection.
*
*   server_connection_manager (ServerConnectionManager)
*   -   only one of these exists. That instance
*   -   keeps track of  a smart pointer to every active instane of ConnectionHandler
*       thereby ensures they stay in existence while active and can be deleted en-mass if/when the
*       server needs to close down
*   -   limits the number of active ConnectionHandler instances at any point in time to ensure that
*       the server does not get overloaded.
*
*/
class HttpServerV2
{
public:

    /**
    * \brief This is for testing only and should be called after all clients have finished
    * to verify that all connections are closed, all connection handlers have been deleted
    * and all sockets are freed up
    */
    static bool verify();

    static void configSet_NumberOfConnections(int num);
    static void configSet_NumberOfThreads(int num);
    static void configSet_HeartbeatInterval(int millisecs);
    static HttpServerV2* get_instance();
    
    HttpServerV2(const HttpServerV2&) = delete;
    HttpServerV2& operator=(const HttpServerV2&) = delete;

    /**
    ** @brief Construct the server to listen on the specified TCP address and port.
    ** @param port defaults to 9991
    */
    explicit HttpServerV2(RequestHandlerFactoryV2 factory);
    ~HttpServerV2();
    /**
    ** @brief starts the listen process on the servers port, and from there
    ** dispatches instances of TRequestHandler to service the connection
    */
    void listen(long port = 9991);
    void terminate();
private:

    static int s_numberOfThreads;
    static int s_numberOfConnections;
    static int s_heartbeat_interval_ms;
    static HttpServerV2* s_instance;

    /**
    ** @brief just as it says - init the server ready to list
    */
    void p_initialize();
    
    /**
    ** @brief Initiates an asynchronous accept operation.
    */
    void p_start_accept();
    
    /**
    ** @brief callback that is invoked on completio of an accept call
    ** @param handler an object responsible for managing the new connection established by the
    **          completed accept call.
    ** @param err a boost errorcide that described any error condition
    */
    void p_handle_accept(ConnectionHandlerV2* handler, const boost::system::error_code& err);

    /**
    ** @brief sets up a signal callback
    */
    void p_wait_for_stop();
    
    /**
    ** @brief IS the signal callback
    */
    void p_do_stop(const Marvin::ErrorType& err);
    void p_start_heartbeat();
    void p_on_heartbeat(const boost::system::error_code& ec);

    
    int                                             m_heartbeat_interval_ms;
    int                                             m_numberOfThreads;
    int                                             m_numberOfConnections;
    long                                            m_port;
    boost::asio::io_service                         m_io;
    boost::asio::signal_set                         m_signals;
    boost::asio::ip::tcp::acceptor                  m_acceptor;
    ServerConnectionManagerV2                       m_connectionManager;
    RequestHandlerFactoryV2                         m_factory;
    boost::asio::deadline_timer                     m_heartbeat_timer;
    bool                                            m_terminate_requested; // heartbeat will terminate server if this is set
};
} // Marvin

#endif // HTTP_SERVER_HPP
