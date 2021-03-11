
// This code is to support building DOT graphs of the EDG AST ans associated nodes
// so that we can better understand issues in the translation of the EDG representation
// into ROSE.  The modivating problem has been the handling of GNU statement expressions
// where types are declared and extra source sequence points are added to the AST and
// we need to understand this better so that we can skip over them in the translation.

namespace CLANG_ROSE_Graph
   {
  // File for output for generated graph.
  // EXTERN std::ofstream file;
     extern std::ofstream file;

#if 0
  // DQ (10/23/2020): We need to express similar function in terms of Clang IR nodes.
     void graph ( clang::Decl * decl );
     void graph ( clang::Stmt * stmt );
     void graph ( const clang::Type * type );
     void graph ( clang::DeclContext * decl_context );
#endif
   }

