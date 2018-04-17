#define ATTR_NORETURN __attribute__((noreturn))

// typedef struct failure_callback_t {} failure_callback_t;
typedef void failure_callback_t(int x);

// The use of struct causes this to be a structure and confuses ROSE.
// struct failure_callback_t*  fatal_handler = 0L;
// failure_callback_t*  fatal_handler = 0L;

void i_set_fatal_handler(failure_callback_t *callback ATTR_NORETURN);

