
#include "rose.h"

// Classic Visitor Pattern in ROSE (implemented using the traversal over 
// the elements stored in the memory pools so it has no cycles and visits 
// ALL IR nodes (including all Sg_File_Info, SgSymbols, SgTypes, and the 
// static builtin SgTypes).
class ClassicVisitor : public ROSE_VisitorPattern
   {
     public:
       // Override virtural function defined in base class
          void visit(SgGlobal* globalScope)
             {
               printf ("Found the SgGlobal IR node \n");
             }

          void visit(SgFunctionDeclaration* functionDeclaration)
             {
               printf ("Found a SgFunctionDeclaration IR node \n");
             }
          void visit(SgTypeInt* intType)
             {
               printf ("Found a SgTypeInt IR node \n");
             }

          void visit(SgTypeDouble* doubleType)
             {
               printf ("Found a SgTypeDouble IR node \n");
             }
   };


int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Classic visitor pattern over the memory pool of IR nodes
     ClassicVisitor visitor_A;
     traverseMemoryPoolVisitorPattern(visitor_A);

     return backend(project);
   }
