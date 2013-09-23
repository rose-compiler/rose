// Note that this is an expected failing test on at least GNU 4.4 and above.

// This is a slightly different version of test2013_261.C
// were we use "iostream.h" instead of "iostream"
// and "cout" instead of "std::cout".

#line 1544 "configure"
#include "confdefs.h"
#include <iostream.h>
int main() {
   cout << "hello, world!";
; return 0; }

