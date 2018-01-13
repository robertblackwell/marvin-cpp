
#ifndef marvin_timeout_hpp
#define marvin_timeout_hpp
/**
* \ingroup SocketIO
*/

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include "boost_stuff.hpp"
#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "buffer.hpp"
#include "i_socket.hpp"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;


using ip::tcp;
using system::error_code;

class Timeout;

using TimeoutSPtr = std::shared_ptr<Timeout>;
using TimeoutUPtr = std::unique_ptr<Timeout>;
/**
* \ingroup SocketIO
* \brief Provides a simple timeout mechanism for async operations, intended as a mixin for other classes.
*
* ## Usage
*
* Provides two interface functions:
*
*      `setTimeout(interval, timeout_handler)`
*
*       This sets the timeout interval and provides the timout handler with a callback to invoke/post
*       if and when the timeout expires before the timeout is cancelled. The handlers purpose is
*       to isolate the Timeout mechaism from knowledge of what action should be taken if an operation is timed out.
*
*       This method should be called just before starting an async operation.
*
*       The timeout_handler should do whatever is appropriate to abort the timed out async operation.
*       For boost socksts/streams the appropriate action is `socket.cancel()`
*
*       `cancelTimeout(cancel_handler)`
*       Called by a completion handler when an async operation completes before a timeout expired. It cancels the
*       timeout_handler provided in the setTimeout call, cancels the asio deadline timer at the heart of the timeout mechanism
*       and calls/posts the cancel_handler when the internal timeout handler is finally called. Thus when the cancel_handler
*       is called one can be sure the timeout mechansim has no outstanding handlers.
*
* ## Changes to async code
*
* Code that implements async operations on behalf of client code will have to be modified to use this tiimeout mechanism.
*
*   -   `setTimeout()` should be called at the start of all async operations.
*   -   all internal handlers should call `cancelTimeout` and should only invoke or post higher level handlers
*       within the `cancel_hander` passed to `cancelTimeout`
*
* ## io_service
*
*   -   Timeout uses no locking and hence it and its client MUST run in a single threaded io_service`.
*   -   Timeout does not directly call `timeout_handler` or `cancel_handler`, but rather posts them to the io_service provied during
*       construction.
*   -   `timeout_handler` or `cancel_handler` MUST NOT ber `strand.wrap()`'d or not as required by the caller.
*/
class Timeout
{
    public:

    Timeout(
            boost::asio::io_service& io_service
               );
    
    ~Timeout();
    void setTimeout(long interval_millisecs, std::function<void()> handler);
    void cancelTimeout(std::function<void()> handler);

private:
    void p_handle_timeout(const boost::system::error_code& err);
    void p_io_post(std::function<void()> noParam_cb);
    
    boost::asio::io_service&        m_io;
    boost::asio::deadline_timer     m_timer;
    std::function<void()>           m_cancel_handler;
    std::function<void()>           m_expire_handler;
    bool                            m_active;   /// is a call to the hander expected in the future
                                                /// \note this depends requires the timer run on a strand
};


#endif
