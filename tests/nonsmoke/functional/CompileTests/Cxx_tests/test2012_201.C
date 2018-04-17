#if 0
typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
 // __extension__ union
    union
    {
      int __spins_B;
    };
  } __data;
} pthread_mutex_t;
#endif


struct __pthread_mutex_s
  {
 // This union is not output.
    union
       {
         int __var1;
         int __var2;
       };
  } __data;
