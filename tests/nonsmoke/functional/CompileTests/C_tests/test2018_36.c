
typedef int ParseErrorHandler_t(int *xxx, const char * token, char demark, int errorCode);

typedef const char const_char;

static ParseErrorHandler_t parseErrorHandler;

// static int parseErrorHandler(int *x, const char * token, char demark, int errorCode)
static int parseErrorHandler(int *x, const_char* token, char demark, int errorCode)
   {
     return errorCode;
   }
