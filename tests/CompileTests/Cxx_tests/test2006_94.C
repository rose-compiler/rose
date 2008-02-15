/*
expTranslator is a simple identity translator. foo.c compiled with g++ should print "positive". When compiled with expTranslator, it prints "nonpositive". The reason is that for unsigned integer constants ("1U") ROSE omits the "U".
(Incidentally, it causes a warning at my environment whenever UINT_MAX is used, because it defined as (INT_MAX * 2U + 1).)

Best regards,
Yarden Nir-Buchbinder
Software Verification and Testing
IBM Haifa Research Lab
Tel. 972-4-8296010
*/

#include <limits.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
   if (INT_MAX + 1U > 0) {
      puts("positive");
   }
   else {
      puts("nonpositive");
   }
   return 0;
}

