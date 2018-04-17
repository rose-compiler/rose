
typedef int ParseErrorHandler_t(int *x, const char * token, char demark, int errorCode);

static ParseErrorHandler_t parseErrorHandler;

static int parseErrorHandler(int *x, const char * token, char demark, int errorCode)
   {
     return errorCode;
   }
