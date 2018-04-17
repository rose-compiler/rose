int verbose;

void Debug_printInfo(int);

#define DEBUG_MACRO( verbose, where ) if ( verbose > 10 ) Debug_printInfo(where); else Debug_printInfo(where+1)

void foobar()
   {
     DEBUG_MACRO(verbose,42);
   }
