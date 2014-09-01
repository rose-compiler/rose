
struct process
   {
     int status;
   };

extern int kill (int __pid, int __sig);

static int
handle_sub(int job, int fg)
   {
     if (0)
        {
          struct process *p;

#if 0
       /* No support for GNU extensions under Windows */
          for (;;)
               if (0)
                  {
#if 1
                    if (0)
                         kill(0, (((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0x7f));
#if 1
                      else
                         kill(0, (((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0x7f));
#endif
#endif
                    kill(0, (((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0x7f));
                    break;
                  }
#endif
        }

     return 0;
   }

