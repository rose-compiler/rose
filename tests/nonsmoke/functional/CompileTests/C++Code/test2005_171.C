/* Bug submitted by Andreas (10/14/2005)
   (caused error in 
 */

#include <iostream.h>

class X
   {
     public:
          istream & input;
   };

void check_Is_cin ( X & in )
   {
     if (&(in.input) == &(cin));
   }

int main()
   {
     return 0;
   }

