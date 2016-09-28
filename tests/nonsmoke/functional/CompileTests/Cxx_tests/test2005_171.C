/* Bug submitted by Andreas (10/14/2005)
   (caused error in 
 */

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3)

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

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

