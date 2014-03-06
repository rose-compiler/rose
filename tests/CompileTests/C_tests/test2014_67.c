
// Code such as this:
//      first = 0xD800 | ((codepoint & 0xffc00) >> 10);
// will be unparsed as:
//      first = 0xD800 | (codepoint & 0xffc00) >> 10;
// Is this correct?
// 
// From the oprator precedence tables: the shift operator ">>" has a 
// HIGHER precedence than the bitwise OR operator "|".  So the generated
// code by ROSE is correct.

// Similarly the bitwise AND operator "&" has higher precedence then 
// the bitwise OR operator "|".  However, some versions of gcc will
// issue the warning:
//    warning: suggest parentheses around arithmetic in operand of |
// and if this is combined with -Werror the compilation will fail.

#include <stdlib.h>
#include <stdio.h>

void foobar()
   {
      unsigned long codepoint = 0xffc00;
      unsigned long first = 0;
      unsigned long last = 0;

   // This will be unparsed as:
   // first = 0xD800 | (codepoint & 0xffc00) >> 10;
      first = 0xD800 | ((codepoint & 0xffc00) >> 10);

   // This will be unparsed as:
   // last = 0xDC00 | codepoint & 0x003ff;
      last = 0xDC00 | (codepoint & 0x003ff);

      printf ("first = %lu \n",first);
   }

