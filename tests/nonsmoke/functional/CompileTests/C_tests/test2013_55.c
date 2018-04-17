
typedef int __pid_t;


int handle_sigchld()
   {
     int status;
     int pid;
     char signame[32];

     if (((union {__pid_t __in;int __i;}){.__in = status} . __i & 0xff) == 0x7f) 
        {
          if (((union {__pid_t __in;int __i;}){status} . __i & 0xff00) >> 8 == -1) 
             {
               signame[0] = 'x';
             }

          signame[0] = 'x';
        }

     signame[0] = 'x';

   }

