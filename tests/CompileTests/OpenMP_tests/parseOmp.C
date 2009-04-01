/*
 * Parsing OpenMP pragma text
by Liao, 9/17/2008
Last Modified: 9/19/2008
*/
#include "rose.h"
#include <iostream>
#include <string>
#include "OmpAttribute.h"
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
  OmpAttribute* attribute = getOmpAttribute(node);
  if (attribute)
    attribute->print();//debug only for now
}

// must have argc and argv here!!
int main(int argc, char * argv[])

{
  SgProject *project = frontend (argc, argv);
  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);

  return backend(project);
}
