typedef int __pthread_list_t;
typedef int __pthread_slist_t;

typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;
    unsigned int __nusers_A;
    int __kind;
    int __spins_A;
    __pthread_list_t __list_A;
# define __PTHREAD_MUTEX_HAVE_PREV	1
    unsigned int __nusers_B;
    __extension__ union
    {
      int __spins_B;
      __pthread_slist_t __list_B;
    };
  } __data;
  char __size[42];
  long int __align;
} pthread_mutex_t;
