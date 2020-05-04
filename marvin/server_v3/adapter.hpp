#ifndef marvin_server_v3_adapter_hpp
#define marvin_server_v3_adapter_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/configure_trog.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/server_context.hpp>
#include <marvin/server_v3/request_handler_interface.hpp>
#include <marvin/server_v3/timer.hpp>
namespace Marvin {
/**
* \brief Brief description of Adapter.
*
* Obsolete Class used to handle requests in the test server for both the bb abd bf
* set of tests. This handler:
*   -   json encodes the full request (headers and body)
*   -   json encodes the uuid for the connection handler and the request handler
*
* and puts that in the body of the response
*/
class Adapter : public Marvin::RequestHandlerInterface
{
public:
    static int counter; // to see if there are multiple instances of the handler
    // boost::asio::deadline_timer m_imer;
    boost::uuids::uuid       m_uuid;
    boost::asio::io_service&            m_io;
    ISocketSPtr                         m_socket_sptr;
    MessageWriterSPtr                   m_wrtr;
    MessageReaderSPtr                   m_rdr;
    Marvin::MessageBaseSPtr       m_msg;
    std::string                         m_body;
    Marvin::HandlerDoneCallbackType     m_done;
    ATimerSPtr                          m_timer_sptr;
    std::function<void()>               m_done_callback;

    Adapter(boost::asio::io_service& io);
    ~Adapter();
    
    void handle(
        Marvin::ServerContext&            server_context,
        ISocketSPtr                       connPtr,
        Marvin::HandlerDoneCallbackType   done
    );

    virtual void handleRequest(
        // ISocketSPtr                       connPtr,
        // MessageWriterSPtr                   m_wrtr,
        // MessageReaderSPtr                   m_rdr
    ) = 0;
protected:
    void p_req_resp_cycle_complete();
    void p_on_completed();
    void p_on_read_error(Marvin::ErrorType err);
    void p_on_write_error(Marvin::ErrorType err);

};
} // namespace
#endif /* test_server_h */
