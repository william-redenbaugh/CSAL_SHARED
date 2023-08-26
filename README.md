# CSAL_SHARED
Shared libraries that use the HALs that I've written across multiple platforms

## List of Components and their functions

#### Event Management
- Labed as ```event_management.cpp/.h```
- Will handle event management consumption and production between different threads. 
- You can make a global event publish with some data, and have it get consumed by a bunch of different threads. 

#### Local Eventqueue 
- Labed as ```local_eventqueue.cpp/.h```
- Multiple producer, single consumer queue for threads to consume events. Utilizes a lot of the same code as the Event Management module, but instead of sending it to a bunch of different consumers this is only sent to a single consumer 

#### Low Priority Workqueue
- Labeled as ```lp_workqueue.h/.cpp```
- A place to throw a bunch of low priority, semi-nonblocking functions into a queue to get executed in a reasonable amount of time... Those function can and usually are perioic. 

####  CLI interface
- Labeled as ```os_cli.cpp/.h```
- Module that will generate a thread to handle all CLI operations, currently only supports UART interfaces, but end goal is to make it agnostic to interfaces. 

#### Error Handling
- Labeled as ```os_error.h``` 
- Not exactly an entire module as per say, but really just holds a bunch of error codes that the system may encounter, allows functions to pass some level of shared enumerated values for all these codes 

#### Mutex
- Labeled as ```os_mutx.h```
- Shared Mutex wrapper that underlying operating systems can use to spawn and utilize their own mutexes

#### Queues
- Labeled as ```os_queue.h/os_queue.cpp```
- Shared Queue wrapper that underlying operating systems can use to spawn and utilize their own queues

#### Semaphore
- Labeled as ```os_semh.h```
- Shared semaphore wrapper that underlying operating systems can use to spawn and utilize their own semaphores

#### Signaling/Setbits
- Labeled as ```os_setbits.h```
- Shared Signaling wrapper that underlying operating systems can use to spawn and utilize their own signaling mechanisms

#### Basic Quick and dirty FFT transformations
- Labled as ```os_quick_fft.cpp```
- FFT inplace processing of 2^N samples of data. 

#### Status
- Labeled as ```os_status.h```
- Just a bunch of status enumeration types for handling status messages. 

#### Timer
- Labeled as ```os_timer..h```
- Shared timer wrapper that underlying operating systems and hardware timers can use to spawn and signal their own timer mechanisms 

#### Threadsafe Circular Buffer
- Labeled as ```save_circular_queue.cpp/.h```
- Simple variable node size circular buffer that is threadsafe, can essentially be used as a queue. 
- Designed with 32bit, or whatever n bits the MCU architecture alignment

#### Threadsafe Circular Buffer
- Labeled as ```save_circular_queue.cpp/.h```
- Simple variable node size fifo that is threadsafe, can essentially be used as a queue. 
- Designed with 32bit, or whatever n bits the MCU architecture alignment

#### Statemachine framework
- Labeled as ```statemachine.cpp/.h```
- Statemachine framework that makes for easy software based statemachines. Makes keeping state of the system a lot easier heh

#### Thread initialization 
- Labaled as ```threads_init.cpp/.h```
- Ideally we want all the initialzed threads in our system to trace back to this module, in embedded systems it's not like we want to spawn and delete threads that often(memory fragmentation on small embeded systems is a painful debugging experience)

#### Unit Testing Framework
- Labeled as ```unit_check.cpp/.h```
- Unit testing framework that allows us to build and run tests on all the other modules, along with other code in the system