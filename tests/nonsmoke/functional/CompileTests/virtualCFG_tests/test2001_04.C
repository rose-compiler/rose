// This test code is representative of autoconf tests 
// the use of #line 42 "filename" was a problem for rose preprocessors
// because EDG sets the internal filename pointers to NULL in this case.

// Because the line numbers will be wrong the unparsed file will not have 
// code and comments in the correct order!
// #line 1680 "configure"
#line 8 "configure"

#include <confdefs.h>

bool b = true;
         
int main() {
/* empty */
; 
return 0; }

