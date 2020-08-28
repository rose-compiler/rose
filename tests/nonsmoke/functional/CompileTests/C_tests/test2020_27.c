// RC-70:

// From: /usr/include/bits/siginfo.h +62
typedef struct 
   {
     union 
        {
          struct 
             {
               int si_pid;
             } _kill;
        } _sifields;
   } siginfo_t;

// From: /usr/include/bits/siginfo.h +131
#define si_pid _sifields._kill.si_pid

#include "rc-70-2.h"

void signals_handler(siginfo_t * si) 
   {
 // "si_pid" appears to be a recursive macro (might have to implement option to  
 // generate *.i and *.ii intermediate files to avoid re-expansion of macros).
     si->si_pid;
   }

