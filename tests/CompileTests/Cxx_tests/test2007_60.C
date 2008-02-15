#include "iostream"

void f()
   {
  // DQ (6/7/2007): Error reported by Gergo.
  // Error: the file info for the SgVarRefExp is marked as being 
  // from the location of the declaration of the cout function!
  // Same for "std::endl".
     std::cout << "foo" << std::endl;
   }
