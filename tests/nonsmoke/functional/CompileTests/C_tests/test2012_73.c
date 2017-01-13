typedef int __pid_t;

typedef union
  {
    union wait *__uptr;
 // int *__iptr;
  } __WAIT_STATUS __attribute__ ((__transparent_union__));

union wait
  {
    struct
      {
        unsigned int __w_retcode:8;
      } __wait_terminated;
    struct
      {
        unsigned int __w_stopval:8;
      } __wait_stopped;
  };

extern __pid_t wait (__WAIT_STATUS __stat_loc);

static void
child_cleanup(int signo __attribute__((unused)))
   {
     wait(((void *)0));
   }
