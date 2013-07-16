#include "addressTakenAnalysis.h"
#include "defUseQuery.h"

class TestDefUseMemObjTraversal : public AstSimpleProcessing
{
  FlowInsensitivePointerAnalysis& fipa;
  VariableIdMapping& vidm;
public:
  TestDefUseMemObjTraversal(FlowInsensitivePointerAnalysis& _fipa, VariableIdMapping& _vidm) 
    : fipa(_fipa), vidm(_vidm)  { }
  void visit(SgNode*);
};

void TestDefUseMemObjTraversal::visit(SgNode* sgn)
{
  if(isSgExpression(sgn))
  {
    DefUseMemObj memobj = getDefUseMemObj(sgn, fipa);
    if(!memobj.isDefSetEmpty() || !memobj.isUseSetEmpty())
    {
      std::cout << "<" << sgn->class_name() << ", " << sgn->unparseToString() << "\n" 
                << memobj.str(vidm) << ">\n";
    }
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
  VariableIdMapping vidm;
  vidm.computeVariableSymbolMapping(project);

  FlowInsensitivePointerAnalysis fipa(project, vidm);
  fipa.runAnalysis();
  //fipa.printAnalysisSets();

  TestDefUseMemObjTraversal tt(fipa, vidm);
  tt.traverseInputFiles(project, preorder);

  return 0;
}
