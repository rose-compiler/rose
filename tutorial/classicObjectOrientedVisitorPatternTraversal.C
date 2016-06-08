
#include "rose.h"

// This file is a place holder for an object-oriented vistor pattern 
// implemented over the IR nodes that are traversed within a traversal
// of nodes marked to be traversed (instead of over ALL nodes using
// the memory pools as a mechanism).

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

  // Classic visitor pattern
     ClassicVisitor visitor_A;
     traverseMemoryPoolVisitorPattern(visitor_A);

     return backend(project);
   }
