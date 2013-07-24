typedef long unsigned int size_t;

typedef unsigned char __u_char;
typedef __u_char u_char;

typedef unsigned long int uintptr_t;
typedef uintptr_t ngx_uint_t;

typedef struct {
    size_t len;
    u_char *data;
} ngx_str_t;

typedef struct {
    ngx_str_t name;
    ngx_uint_t value;
} ngx_conf_enum_t;


// Should unparse as: static ngx_conf_enum_t ngx_debug_points[] = { { { sizeof("stop") - 1, (u_char *) "stop" }, 1 }, { { sizeof("abort") - 1, (u_char *) "abort" }, 2 }, { { 0, ((void *)0) }, 0 } };
static ngx_conf_enum_t ngx_debug_points[] = {
    { { sizeof("stop") - 1, (u_char *) "stop" }, 1 },
    { { sizeof("abort") - 1, (u_char *) "abort" }, 2 },
    { { 0, ((void *)0) }, 0 }
};
