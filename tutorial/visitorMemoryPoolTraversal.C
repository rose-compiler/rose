
#include "rose.h"

// ROSE Visit Traversal (similar interface as Markus's visit traversal) 
// in ROSE (implemented using the traversal over 
// the elements stored in the memory pools so it has no cycles and visits 
// ALL IR nodes (including all Sg_File_Info, SgSymbols, SgTypes, and the 
// static builtin SgTypes).
class RoseVisitor : public ROSE_VisitTraversal
   {
     public:
          int counter;
          void visit ( SgNode* node);

          RoseVisitor() : counter(0) {}        
   };


void RoseVisitor::visit ( SgNode* node)
   {
  // printf ("roseVisitor::visit: counter %4d node = %s \n",counter,node->class_name().c_str());
     counter++;
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // ROSE visit traversal
     RoseVisitor visitor;
     visitor.traverseMemoryPool();

     printf ("Number of IR nodes in AST = %d \n",visitor.counter);

     return backend(project);
   }
