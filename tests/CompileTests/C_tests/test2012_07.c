typedef long unsigned int size_t;
typedef unsigned char __u_char;
typedef __u_char u_char;
typedef struct ngx_log_s ngx_log_t;

typedef u_char *(*ngx_log_handler_pt) (ngx_log_t *log, u_char *buf, size_t len);

struct ngx_log_s {
    ngx_log_handler_pt handler;
    void *data;
    char *action;
};


void ngx_log_error_core(ngx_log_t *log)
   {
     u_char *p, *last;
     p = log->handler(log, p, last - p);
   }
