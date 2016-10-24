#if 0
#include <sys/socket.h>


struct sockaddr_in {};


typedef struct len_and_sockaddr {
	socklen_t len;
	union {
		struct sockaddr sa;
		struct sockaddr_in sin;
#if ENABLE_FEATURE_IPV6
		struct sockaddr_in6 sin6;
#endif
	} u;
} len_and_sockaddr;
#endif


enum XXXYYYZZZ 
   {
     LSA_LEN_SIZE = 0, // offsetof(len_and_sockaddr, u),
     LSA_SIZEOF_SA = sizeof(
          union
             {
               int x;
               long y;
             } ),
     LAST_ENUM = 42
   } XXX;


void foo()
   {
     enum XXXYYYZZZ value = LAST_ENUM;
   }
