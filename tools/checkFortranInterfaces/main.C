// This program checks Fortran and C files and declares if they
// have matching interfaces based on functions arguments.
//

#include "checkFortranInterfaces.h"


int main (int argc, char* argv[])
{
  // Build the abstract syntax tree
   SgProject* project = frontend(argc,argv);
   ROSE_ASSERT (project != NULL);

  // Build the inherited attribute
   InheritedAttribute inheritedAttribute;

  // Define the traversal
   Traversal astTraversal;

  // Call the traversal starting at the project (root) node of the AST
   astTraversal.traverseInputFiles(project,inheritedAttribute);

   std::cout << "\n" << std::endl;

   mapType map_f = astTraversal.getFortranMap();
   mapType map_c_or_cxx = astTraversal.getCMap();

   InterfaceChecker check;
   if (check.check_interface(map_f, map_c_or_cxx)) {
      std::cout << "END OF PROGRAM: map's functions match" << std::endl;
   }
   else {
      std::cout << "END OF PROGRAM: map's functions do not match" << std::endl;
   }

   return 0;
}
