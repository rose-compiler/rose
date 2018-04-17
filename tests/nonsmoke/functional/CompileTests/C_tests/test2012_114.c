
extern int kill (int x, char* s);

void handle_sub(int job, int fg)
   {
     int status;

  // The handling of the first function arguement leaves an already processed body (sse) in the source sequence list and causes a problem in the processing of the second argument.
  // The solution is to look for these "already processed body (sse) in the source sequence list" and removed them ahead of processing that would be confused by them.
  // kill((((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0x7f), (((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0x80) ? " (core dumped)" : "");
     kill( 0, (((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0x80) ? " (core dumped)" : "");
   }

