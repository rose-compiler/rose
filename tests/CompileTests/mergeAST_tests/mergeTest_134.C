#include <pthread.h>
#include <unistd.h>

typedef pthread_key_t __gthread_key_t;
typedef pthread_once_t __gthread_once_t;
typedef pthread_mutex_t __gthread_mutex_t;
typedef pthread_mutex_t __gthread_recursive_mutex_t;

# define __gthrw(name) \
  extern __typeof(name) __gthrw_ ## name __attribute__ ((__weakref__(#name)))

__gthrw(pthread_once);
__gthrw(pthread_key_create);
__gthrw(pthread_key_delete);
__gthrw(pthread_getspecific);
__gthrw(pthread_setspecific);
__gthrw(pthread_create);
