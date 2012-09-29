
// Note that this must be marked as __transparent_union__
#if 0
typedef union
   {
     int *__restrict __sockaddr__; 
   } __SOCKADDR_ARG __attribute__ ((__transparent_union__));
#else
union ZZZ
   {
     int *__restrict __sockaddr__;
   } __attribute__ ((__transparent_union__));

typedef union ZZZ __SOCKADDR_ARG;
#endif

extern int getsockname (__SOCKADDR_ARG __addr);

int
ngx_set_inherited_sockets()
   {
#if 1
  // This will force the cast operation (demonstrated bug).
     int  *functionArg;
#else
  // This will not force a cast (does not demonstrate bug).
     __SOCKADDR_ARG functionArg;
#endif

  // Error: unparses as: "getsockname({.__sockaddr__ = (sockaddr)});"
  // But should be" "getsockname(sockaddr);"
     getsockname(functionArg);

     return 0;
   }

