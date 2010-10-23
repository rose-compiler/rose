/*
 * Parsing OpenMP pragma text
by Liao, 9/17/2008
Last Modified: 9/19/2008
*/
#include "rose.h"
#include <iostream>
#include <string>
#include "OmpAttribute.h"
using namespace std;
using namespace OmpSupport;

class visitorTraversal : public AstSimpleProcessing
{
  protected: 
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* node)
{
  // Some OmpAttribute may be attached redundantly,
  // e.g., omp for is attached to both the pragma and the loop. 
  OmpAttributeList* attributelist = getOmpAttributeList(node);
  if (attributelist)
  {
    cout<<"Found att attached to "<<node<<" "<<node->class_name()<<" at line "
      <<node->get_file_info()->get_line()<<endl;
    attributelist->print();//debug only for now
  }
}

// must have argc and argv here!!
int main(int argc, char * argv[])
{
// DQ (4/4/2010): Note that althought we don't yet support OpenMP for Fortran,
// This function will operate on Fortran code and do some transformations
// For example, specification of private variables will cause new variables to
// be added to the curent scope (or at least searched for).  I am not clear if 
// this mutable operation on the Fortran AST is intended.  For C/C++ OpenMP is 
// of course supported.

  SgProject *project = frontend (argc, argv);

  AstTests::runAllTests(project);

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (turn off output of dot files before committing code) \n");
     generateDOT ( *project );
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);
  
  return backend(project);
}
