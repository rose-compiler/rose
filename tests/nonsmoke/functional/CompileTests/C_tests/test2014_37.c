
typedef int CSParse_t;
typedef long StateRet_t;

typedef StateRet_t ParseErrorHandler_t(CSParse_t * pCSParse, const char * token, char demark, StateRet_t errorCode);

static ParseErrorHandler_t parseErrorHandler;


static StateRet_t parseErrorHandler(CSParse_t * pCSParse, const char * token, char demark, StateRet_t errorCode)
   {
     return errorCode;
   }
