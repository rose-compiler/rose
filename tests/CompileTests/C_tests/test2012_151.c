// Source code position tests.

typedef struct ngx_listening_s ngx_listening_t;

struct sockaddr
  {
//  sa_family_t sa_family;
    char sa_data[14];
  };


typedef int socklen_t;


struct ngx_listening_s {
#if 0
    ngx_socket_t fd;

    struct sockaddr *sockaddr;
    socklen_t socklen;
    size_t addr_text_max_len;
    ngx_str_t addr_text;

    int type;

    int backlog;
    int rcvbuf;
    int sndbuf;

    int keepidle;
    int keepintvl;
    int keepcnt;



    ngx_connection_handler_pt handler;

    void *servers;

    ngx_log_t log;
    ngx_log_t *logp;

    size_t pool_size;

    size_t post_accept_buffer_size;

    ngx_msec_t post_accept_timeout;

    ngx_listening_t *previous;
    ngx_connection_t *connection;

    unsigned open:1;
    unsigned remain:1;
    unsigned ignore:1;

    unsigned bound:1;
    unsigned inherited:1;
    unsigned nonblocking_accept:1;
    unsigned listen:1;
    unsigned nonblocking:1;
    unsigned shared:1;
    unsigned addr_ntop:1;




    unsigned keepalive:2;


    unsigned deferred_accept:1;
    unsigned delete_deferred:1;
    unsigned add_deferred:1;
// # 83 "src/core/ngx_connection.h"
#else
   // int* sockaddr;
      struct sockaddr *sockaddr;
   // int socklen;
      socklen_t socklen;
#endif
};


#if 1
typedef union { __const struct sockaddr *__restrict __sockaddr__; __const struct sockaddr_at *__restrict __sockaddr_at__; __const struct sockaddr_ax25 *__restrict __sockaddr_ax25__; __const struct sockaddr_dl *__restrict __sockaddr_dl__; __const struct sockaddr_eon *__restrict __sockaddr_eon__; __const struct sockaddr_in *__restrict __sockaddr_in__; __const struct sockaddr_in6 *__restrict __sockaddr_in6__; __const struct sockaddr_inarp *__restrict __sockaddr_inarp__; __const struct sockaddr_ipx *__restrict __sockaddr_ipx__; __const struct sockaddr_iso *__restrict __sockaddr_iso__; __const struct sockaddr_ns *__restrict __sockaddr_ns__; __const struct sockaddr_un *__restrict __sockaddr_un__; __const struct sockaddr_x25 *__restrict __sockaddr_x25__;
       } __CONST_SOCKADDR_ARG __attribute__ ((__transparent_union__));
#else
typedef union {  struct sockaddr * __sockaddr__; int x; } __CONST_SOCKADDR_ARG __attribute__ ((__transparent_union__));
#endif

// extern int bind (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len) __attribute__ ((__nothrow__));
// extern int bind (int __fd, int* __addr, int __len) __attribute__ ((__nothrow__));
extern int bind (int __fd, __CONST_SOCKADDR_ARG __addr, int __len) __attribute__ ((__nothrow__));

void foobar()
   {
     int s;
     int i;
     ngx_listening_t *ls;

  // if (bind(s, ls[i].sockaddr, ls[i].socklen) == -1) 
     if (bind(s, ls[i].sockaddr, 42) == -1) 
        {
        }
   }
