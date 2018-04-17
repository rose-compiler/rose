
#include "test2014_07.h"

// DQ (8/28/2015): Modified this to detect backend use (required for ROSE when compiled using the Intel compiler).
// DQ (1/3/2014): ROSE specific code added to fix bug (it is difficult to 
// automatically add this in ROSE, but it could be supported later).  
// #if defined(USE_ROSE) && !defined(__EDG__)
#if defined(USE_ROSE_BACKEND)
  #undef timeout
#endif

char* send_mail_command(const char *fmt, const char *param)
   {
     char *msg;

     if (timeout)
        {
        }
   }
