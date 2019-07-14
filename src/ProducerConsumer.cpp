#include <iostream>
#include <thread>
#include <memory>
#include "ThreadSafeQueue.hpp"


using namespace std;

int main()
{
   using namespace threading::data_structures;
   std::shared_ptr<LockQueue<int> > spQueue(new LockQueue<int>(3));

   std::thread thCons1([spQueue]()
   {
      std::this_thread::sleep_for(std::chrono::seconds(5));
      spQueue->dequeue();
   });

   std::thread thCons2([spQueue]()
   {
      // consume an 9 elements one every 5 secs
      for (std::size_t k = 0; k < 9; ++k)
      {
         std::this_thread::sleep_for(std::chrono::seconds(5));
         spQueue->dequeue();
      }
   });

   std::thread thProducer1([spQueue]()
   {
      spQueue->enqueue(12);
      spQueue->enqueue(13);
      spQueue->enqueue(14);
      spQueue->enqueue(15);
      spQueue->enqueue(16);
      spQueue->enqueue(17);
      spQueue->enqueue(18);
      spQueue->enqueue(19);
   });

   std::thread thProducer2([spQueue]()
   {
      spQueue->enqueue(20);
      spQueue->enqueue(21);
      spQueue->enqueue(22);
   });

   thCons1.join();
   thCons2.join();
   thProducer1.join();
   thProducer2.join();

	return 0;
}
