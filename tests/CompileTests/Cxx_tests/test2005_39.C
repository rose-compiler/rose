#if 0
// Original Code:
//      FaceMap.erase(insertion_result.first);
// Generated Code:
//      FaceMap.erase(std::_Rb_tree_iterator < Geometry::value_type , Geometry::reference , Geometry::pointer > (insertion_result.first));

// Problem:  Geometry::value_type is undeclared in the namespace Geometry.
//           Since this is part of an implicit cast it should not have been put out!
#endif

#if 0
#include<string>
// This code demonstrates the extrodinarily long template argument expansion which it would be nice to avoid in generated code!
std::pair< std::string, bool > stringBoolPair;
#endif

#include<vector>
void foo()
   {
     int size = 2;

  // Note: this is equivalent to std::vector<int> temp3 = std::vector<int>() in the AST.
     std::vector<int > temp3;

  // The optimization fixup (FixupforGnuBackendCompiler) should not go to far in eliminating redundant 
  // copy constuctors and casts (we need to leave one level of such copy constuctors and casts) as the 
  // following example shows.
     temp3 = std::vector<int >(size);
   }

