/* Example C code demonstrating operations using precedence
   and assertion tests to check for correct answers.
*/

#include "assert.h"

typedef _Bool bool;
#define true 1
#define false 0

int main()
   {
     int x1 = 1 + 2 * 3;
     assert(x1 == 7);

     int x2 = 2 * 3 + 1;
     assert(x2 == 7);

     int x3 = (1 + 2) * 3;
     assert(x3 == 9);

     int x4 = 2 * (3 + 1);
     assert(x4 == 8);

     bool b1 = true | false;
     assert(b1 == true);

     bool b2 = !(true | false);
     assert(b2 == false);

     bool b3 = !false | false;
     assert(b3 == true);

     bool b4 = !!!false | false;
     assert(b4 == true);

     bool b5a = true;
     bool b5 = !!!--b5a | false;
     assert(b5 == true);

     return 1;
   }


