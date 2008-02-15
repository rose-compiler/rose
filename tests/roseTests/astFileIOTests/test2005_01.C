// The first bug of the new year! (from Vera)

// If DEMONSTRATE_BUG is true, then g++ will compile the code but
// ROSE will not (due to some header file problem).  If DEMONSTRATE_BUG
// is false, then both g++ and ROSE will compile the program correctly.

#define DEMONSTRATE_BUG 1

#if DEMONSTRATE_BUG
#include <iostream>
#else
#include <stdio.h>
#include <stdlib.h>
#endif

// This does not appear to be required
// using namespace std;

int main()
   {
     int a = 42;
     printf("%d",a);
     return 0;
   }
