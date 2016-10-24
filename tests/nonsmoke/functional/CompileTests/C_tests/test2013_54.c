

int handle_sigchld()
   {
     int status;
     int xyz;

  // while (((union {int __in;int __i;}){status} . __i & 0xff00) >> 8 == -1) 
     while (((union {int __in;int __i;}){status} . __i) == -1)
        {
          xyz = 7;
        }
   }

