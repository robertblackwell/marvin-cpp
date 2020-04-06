
#ifndef marvin_http_server_v3_hpp
#define marvin_http_server_v3_hpp

#include <string>
#include <signal.h>
#include <utility>

#include <marvin/boost_stuff.hpp>

#include <marvin/connection/socket_interface.hpp>
#include <marvin/error/marvin_error.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/server_v3/connection_handler.hpp>
#include <marvin/server_v3/request_handler_interface.hpp>
#include <marvin/server_v3/server_connection_manager.hpp>

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
class HttpServer
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
    static HttpServer* get_instance();
    
    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;

    /**
    ** Construct the server. Each request will be serviced by a handler object
    **  that conforms the RequestHandlerBase. A new instances of the handler object
    ** will be returned by each call to the factory function.
    */
    explicit HttpServer(RequestHandlerFactory factory);
    ~HttpServer();
    /**
    ** Starts listen process on the servers port, and from there
    ** dispatches instances of request handler bkects to service the connection
    **
    ** The callback function will be posted to run on the server's io_service
    ** immediately before io_service.run() is called. This allows a caller to
    ** be informed when the io_service is up and running. Good for detecting
    ** when the server becomes fully functional.
    */
    void listen(long port, std::function<void()> ready_cb = nullptr);
    void terminate();
private:

    static int s_numberOfThreads;
    static int s_numberOfConnections;
    static int s_heartbeat_interval_ms;
    static HttpServer* s_instance;

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
    void p_handle_accept(ConnectionHandler* handler, const boost::system::error_code& err);

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
    ServerConnectionManager                       m_connectionManager;
    RequestHandlerFactory                         m_factory;
    boost::asio::deadline_timer                     m_heartbeat_timer;
    bool                                            m_terminate_requested; // heartbeat will terminate server if this is set
};
} // Marvin

#endif // HTTP_SERVER_HPP
