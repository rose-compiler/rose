
typedef enum {
 http_get = 1,
 http_head = 1LL << 2,
} cherokee_http_method_t;

typedef struct
   {
     int x;
     cherokee_http_method_t valid_methods;
   } cherokee_plugin_info_handler_t;

cherokee_plugin_info_handler_t cherokee_file_info = { { 42 }, http_get | http_head };


