
#if 1
// Note that this must be marked as __transparent_union__
typedef union 
   {
  // struct sockaddr *__restrict __sockaddr__; 
     int *__restrict __sockaddr__; 
   } __SOCKADDR_ARG __attribute__ ((__transparent_union__));
#else
typedef union 
   {
     struct sockaddr *__restrict __sockaddr__; 
   } __SOCKADDR_ARG;
#endif

extern int getsockname (__SOCKADDR_ARG __addr);

int
ngx_set_inherited_sockets()
   {
  // struct sockaddr *sockaddr;
     int  *sockaddr;

  // Error: unparses as: "getsockname({.__sockaddr__ = (sockaddr)});"
  // But should be" "getsockname(sockaddr);"
     getsockname(sockaddr);

     return 0;
   }

