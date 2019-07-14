#ifndef _thread_safe_queue_hpp_
#define _thread_safe_queue_hpp_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>

#include "Common.hpp"
#include "Logging.hpp"

THREADING_BEGIN
DATA_STRUCTURES_BEGIN

template
<typename element_type_,
 typename container_type_ = std::queue<element_type_> >
class LockQueue
{
public:

   using elem_type = element_type_;
   using container = container_type_;
   using pair_flag_elem = std::pair<bool, elem_type>;

   /**
    * @brief Constructor
    */
   LockQueue(std::size_t max_sz = 0) : defined_maximum_(max_sz)
   {
   }

   /**
    * @brief Tries to enqueue/produce an element. Blocks until the element is enqueued
    *
    * @param element the element that has to be enqueued/produced
    */
   void enqueue(const elem_type& element)
   {
      std::unique_lock<std::mutex> lock(mtx_container);
      LOG("+ Queue size %d") % elem_queue.size();
      cv_produce.wait(lock, [this]() -> bool { return can_produce(); });

      // lock acquired at this point
      LOG("Enqueuing element %d") % element;

      elem_queue.push(element);
      lock.unlock();
      cv_consume.notify_all();
   }

   /**
    * @brief Tries to enqueue an element and returns if it cannot enqueue
    *        e.g. if the lock is held
    *
    * @param element the element that has to be enqueued/produced
    * @return true if the element has been enqueued false otherwise
    */
   bool try_enqueue(const elem_type& element)
   {
      return true;
   }

   /**
    * @brief Tries to enqueue an element and returns if it cannot enqueue
    *        e.g. if the lock is held within the specified timeout
    *
    * @param element the element that has to be enqueued/produced
    * @param timeout interval in milliseconds to try and enqueue the element
    * @return true if the element has been enqueued false otherwise
    */
   bool try_timed_enqueue(const elem_type& element, std::chrono::milliseconds timeout)
   {
      return true;
   }

   /**
    * @brief Tries to dequeue/consume an element. Blocks until the element is dequeued
    *
    * @return the element that has just been dequeud
    */
   elem_type dequeue()
   {
      std::unique_lock<std::mutex> lock(mtx_container);
      LOG("- Queue size %d") % elem_queue.size();
      cv_consume.wait(lock, [this]() -> bool { return can_consume(); });

      // lock acquired at this point
      elem_type ret_elem = elem_queue.front();
      elem_queue.pop();
      LOG("Dequeuing element %d") % ret_elem;

      // check if there is a size restriction defined
      if (can_produce())
      {
         lock.unlock();
         cv_produce.notify_all();
      }

      return ret_elem;
   }

   /**
    * @brief Tries to dequeue/consume an element. Returns if the element cannot be dequeued
    *        If false is returned a default element is constructed and returned
    *
    * @return a pair with the status and element
    */
   pair_flag_elem try_dequeue()
   {
      return pair_flag_elem();
   }

   /**
    * @brief Tries to dequeue/consume an element within the specified amount of time. Returns if the element cannot be dequeued
    *        within the time specified.
    *        If false is returned a default element is constructed and returned
    *
    * @param timeout interval in milliseconds to try and dequeue the element
    * @return a pair with the status and element
    */
   pair_flag_elem try_timed_dequeue(std::chrono::milliseconds timeout)
   {
      return pair_flag_elem();
   }

protected:

   /**
    * @brief Determines if the producer limit has been reached
    *
    * @return true if the limit has been stepped over false otherwise
    */
   bool can_produce()
   {
      if (defined_maximum_ == 0)
      {
         return true;
      }

      if (elem_queue.size() >= defined_maximum_)
      {
         return false;
      }

      return true;
   }

   /**
    * Determines if the consumer has anything to consume or needs to wait
    *
    * @return true if there are still elements to consume false otherwise
    */
   bool can_consume()
   {
      return elem_queue.size() > 0;
   }

   std::size_t defined_maximum_;

   container elem_queue;
   std::mutex mtx_container;
   std::condition_variable cv_produce;
   std::condition_variable cv_consume;
};

DATA_STRUCTURES_END
THREADING_END








#endif // _thread_safe_queue_hpp_
