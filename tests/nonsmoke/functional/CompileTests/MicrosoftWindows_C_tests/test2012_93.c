
#if 0
       /* No support for GNU statement expressions (GNU extension) (or maybe also un-named unions) under Windows */
          for (pn = jobtab[jn -> other].procs; pn; pn = pn -> next) 
            if (((({
              union {
              long __in;
              int __i;}__u;
              __u.__in = pn -> status;
              __u.__i;
            })) & 0xff) == 0x7f) 
{
            }
          break; 
#endif




void foobar()
   {
      for (;;)
           break;
#if 0
       /* No support for GNU statement expressions (GNU extension) (or maybe also un-named unions) under Windows */
         if (((({ union {
              long __in;
              int __i;}__u;
              __u.__in = 42;
              __u.__i;
                  })) & 0xff) == 0x7f)
         {
            break;
         }
#endif      
   }


