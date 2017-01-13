struct A
   {
     int xyz;
   };

typedef union 
   {
     struct A* __sockaddr__;
   } __SOCKADDR_ARG 
#if 1
// This is required to process through EDG and GNU (backend).
__attribute__ ((__transparent_union__))
#endif
     ;

extern int getsockname (int __fd, __SOCKADDR_ARG __addr);

struct B
   {
     int fd;

     struct A* sockaddr;
   };



void foobar()
   {
     unsigned int i;
     struct B ls;

  // getsockname(ls[i].fd,.__sockaddr__ = ls[i] . sockaddr,&ls[i] . socklen)
  // getsockname(ls[i].fd,ls[i].sockaddr,&ls[i].socklen);

  // Unparsed as:   getsockname(ls[i] . fd,.__sockaddr__ = ls[i] . sockaddr);
  // We might think that we want:   getsockname(ls[i] . fd,{.__sockaddr__ = ls[i] . sockaddr});
  // But I think that we can't unparse the initializer as a designated initializer in a function argument list.
     getsockname(ls.fd,ls.sockaddr);
   }





