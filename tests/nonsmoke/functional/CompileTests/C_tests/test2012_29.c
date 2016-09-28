
typedef enum 
   {
     http_get = 1,
     http_head = 1LL << 2
   } cherokee_http_method_t;


typedef struct 
   {
     cherokee_http_method_t valid_methods;
   } cherokee_plugin_info_handler_t;

// This has to be an expression to trigger the cast (the explicit cast is the error).
cherokee_plugin_info_handler_t cherokee_file_info = { (http_get | http_head) };


