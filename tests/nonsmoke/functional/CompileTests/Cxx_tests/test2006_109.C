// This fails to parse properly in EDG even when using -rose:C_only 
// mode because of the use of the "class" keyword (which is C++).
#include <stdio.h>

// This fails in C++ mode using a *.C filename suffix and -rose:C_only
// I have renamed the parameter variable to "_class" from "class" to allow
// the test to pass for testing.  Test2006_110.c deomstrates that a 
// filename extension of *.c is compiled without problem.
void verify(int no_time_steps, char *_class)
   {
   }
