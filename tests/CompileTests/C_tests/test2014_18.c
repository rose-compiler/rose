#define ATTR_NORETURN __attribute__((noreturn))

// typedef struct failure_callback_t 
//    {
//    } failure_callback_t;
// typedef void failure_callback_t(const struct failure_context *ctx,const char *format, va_list args);
typedef void failure_callback_t(int x);

struct failure_callback_t*  fatal_handler = 0L;

void i_set_fatal_handler(failure_callback_t *callback ATTR_NORETURN);

void i_set_fatal_handler(failure_callback_t *callback ATTR_NORETURN)
   {
     fatal_handler = callback;
   }

