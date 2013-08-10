#include "sage3basic.h"

#include "addressTakenAnalysis.h"
#include "defUseQuery.h"
#include "Timer.h"
#include "AnalysisAbstractionLayer.h"


/*************************************************************
 * Copyright: (C) 2013 by Sriram Aananthakrishnan            *
 * Author   : Sriram Aananthakrishnan                        *
 * email    : aananthakris1@llnl.gov                         *
 *************************************************************/

using namespace CodeThorn;
using namespace AnalysisAbstractionLayer;

class TestDefUseVarsInfoTraversal : public AstSimpleProcessing
{
  VariableIdMapping& vidm;
  long flagRaisedDefSet;
  long flagRaisedUseSet;
  long n_expr;
public:
  TestDefUseVarsInfoTraversal(VariableIdMapping& _vidm) 
  : vidm(_vidm), flagRaisedDefSet(0), flagRaisedUseSet(0), n_expr(0) { }
  void visit(SgNode*);
  void atTraversalEnd();
};

void TestDefUseVarsInfoTraversal::visit(SgNode* sgn)
{
  if(isSgExpression(sgn) || isSgInitializedName(sgn))
  {
    n_expr++;
    DefUseVarsInfo memobj = getDefUseVarsInfo(sgn, vidm);
    if(memobj.isDefSetModByPointer()) {
      // std::cout << "def_set flag raised\n";
      ++flagRaisedDefSet;
    }
    if(memobj.isUseSetModByPointer()) {
      // std::cout << "use_set flag raised\n";
      ++flagRaisedUseSet;
    }
#if 0
    if(!memobj.isDefSetEmpty() || !memobj.isUseSetEmpty() || !memobj.isFunctionCallExpSetEmpty())
    {
      std::cout << "<" << sgn->class_name() << ", " << sgn->unparseToString() << "\n" 
                << memobj.str(vidm) << ">\n";
    }
#endif
  }
}

void TestDefUseVarsInfoTraversal::atTraversalEnd()
{
  std::cout << "DefSetModByPtr: " << flagRaisedDefSet << "\n";
  std::cout << "UseSetModByPtr: " << flagRaisedUseSet << "\n";
  std::cout << "n_expr: " << n_expr << "\n";
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

  Timer timer;

  timer.start();

  // compute variableId mappings
  VariableIdMapping vidm;
  vidm.computeVariableSymbolMapping(project);

  // collect all the variables that are used in functions in
  // the code we are analyzing
  // collect type information only about these variables
  VariableIdSet usedVarsInProgram = usedVariablesInsideFunctions(project, &vidm);

  FlowInsensitivePointerInfo fipi(project, vidm, usedVarsInProgram);
  fipi.collectInfo();
  fipi.printInfoSets();

  timer.stop();
  double fipaMeasuredTime=timer.getElapsedTimeInMilliSec();

  TestDefUseVarsInfoTraversal tt(vidm);
  // change to traverse for entire project
  timer.start();
  tt.traverse(project, preorder);
  timer.stop();

  double duMeasuredTime = timer.getElapsedTimeInMilliSec();

  std::cout << "fipa : " << fipaMeasuredTime << "\n";
  std::cout << "du : " << duMeasuredTime << "\n";

  return 0;
}
