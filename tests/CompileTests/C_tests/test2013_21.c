
struct sockaddr
   {
     int xyz;
   };

typedef union 
   { 
     struct sockaddr *__restrict __sockaddr__;
   } __SOCKADDR_ARG __attribute__ ((__transparent_union__));

// extern int getsockname (int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __len) __attribute__ ((__nothrow__));
extern int getsockname (int __fd, __SOCKADDR_ARG __addr) __attribute__ ((__nothrow__));

typedef struct ngx_listening_s ngx_listening_t;

struct ngx_listening_s 
   {
  // ngx_socket_t fd;
     int fd;

     struct sockaddr *sockaddr;

  // socklen_t socklen;
     int socklen;
   };



void foobar()
   {
     unsigned int i;
     ngx_listening_t *ls;

  // getsockname(ls[i].fd,.__sockaddr__ = ls[i] . sockaddr,&ls[i] . socklen)
  // getsockname(ls[i].fd,ls[i].sockaddr,&ls[i].socklen);

  // Unparsed as:   getsockname(ls[i] . fd,.__sockaddr__ = ls[i] . sockaddr);
  // We might think that we want:   getsockname(ls[i] . fd,{.__sockaddr__ = ls[i] . sockaddr});
  // But I think that we can't unparse the initializer as a designated initializer in a function argument list.
     getsockname(ls[i].fd,ls[i].sockaddr);
   }





