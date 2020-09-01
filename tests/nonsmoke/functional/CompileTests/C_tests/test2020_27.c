// RC-70:


// Since the macro is defined in the header file, we can't detect it unless
// we collected all of the #defines from all possible header files, which is 
// prohibatively expensive.  An alternative would be to support an option to
// generate a *.i file (or a *.ii file, for C++) which would put all of the
// include files into the generated file (unparsing the full translation unit),
// and as a result supress the expansion of all macros.


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

