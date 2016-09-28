
typedef union
  {
    union wait *__uptr;
  } __WAIT_STATUS __attribute__ ((__transparent_union__));

union wait
  {
  };

void wait (__WAIT_STATUS __stat_loc);

void
child_cleanup(int signo __attribute__((unused)))
   {
  // The bug is that this was unparsed as: "  wait(.__uptr = (((void *)0)));"
  // The compiler generated designated initializer shuld not have been output 
  // in the generated code (it is of course a part of the AST).
     wait((void *)0);
   }
