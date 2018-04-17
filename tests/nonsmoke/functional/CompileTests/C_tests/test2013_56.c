
typedef int __pid_t;


int handle_sigchld()
   {
     int status;
     int pid;
     char signame[32];

//   if (((union {__pid_t __in;int __i;}){.__in = status} . __i & 0xff) == 0x7f) 
        {
       // DQ (9/5/2013): It is a problem when the statement expression is in a function paramter.
       // while (sig2str(((union {__pid_t __in;int __i;}){status} . __i & 0xff00) >> 8,signame) == -1)
          while (sig2str(((union {__pid_t __in;int __i;}){status} . __i & 0xff00) >> 8,signame) == -1)
             {
               signame[0] = 'x';
             }
          signame[0] = 'x';
        }

     signame[0] = 'x';

   }

