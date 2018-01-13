//
// This example implements handler callback guard as lambda function inside arm() method.
//
// compile with
//   g++ -std=c++11 -g -Wall 01-asio-timer-with-weak-ptr.cpp -lboost_system -o 01-asio-timer-with-weak-ptr
//   clang++ -std=c++11 -g -Wall 01-asio-timer-with-weak-ptr.cpp -lboost_system -o 01-asio-timer-with-weak-ptr
//
// run:
//   ./01-asio-timer-with-weak-ptr
//   valgrind --leak-check=full --show-leak-kinds=all ./01-asio-timer-with-weak-ptr
//
// output:
//   my_timer_task::my_timer_task()
//   my_timer_task::arm()
//   running lambda
//   my_timer_task::arm()
//   my_timer_task::timer_expired_callback() 1
//   running lambda
//   my_timer_task::arm()
//   my_timer_task::timer_expired_callback() 2
//   DELETING TIMER OBJECT
//   my_timer_task::~my_timer_task()
//   running lambda
//   timer ec: Operation canceled
//

#include <iostream>
#include <functional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>


class my_timer_task;
static std::shared_ptr<my_timer_task> g_task;



class my_timer_task : public std::enable_shared_from_this<my_timer_task>
{
public:
   my_timer_task(boost::asio::io_service& io_service)
      : timer_(io_service), counter_(0)
    {
        std::cout << "my_timer_task::my_timer_task()" << std::endl;;
        m_cancel_flag = false;
   }
   
   ~my_timer_task()
   {
      std::cout << "my_timer_task::~my_timer_task()" << std::endl;
   }

   void arm()
   {
      std::cout << "my_timer_task::arm()" << std::endl;

      timer_.expires_from_now(std::chrono::seconds(1));
#define XWEAKSPTR
#define SHARED_FROM_THIS
#ifdef WEAKSPTR
      // weak_ptr is used as guard:
      //  - if weak_ptr is not expired, call timer_expired_callback() member method with valid 'this'
      //  - if weak_ptr is expired, skip calling member method since 'this' is not valid anymore
      std::weak_ptr<my_timer_task> self(shared_from_this());
      timer_.async_wait(
         [this, self](const boost::system::error_code& ec)
         {
            std::cout << "running lambda 1" << std::endl;

            // check the 'self' weak_ptr to check if it is safe to use 'this'
            if (!ec)
            {
               if (auto s = self.lock())
               {
                  // timer object alive -> safe to use 'this'
                  arm(); // re-arm the timer
                  timer_expired_callback();
               }
            }

            // timer object not alive anymore -> do not use 'this'
            if (ec)
            {
               std::cout << "timer ec: " << ec.message() << std::endl;
            }
         });
#else
#ifdef SHARED_FROM_THIS
    std::shared_ptr<my_timer_task> self(shared_from_this());
    timer_.async_wait(
        [this, self](const boost::system::error_code& ec)
         {
            std::cout << "running lambda 2" << std::endl;

            // check the 'self' weak_ptr to check if it is safe to use 'this'
            if (!ec && ( ! m_cancel_flag))
            {
                this->arm(); // re-arm the timer
                this->timer_expired_callback();
            }

            // timer object not alive anymore -> do not use 'this'
            if (ec || m_cancel_flag)
            {
               std::cout << "timer ec: " << ec.message() << std::endl;
            }
         });
#else
    timer_.async_wait(
        [this](const boost::system::error_code& ec)
         {
            std::cout << "running lambda 3" << std::endl;

            // check the 'self' weak_ptr to check if it is safe to use 'this'
            if (!ec)
            {
                this->arm(); // re-arm the timer
                this->timer_expired_callback();
            }

            // timer object not alive anymore -> do not use 'this'
            if (ec)
            {
                long i = counter_; // this might crash depending as this is now a dangling pointer
                std::cout << "timer ec: " << ec.message() << std::endl;
            }
         });
#endif
#endif
   }
   
   void timer_expired_callback()
   {
      std::cout << "my_timer_task::timer_expired_callback() " << counter_ << std::endl;;

      // Here we demonstrate deleting the timer object.  This would be normally
      // done from outside e.g. when 'my_timer_task' falls out of scope (RAII)
      if (counter_ == 2)
      {
         std::cout << "DELETING TIMER OBJECT" << std::endl;
         this->m_cancel_flag = true;
         g_task.reset();
      }
      counter_ ++;
   }
    
private:
   boost::asio::steady_timer timer_;
   unsigned int counter_;
   bool m_cancel_flag;
};


int
main(int argc, char *argv[])
{
   boost::asio::io_service io_service;

   // In this example shared_ptr must be used for the timer object,
   // since that enables using shared_from_this().  Note also, that
   // shared_from_this() cannot be called directly from constructor,
   // since shared_ptr has been created at that time yet.
   g_task = std::make_shared<my_timer_task>(io_service);

   g_task->arm();
   
   io_service.run();

   return 0;
}

