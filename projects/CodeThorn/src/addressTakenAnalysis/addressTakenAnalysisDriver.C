#include "addressTakenAnalysis.h"
#include "defUseQuery.h"

/*************************************************************
 * Copyright: (C) 2013 by Sriram Aananthakrishnan            *
 * Author   : Sriram Aananthakrishnan                        *
 * email    : aananthakris1@llnl.gov                         *
 *************************************************************/

class TestDefUseVarsInfoTraversal : public AstSimpleProcessing
{
  VariableIdMapping& vidm;
public:
  TestDefUseVarsInfoTraversal(VariableIdMapping& _vidm) 
    : vidm(_vidm)  { }
  void visit(SgNode*);
};

void TestDefUseVarsInfoTraversal::visit(SgNode* sgn)
{
  if(isSgExpression(sgn) || isSgInitializedName(sgn))
  {
    DefUseVarsInfo memobj = getDefUseVarsInfo(sgn, vidm);
    if(!memobj.isDefSetEmpty() || !memobj.isUseSetEmpty() || !memobj.isFunctionCallExpSetEmpty())
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

  FlowInsensitivePointerInfo fipi(project, vidm);
  fipi.collectInfo();
  fipi.printInfoSets();

  TestDefUseVarsInfoTraversal tt(vidm);
  tt.traverseInputFiles(project, preorder);

  return 0;
}
