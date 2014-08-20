
extern int kill (int x, char* s);

void handle_sub(int job, int fg)
   {
     int status;

     for (;;)
        {
#if 0
       /* No support for GNU extensions under Windows */
          if ((((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0x7f)) 
             {
               if (2)
                    kill((((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0x7f), (((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0x80) ? " (core dumped)" : "");
             }
            else
             {
               if (6)
                    kill(((((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0xff00) >> 8),"");
             }
#endif
          break;
        }
   }

