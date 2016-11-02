
int index;
int length;
bool debug;
bool debug_active;
int verbose;

void Debug_printInfo(int,int,int, char*);

#define DEBUG_MACRO( verbose, where ) \
  if ( debug_active && (verbose > 10 ))  \
    Debug_printInfo( debug, index, length, where)


void foobar()
   {
     int x;
     x = 7;
     DEBUG_MACRO(verbose, "This is getting old");
     x = 42;
   }
