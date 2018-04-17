
// This example demonstrates that the default behavior of the Intel compiler is C++11.
// And that the default behavior of g++ version 4.8.3 is NOT C++11.

#define __GTHREAD_MUTEX_INIT 42;

typedef int __gthread_mutex_t;

class __mutex 
   {
     private:
#if 1
          __gthread_mutex_t _M_mutex = __GTHREAD_MUTEX_INIT;
#else
          __gthread_mutex_t _M_mutex;
#endif
  };

