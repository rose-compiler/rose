#include "addressTakenAnalysis.h"
#include "defUseQuery.h"

/*************************************************************
 * Copyright: (C) 2013 by Sriram Aananthakrishnan            *
 * Author   : Sriram Aananthakrishnan                        *
 * email    : aananthakris1@llnl.gov                         *
 *************************************************************/

class TestDefUseMemObjInfoTraversal : public AstSimpleProcessing
{
  FlowInsensitivePointerInfo& fipi;
  VariableIdMapping& vidm;
public:
  TestDefUseMemObjInfoTraversal(FlowInsensitivePointerInfo& _fipi, VariableIdMapping& _vidm) 
    : fipi(_fipi), vidm(_vidm)  { }
  void visit(SgNode*);
};

void TestDefUseMemObjInfoTraversal::visit(SgNode* sgn)
{
  if(isSgExpression(sgn))
  {
    DefUseMemObjInfo memobj = getDefUseMemObjInfo(sgn, fipi);
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

  FlowInsensitivePointerInfo fipi(project, vidm);
  fipi.collectInfo();
  //fipi.printAnalysisSets();

  TestDefUseMemObjInfoTraversal tt(fipi, vidm);
  tt.traverseInputFiles(project, preorder);

  return 0;
}
