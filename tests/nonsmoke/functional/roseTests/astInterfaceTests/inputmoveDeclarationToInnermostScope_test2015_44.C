int verbose;

void Debug_printInfo(int);

#define DEBUG_MACRO( verbose, where ) if ( verbose > 10 ) Debug_printInfo(where)

void foobar()
   {
     DEBUG_MACRO(verbose,42);
   }
