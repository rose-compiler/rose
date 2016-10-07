#include "rose.h"
#include "finiteDifferencing.h"

using namespace std;

std::vector<SgExpression*> exprs;

class FindMinusVisitor: public AstSimpleProcessing {
  public:
  virtual void visit(SgNode* n) {
    SgMinusOp* n2 = isSgMinusOp(n);
    if (n2) {
      exprs.push_back(n2->get_operand());
      // cout << "Minus found around " << n2->get_operand()->unparseToString() << endl;
    }
  }
};

std::vector<SgExpression*> bang_exprs;

class FindNotVisitor: public AstSimpleProcessing {
  public:
  virtual void visit(SgNode* n) {
    SgNotOp* n2 = isSgNotOp(n);
    if (n2) {
      bang_exprs.push_back(n2->get_operand());
    }
  }
};

vector<SgFunctionDefinition*> functions;

class FindFunctionsVis: public AstSimpleProcessing {
  public:
  virtual void visit(SgNode* n) {
    if (isSgFunctionDefinition(n)) {
      functions.push_back(isSgFunctionDefinition(n));
    }
  }
};

int main (int argc, char* argv[]) {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
  SgProject* sageProject = frontend(argc,argv);

  moveForDeclaredVariables(sageProject);

  SgNode* tempProject = sageProject;
  rewrite(getAlgebraicRules(), tempProject);
  sageProject = isSgProject(tempProject);
  ROSE_ASSERT (sageProject);

  FindFunctionsVis().traverse(sageProject, preorder);

  for (unsigned int x = 0; x < functions.size(); ++x) {
    SgBasicBlock* body = functions[x]->get_body();

    exprs.clear();
    FindMinusVisitor().traverse(body, preorder);

    for (unsigned int i = 0; i < exprs.size(); ++i) {
      SgStatement* stmt = 
	isSgStatement(exprs[i]->get_parent()->get_parent()->get_parent());
      if (stmt)
	SageInterface::myRemoveStatement(stmt);
      doFiniteDifferencingOne(exprs[i], body, getFiniteDifferencingRules());
    }

    bang_exprs.clear();
    FindNotVisitor().traverse(body, preorder);

    for (unsigned int i = 0; i < bang_exprs.size(); ++i) {
      SgStatement* bang_stmt = 
	isSgStatement(bang_exprs[i]->get_parent()->get_parent()->get_parent());
      if (!bang_stmt) continue;
      SgBasicBlock* bb = 
	isSgBasicBlock(bang_stmt->get_parent());
      if (!bb) continue;
      SgVarRefExp* vr = isSgVarRefExp(bang_exprs[i]);
      if (!vr) continue;
      SageInterface::myRemoveStatement(bang_stmt);
      simpleUndoFiniteDifferencingOne(bb, vr);
    }
    SgNode* tempBody = body;
    rewrite(getAlgebraicRules(), tempBody);
    body = isSgBasicBlock(tempBody);
    ROSE_ASSERT (body);
  }

  // AstPDFGeneration().generateInputFiles(sageProject);

  // Generate the final C++ source code from the potentially modified SAGE AST
  sageProject->unparse();

  // What remains is to run the specified compiler (typically the C++ compiler) using 
  // the generated output file (unparsed and transformed application code) to generate
  // an object file.
  int finalCombinedExitStatus = sageProject->compileOutput();

  // return exit code from complilation of generated (unparsed) code
  return finalCombinedExitStatus;
}
