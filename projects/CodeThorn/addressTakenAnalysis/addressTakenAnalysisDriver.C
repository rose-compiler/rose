#include "addressTakenAnalysis.h"
#include "defUseQuery.h"

class TestDefUseMemObjTraversal : public AstSimpleProcessing
{
  FlowInsensitivePointerAnalysis& fipa;
public:
  TestDefUseMemObjTraversal(FlowInsensitivePointerAnalysis& _fipa) : fipa(_fipa) { }
  void visit(SgNode*);
};

void TestDefUseMemObjTraversal::visit(SgNode* sgn)
{
  if(isSgExpression(sgn))
  {
    MemObj memobj = getDefMemObj(sgn, fipa);
  }
}


/*************************************************
 ******************* main ************************
 *************************************************/
int main(int argc, char* argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  SgNode* root = project;

  RoseAst ast(root);

  // compute variableId mappings
  FlowInsensitivePointerAnalysis fipa(project);
  fipa.runAnalysis();
  //fipa.printAnalysisSets();

  return 0;
}
