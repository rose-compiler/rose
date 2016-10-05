#if 1
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


enum {
	LSA_LEN_SIZE = 0, // offsetof(len_and_sockaddr, u),
	LSA_SIZEOF_SA = sizeof(
		union {
      // struct sockaddr sa;
      // struct sockaddr_in sin;
#if ENABLE_FEATURE_IPV6
      // struct sockaddr_in6 sin6;
#endif
		}
	)
};
