#include "SIMDAnalysis.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace SIMDAnalysis;

/******************************************************************************************************************************/
/*
  Check if the loop is innermostloop.  If yes, it is candidate for SIMD translation.
  Otherwise, it needs more transformation to perform SIMD. 
  We perform the node query on a forStatement.  If the result has only one forStatement from the query,
  this imply it's a innermost loop.
*/
/******************************************************************************************************************************/
bool SIMDAnalysis::isInnermostLoop(SgNode* n)
{
  Rose_STL_Container<SgNode*> innerLoopList;
  bool result = false;
  switch(n->variantT())
  {
    case V_SgForStatement:
      {
        SgForStatement* forStatement = isSgForStatement(n);
        innerLoopList = NodeQuery::querySubTree (forStatement,V_SgForStatement);
        if(innerLoopList.size() == 1)
          result = true;
        else
           result = false;
      }
      break;
    case V_SgFortranDo:
      {
        SgFortranDo* fortranDo = isSgFortranDo(n);
        innerLoopList = NodeQuery::querySubTree (fortranDo,V_SgFortranDo);
        if(innerLoopList.size() == 1)
          result = true;
        else
          result = false;
      }
      break;
    case V_SgWhileStmt:
      {
        SgWhileStmt* whileStatement = isSgWhileStmt(n);
        innerLoopList = NodeQuery::querySubTree (whileStatement,V_SgWhileStmt);
        if(innerLoopList.size() == 1)
          result = true;
        else
          result = false;
      }
      break;
    default:
      {
        cerr<<"warning, unhandled node type: "<< n->class_name()<<endl;
        result = false;
      }
  }
  return result;
}

/******************************************************************************************************************************/
/*
  Check if the loop has stride distance 1.  
  We only test this after the loop normalization.  Therefore, the increment expression has only two cases:
  1. i += 1;
  2. i = i +/- k;
*/
/******************************************************************************************************************************/

bool SIMDAnalysis::isStrideOneLoop(SgNode* loop)
{
  SgInitializedName* ivar = NULL;
  SgExpression* lb = NULL;
  SgExpression* ub = NULL;
  SgExpression* step = NULL;
  SgStatement* orig_body = NULL;
  bool is_canonical = false;

  if (SgFortranDo* doLoop = isSgFortranDo(loop))
  {
    is_canonical = isCanonicalDoLoop(doLoop, &ivar, &lb, &ub, &step, &orig_body, NULL, NULL);
  }
  else if  (SgForStatement* forLoop = isSgForStatement(loop))
  {
    is_canonical = isCanonicalForLoop(forLoop, &ivar, &lb, &ub, &step, &orig_body, NULL, NULL);
  }
  else
  {
    cerr<<"warning, input is not loop. "<< endl;    
    ROSE_ASSERT(false);
  }

  SgIntVal* strideDistance = isSgIntVal(step);
  return (is_canonical && (strideDistance != NULL) && (strideDistance->get_value() == 1));
}

static SgExpression* SkipCasting (SgExpression* exp)
{
  SgCastExp* cast_exp = isSgCastExp(exp);
   if (cast_exp != NULL)
   {
      SgExpression* operand = cast_exp->get_operand();
      assert(operand != 0);
      return SkipCasting(operand);
   }
  else
    return exp;
}
//!Return the loop index variables for a C/C++ for or Fortran Do loop
vector<SgInitializedName*> SIMDAnalysis::getLoopIndexVariable(SgNode* loop)
{
  ROSE_ASSERT(loop != NULL);
  SgInitializedName* varName;
  vector<SgInitializedName*> ivarname;

  // Fortran case ------------------
  if (SgFortranDo * do_loop = isSgFortranDo(loop))
  {
    SgAssignOp* assign_op = isSgAssignOp (do_loop->get_initialization());
    ROSE_ASSERT (assign_op != NULL);
    SgVarRefExp* var = isSgVarRefExp(assign_op->get_lhs_operand());
    ROSE_ASSERT (var != NULL);
    varName = var->get_symbol()->get_declaration();
    ROSE_ASSERT (varName != NULL);
    ivarname.push_back(varName);
    return ivarname;
  }
  // C/C++ case ------------------------------
  SgForStatement* fs = isSgForStatement(loop);
  ROSE_ASSERT (fs != NULL);

  
  //Check initialization statement is something like i=xx;
  SgStatementPtrList & init = fs->get_init_stmt();
  for(SgStatementPtrList::iterator i=init.begin();i!=init.end(); ++i)
  {
    SgStatement* init1 = (*i); 
    SgExpression* ivarast=NULL;

    bool isCase1=false, isCase2=false;
    //consider C99 style: for (int i=0;...)
    if (isSgVariableDeclaration(init1))
    {
      SgVariableDeclaration* decl = isSgVariableDeclaration(init1);
      varName = decl->get_variables().front();
      ROSE_ASSERT(varName != NULL);
      ivarname.push_back(varName);
      SgInitializer * initor = varName->get_initializer();
      if (isSgAssignInitializer(initor))
        isCase1 = true;
    }// other regular case: for (i=0;..)
    else if (isAssignmentStatement(init1, &ivarast))
    {
      SgVarRefExp* var = isSgVarRefExp(SkipCasting(ivarast));
      if (var)
      {
        varName = var->get_symbol()->get_declaration();
        ivarname.push_back(varName);
        isCase2 = true;
      }
    }
    else if (SgExprStatement* exp_stmt = isSgExprStatement(init1))
    { //case like: for (i = 1, len1 = 0, len2=0; i <= n; i++)
       // AST is: SgCommaOpExp -> SgAssignOp -> SgVarRefExp
      if (SgCommaOpExp* comma_exp = isSgCommaOpExp(exp_stmt->get_expression()))
      {
        SgCommaOpExp* leaf_exp = comma_exp;
        while (isSgCommaOpExp(leaf_exp->get_lhs_operand()))
          leaf_exp = isSgCommaOpExp(leaf_exp->get_lhs_operand());
        if (SgAssignOp* assign_op = isSgAssignOp(leaf_exp->get_lhs_operand()))
        {
          SgVarRefExp* var = isSgVarRefExp(assign_op->get_lhs_operand());
          if (var)
          {
            varName = var->get_symbol()->get_declaration();
            ivarname.push_back(varName);
          }
        }
      }
    }
    else
    {
      cerr<<"Error. SageInterface::getLoopIndexVariable(). Unhandled init_stmt type of SgForStatement"<<endl;
      cerr<<"Init statement is :"<<init1->class_name() <<" " <<init1->unparseToString()<<endl;
      init1->get_file_info()->display("Debug");
      ROSE_ASSERT (false);
    }

  }  

  return ivarname;
}

//!Check if a SgInitializedName is used as a loop index within a AST subtree
//! This function will use a bottom-up traverse starting from the subtree to find all enclosing loops and check if ivar is used as an index for either of them.
bool SIMDAnalysis::isLoopIndexVariable(SgInitializedName* ivar, SgNode* subtree_root)
{
  ROSE_ASSERT (ivar != NULL);
  ROSE_ASSERT (subtree_root != NULL);
  bool result = false;
  SgScopeStatement * cur_loop = findEnclosingLoop (getEnclosingStatement(subtree_root));
  while (cur_loop)
  {
    vector<SgInitializedName*> indexList = SIMDAnalysis::getLoopIndexVariable(cur_loop);
    vector<SgInitializedName*>::iterator it = find(indexList.begin(),indexList.end(), ivar);
    if (it != indexList.end())
    {
      result = true;
      break;
    }
    else
    { // findEnclosingLoop() is inclusive.
      cur_loop = findEnclosingLoop (getEnclosingStatement(cur_loop->get_parent()));
    }
  }
  return result;
}
