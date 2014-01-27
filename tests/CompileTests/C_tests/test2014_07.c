
#include "test2014_07.h"

// DQ (1/3/2014): ROSE specific code added to fix bug (it is difficult to 
// automatically add this in ROSE, but it could be supported later).  
#if defined(USE_ROSE) && !defined(__EDG__)
  #undef timeout
#endif

char* send_mail_command(const char *fmt, const char *param)
   {
     char *msg;

     if (timeout)
        {
        }
   }
