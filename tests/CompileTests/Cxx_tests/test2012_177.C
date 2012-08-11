
#include<vector>
void foo()
   {
     int size = 2;

  // Note: this is equivalent to std::vector<int> temp3 = std::vector<int>() in the AST.
     std::vector<int > temp3;

  // The optimization fixup (FixupforGnuBackendCompiler) should not go to far in eliminating redundant 
  // copy constuctors and casts (we need to leave one level of such copy constuctors and casts) as the 
  // following example shows.

  // temp3 = std::vector<int >(size);
   }

