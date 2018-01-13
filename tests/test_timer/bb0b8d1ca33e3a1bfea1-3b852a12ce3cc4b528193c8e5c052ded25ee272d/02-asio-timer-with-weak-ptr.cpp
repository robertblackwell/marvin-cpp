//
// This example implements handler callback guard as weak_callback() template function
// 
// compile with
//   g++ -std=c++11 -g -Wall 02-asio-timer-with-weak-ptr.cpp -lboost_system -o 02-asio-timer-with-weak-ptr
//   clang++ -std=c++11 -g -Wall 02-asio-timer-with-weak-ptr.cpp -lboost_system -o 02-asio-timer-with-weak-ptr
//   
// run:
//   ./02-asio-timer-with-weak-ptr
//   valgrind --leak-check=full --show-leak-kinds=all ./02-asio-timer-with-weak-ptr
//
// output:
//   my_timer_task::my_timer_task()
//   my_timer_task::arm()
//   guard object alive: executing function
//   my_timer_task::timer_expired_callback() 1
//   guard object alive: executing function
//   my_timer_task::timer_expired_callback() 2
//   DELETING TIMER OBJECT
//   my_timer_task::~my_timer_task()
//   guard object released: do not execute function
//

#include <iostream>
#include <functional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>


class my_timer_task;
static std::shared_ptr<my_timer_task> task;


template <typename Guard, typename Function>
struct weak_callback_details
{
   std::weak_ptr<typename Guard::element_type> g_;
   Function f_;

   weak_callback_details(Guard&& g, Function&& f)
      : g_(std::forward<Guard>(g)), f_(std::forward<Function>(f)) { }

   template <typename ...Args> 
   void operator() (Args&& ...args) 
   {
      // weak_ptr is used as guard:
      //  - if weak_ptr is not expired, call provided function
      //  - if weak_ptr is expired, skip the call
      if (auto s = g_.lock())
      {
         std::cout << "guard object alive: executing function" << std::endl;
         f_(std::forward<Args>(args)...);
      }
      else
      {
         std::cout << "guard object released: do not execute function" << std::endl;
      }
   }
};

template <typename Guard, typename Function>
weak_callback_details<Guard, Function> weak_callback(Guard&& g, Function&& f)
{
   return weak_callback_details<Guard, Function>(std::forward<Guard>(g), std::forward<Function>(f));
}


class my_timer_task : public std::enable_shared_from_this<my_timer_task>
{
public:
   my_timer_task(boost::asio::io_service& io_service)
      : timer_(io_service), counter_(0)
   {
      std::cout << "my_timer_task::my_timer_task()" << std::endl;;
   }
   
   ~my_timer_task()
   {
      std::cout << "my_timer_task::~my_timer_task()" << std::endl;
   }

   void arm()
   {
      std::cout << "my_timer_task::arm()" << std::endl;
      
      timer_.expires_from_now(std::chrono::seconds(1));
      timer_.async_wait(weak_callback(shared_from_this(), std::bind(&my_timer_task::timer_expired_callback, this, std::placeholders::_1)));
   }
   
   void timer_expired_callback(const boost::system::error_code& ec)
   {
      std::cout << "my_timer_task::timer_expired_callback() " << ++counter_ << std::endl;;

      if (!ec)
      {
         timer_.async_wait(weak_callback(shared_from_this(), std::bind(&my_timer_task::timer_expired_callback, this, std::placeholders::_1)));

         // Here we demonstrate deleting the timer object.  This would be normally
         // done from outside e.g. when 'my_timer_task' falls out of scope (RAII) 
         if (counter_ == 2)
         {
            std::cout << "DELETING TIMER OBJECT" << std::endl;
            task.reset();
         }
      }
      else
      {
         std::cout << "error_code: " << ec.message() << std::endl;
      }

   }  
         
private:
   boost::asio::steady_timer timer_;
   unsigned int counter_;
};


int
main(int argc, char *argv[])
{
   boost::asio::io_service io_service;

   // In this example shared_ptr must be used for the timer object,
   // since that enables using shared_from_this().  Note also, that
   // shared_from_this() cannot be called directly from constructor,
   // since shared_ptr has been created at that time yet.
   task = std::make_shared<my_timer_task>(io_service);

   task->arm();
   
   io_service.run();

   return 0;
}
