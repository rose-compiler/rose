// Liao, 5/1/2008
// Demonstrate how to build a if statement 
// The code was originally from Thomas.
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{

  SgProject *project = frontend (argc, argv);
  SgGlobal* global = getFirstGlobalScope(project);
  pushScopeStack(global);

  SgInitializedName* arg1 = buildInitializedName("n",buildPointerType(buildVoidType()));

// DQ (8/21/2010): buildStringType constructs a SgTypeString which is only used in Fortran!
// SgInitializedName* arg2 = buildInitializedName("desc",buildStringType());
// SgInitializedName* arg2 = buildInitializedName("desc",buildStringType(NULL,4));
   int stringSize = 7;
   Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
   SgIntVal* lengthExpression = new SgIntVal(fileInfo,stringSize,"7");
   ROSE_ASSERT(lengthExpression != NULL);
   SgInitializedName* arg2 = buildInitializedName("desc",buildStringType(lengthExpression));

  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList, arg1);
  appendArg(paraList, arg2);

  // build defining function declaration
  SgFunctionDeclaration * func_def = buildDefiningFunctionDeclaration \
    ("check_var",buildVoidType(),paraList);

   // Build a corresponding prototype
  // Must not share a parameter list for different function declarations!
    SgFunctionParameterList * paraList2 = isSgFunctionParameterList(deepCopy(paraList));
  SgFunctionDeclaration * func_decl = buildNondefiningFunctionDeclaration 
  (SgName("check_var"),buildVoidType(),paraList2); 

  // build a statement inside the function body
  SgBasicBlock *func_body = func_def->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
  pushScopeStack (func_body);

  SgBasicBlock* true_body = buildBasicBlock();
  SgBasicBlock* false_body = buildBasicBlock();
  SgVarRefExp* op1 = buildVarRefExp("n",isSgScopeStatement (func_body));
  SgExprStatement* conditional = buildExprStatement(buildEqualityOp(op1,buildIntVal(0)));
  SgIfStmt *ifstmt = buildIfStmt (conditional, true_body, false_body);
  appendStatement (ifstmt);

  popScopeStack ();
  // insert the defining and declaring function
  appendStatement (func_def);
  prependStatement (func_decl);

  popScopeStack ();
  AstTests::runAllTests(project);
  project->unparse();

  return 0;
}

