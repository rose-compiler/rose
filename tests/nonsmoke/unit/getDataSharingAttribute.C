#include "rose.h"
#include <iostream>
#include <string>
#include <vector>
#include "OmpAttribute.h"
#include "omp_lowering.h" //TODO: put all OpenMP stuff into one header OmpSupport.h ?
using namespace std;
using namespace OmpSupport;
using namespace SageInterface;

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* node)
{
  if (SgLocatedNode* lnode = isSgLocatedNode(node))
  {
    //skip system headers
    if (insideSystemHeader (lnode))
      return; 

    if (SgForStatement* forloop= isSgForStatement(node))
    {
      std::vector< SgVarRefExp * > ref_vec; 
      collectVarRefs (forloop, ref_vec);
      for (std::vector< SgVarRefExp * >::iterator iter = ref_vec.begin(); iter!= ref_vec.end(); iter ++) 
      {
        SgSymbol* s = (*iter)->get_symbol();
        omp_construct_enum atr = getDataSharingAttribute (s, forloop);
        cout<<s->get_name()<<"\t"<<toString(atr) <<endl; 
      }
    }
  }
}

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);

  return backend(project);
}

