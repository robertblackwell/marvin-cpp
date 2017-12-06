

//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <cassert>
#include <csignal>
#include <iostream>
#include <thread>

#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <pthread.h>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)
#include "repeating_timer.hpp"
#include "marvin_error.hpp"

#define USE_BOOST_INTERRUPT 0

boost::asio::io_service _server_io;
boost::asio::io_service _client_io;

void testRepeatTimer(boost::asio::io_service & io_service, std::string message, int how_many_times) {
//    boost::asio::io_service io_service;
    int n = how_many_times;
    RepeatingTimer rt(io_service, 2000);
    rt.start([&n, &message](const boost::system::error_code& ec)->bool{
#if USE_BOOST_INTERRUPT
        boost::this_thread::interruption_point();
#endif
        printf("%s\n", message.c_str());
        LogDebug(" repeating call back n:", n);
        if( n == 0) return false;
        n--;
        return true;
    });
    io_service.run();
    std::cout << "testRepeatTimer:: after io.run " << message << std::endl;
    
}


void
serverFunction()
{
    boost::asio::io_service& io = _server_io;
    boost::asio::signal_set signals(io, SIGINT, SIGTERM, SIGQUIT);
    signals.async_wait([&io](const boost::system::error_code& er, int signal_number){
        std::cout << "server signal handler " << signal_number << std::endl;

    });
#if 0
    sleep(100);
    std::cout << "server after sleep" << std::endl;
#else
    testRepeatTimer(_server_io, "server", 100);
    std::cout << "server after timer" << std::endl;
#endif
    return;
}
/**
* This function runs the testcases
*/
void clientFunction()
{
    testRepeatTimer(_client_io, "client", 2);
}
#if 0
int main(){
    RBLogging::setEnabled(false);
    std::cout.setf(std::ios::unitbuf);
    long retCode;
    void* rcodePtr = &retCode;;
    /**
    * start server
    */

    boost::thread server_thread(serverFunction);
    boost::thread client_thread(clientFunction);
    
    
    client_thread.join();
#if USE_BOOST_INTERRUPT
    server_thread.interrupt();
#endif
    raise(SIGABRT);
    server_thread.join();
    return 0;
    exit(0); // server will not exit
    
    printf("after joins \r");

}
#else
/**
** This is aperfectly good model for threads and io_service handling of signals
** BUT - it does not work in the XCode debugger - run the executable from the command line
**/
int main()
{

  // Initiate thread that will run the io_service and a timer to ensure
  // the io_service is in wait when we send signal.  This will invoke
  // the queued handler that is ready for completion.
  std::thread work_thread([]() {
      boost::asio::io_service io_service;
      // Prevent io_service from running out of work.
      boost::asio::io_service::work work(io_service);
      
      // set up signal handling
      boost::asio::signal_set signal_set(io_service, SIGTERM);
      auto handler = [&io_service](
           const boost::system::error_code& error,
           int signal_number)
        {
          std::cout << "Handler Got signal " << signal_number << "; "
                       "stopping io_service." << std::endl;
          io_service.stop();
        };
      signal_set.async_wait(handler);
      
      // set up a long timer to ensure io_service is waiting
      std::cout << "set up timer" << std::endl;
      boost::asio::deadline_timer timer(io_service);
      timer.expires_from_now(boost::posix_time::milliseconds(100*1000));
      timer.async_wait([](const boost::system::error_code& err){
          std::cout << "timer finished" << std::endl;
      });

      io_service.run();
      std:: cout << "thread after io.run " << std::endl;
  });

  // Raise SIGTERM. But wait long enough for other thread to get into wait
  sleep(10);
  std::cout << "raise" << std::endl;
  
  std::raise(SIGTERM); // sends a signal
  std::cout << "raise" << std::endl;


  // By the time raise() returns, Boost.Asio has handled SIGTERM with its
  // own internal handler, queuing it internally.  At this point, Boost.Asio
  // is ready to dispatch this notification to a user signal handler
  // (i.e. those provided to signal_set.async_wait()) within the
  // io_service event loop.
//  std::cout << "io_service stopped? " << io_service.stopped() << std::endl;
//  assert(false == io_service.stopped());
  
  std::cout << "before join" << std::endl;
  // Synchornize on the work_thread.  Letting it run to completion.
  work_thread.join();
  std::cout << "after join" << std::endl;

  // The io_service has been explicitly stopped in the async_wait
  // handler.
//  std::cout << "io_service stopped? " << io_service.stopped() << std::endl;
//  assert(true == io_service.stopped());
}
#endif
