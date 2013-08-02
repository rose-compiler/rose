#include "sage3basic.h"

#include "defUseQuery.h"
#include <algorithm>
#include <iterator>
#include <set>

using namespace CodeThorn;

/*************************************************
 *************** DefUseVarsInfo ****************
 *************************************************/

DefUseVarsInfo::DefUseVarsInfo(const VarsInfo& _def_info, const VarsInfo& _use_info, const FunctionCallExpSet& _fset) :
  def_vars_info(_def_info), use_vars_info(_use_info), func_set(_fset)
{
}

VarsInfo DefUseVarsInfo::getDefVarsInfo()
{
  return def_vars_info;
}

VarsInfo DefUseVarsInfo::getUseVarsInfo()
{
  return use_vars_info;
}

VarsInfo& DefUseVarsInfo::getDefVarsInfoMod()
{
  return def_vars_info;
}

VarsInfo& DefUseVarsInfo::getUseVarsInfoMod()
{
  return use_vars_info;
}

const VarsInfo& DefUseVarsInfo::getDefVarsInfoRef() const
{
  return def_vars_info;
}

const VarsInfo& DefUseVarsInfo::getUseVarsInfoRef() const
{
  return use_vars_info;
}

FunctionCallExpSet DefUseVarsInfo::getFunctionCallExpSet()
{
  return func_set;
}

FunctionCallExpSet& DefUseVarsInfo::getFunctionCallExpSetMod()
{
  return func_set;
}

const FunctionCallExpSet& DefUseVarsInfo::getFunctionCallExpSetRef() const
{
  return func_set;
}

bool DefUseVarsInfo::isModByFunction()
{
  return (func_set.size() > 0);
}

bool DefUseVarsInfo::isDefSetModByPointer()
{
  return def_vars_info.second;
}

bool DefUseVarsInfo::isUseSetModByPointer()
{
  return use_vars_info.second;
}

bool DefUseVarsInfo::isDefSetEmpty()
{
  return def_vars_info.first.size() == 0;
}

bool DefUseVarsInfo::isFunctionCallExpSetEmpty()
{
  return func_set.size() == 0;
}

bool DefUseVarsInfo::isUseSetEmpty()
{
  return use_vars_info.first.size() == 0;
}

void DefUseVarsInfo::copyDefToUse()
{
  use_vars_info.first.insert(def_vars_info.first.begin(), def_vars_info.first.end());
  use_vars_info.second = use_vars_info.second || def_vars_info.second;
}

void DefUseVarsInfo::copyUseToDef()
{
  def_vars_info.first.insert(use_vars_info.first.begin(), use_vars_info.first.end());
  def_vars_info.second = def_vars_info.second || use_vars_info.second;
}

// combine the two DefUseVarsInfo functions
DefUseVarsInfo DefUseVarsInfo::operator+(const DefUseVarsInfo& duvi1)
{  
  const VarsInfo& d1_def_vars_info = duvi1.getDefVarsInfoRef();
  const VarsInfo& d1_use_vars_info = duvi1.getUseVarsInfoRef();
  const FunctionCallExpSet& d1_func_set = duvi1.getFunctionCallExpSetRef();

  VarsInfo rdef_vars_info, ruse_vars_info;
  FunctionCallExpSet rfunc_set;
  
  // not efficient way to merge the maps
  // the keys don't change
  // the types also don't change
  // inserting into new map to avoid side-effects on this object
  rdef_vars_info.first.insert(def_vars_info.first.begin(), def_vars_info.first.end());
  rdef_vars_info.first.insert(d1_def_vars_info.first.begin(), d1_def_vars_info.first.end());
  ruse_vars_info.first.insert(use_vars_info.first.begin(), use_vars_info.first.end());
  ruse_vars_info.first.insert(d1_use_vars_info.first.begin(), d1_use_vars_info.first.end());
  
  // set_union(def_set.first, d1_def_set.first, rdef_set.first);
  // set_union(use_set.first, d1_use_set.first, ruse_set.first);
  set_union(func_set.begin(), func_set.end(),
            d1_func_set.begin(), d1_func_set.end(),
            std::inserter(rfunc_set, rfunc_set.begin()));

  rdef_vars_info.second = def_vars_info.second || d1_def_vars_info.second;
  ruse_vars_info.second = use_vars_info.second || d1_use_vars_info.second;
  // rfunc_set.second = func_set.second || d1_func_set.second;

  return DefUseVarsInfo(rdef_vars_info, ruse_vars_info, rfunc_set);
}

std::string DefUseVarsInfo::functionCallExpSetPrettyPrint(FunctionCallExpSet& func_set)
{
  std::ostringstream oss;
  FunctionCallExpSet::iterator it = func_set.begin(); 
  oss << "{";
  for( ;it != func_set.end(); )
  {
    oss << (*it)->unparseToString();
    it++;
    if(it != func_set.end())
      oss << ", ";
  }
  oss << "}";
  return oss.str();
}

std::string DefUseVarsInfo::varsInfoPrettyPrint(VarsInfo& vars_info, VariableIdMapping& vidm)
{
  std::ostringstream oss;
  oss << "[" << (vars_info.second? "true" : "false") << ", ";
  oss << "{";
  VariableIdInfoMap::iterator it = vars_info.first.begin();
  for( ; it != vars_info.first.end();  )
  {
    // oss << "<" << (*it).first.toString() << ", " << vidm.variableName((*it).first) << ", ";
    oss << "<" << vidm.variableName((*it).first) << ", ";
    switch((*it).second) {
    case VARIABLE:
      oss << "var";
      break;
    case ARRAY:
      oss << "array";
      break;
    case POINTER:
      oss << "ptr";
      break;
    case REFERENCE:
      oss << "ref";
      break;
    }
    oss <<">";
    it++;
    if(it != vars_info.first.end())
      oss << ", ";
  }
  oss << "}";
  oss <<"]";
  return oss.str();
}

std::string DefUseVarsInfo::str(VariableIdMapping& vidm)
{
  std::ostringstream oss;
  oss << "def_vars_info: " << varsInfoPrettyPrint(def_vars_info, vidm) << "\n";
  oss << "use_vars_info: " << varsInfoPrettyPrint(use_vars_info, vidm) << "\n";
  oss << "func_set: " << functionCallExpSetPrettyPrint(func_set) << ">\n";
  return oss.str();
}


ExprWalker::ExprWalker(VariableIdMapping& _vidm, bool _isModExpr) :
  vidm(_vidm), isModExpr(_isModExpr)
{
  // default constructor
}

void ExprWalker::visit(SgAssignOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi = getDefUseVarsInfo_rec(lhs, vidm, true);
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(rhs, vidm, false);
  // if the rhs writes to a memory (i.e sideffect)
  // add to the use_set to be unioned in next step
  if(!rduvi.isDefSetEmpty())
  {
    rduvi.copyDefToUse();
  }
  // union lduvi and rduvi
  duvi = lduvi + rduvi;
}

void ExprWalker::visit(SgCompoundAssignOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi = getDefUseVarsInfo_rec(lhs, vidm, true);
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(rhs, vidm, false);
  // if the rhs writes to a memory (i.e side-effect)
  // add to the def_set to be unioned later
  if(!rduvi.isDefSetEmpty())
  {
    rduvi.copyDefToUse();
  }
  // union lduvi and rduvi
  duvi = lduvi + rduvi;
}

void ExprWalker::visit(SgCastExp* sgn)
{
  SgNode* operand = sgn->get_operand();
  DefUseVarsInfo opduvi = getDefUseVarsInfo_rec(operand, vidm, false);
  duvi = opduvi;
}

void ExprWalker::visit(SgAddressOfOp* sgn)
{
  SgNode* operand = sgn->get_operand();
  DefUseVarsInfo opduvi = getDefUseVarsInfo_rec(operand, vidm, false);
  duvi = opduvi;
}

void ExprWalker::visit(SgMinusMinusOp* sgn)
{
  DefUseVarsInfo uduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  // all the side-effects and the variable are also used by this expression
  uduvi.copyUseToDef();
  duvi = uduvi;
}
void ExprWalker::visit(SgMinusOp* sgn)
{
  // its only used
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();
  duvi = rduvi;
}

void ExprWalker::visit(SgUnaryAddOp *sgn)
{
  // its only used
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();
  duvi = rduvi;
}

void ExprWalker::visit(SgNotOp* sgn)
{
  // its only used
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();
  duvi = rduvi;
}
  
void ExprWalker::visit(SgPlusPlusOp* sgn)
{
  DefUseVarsInfo uduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  // all the side-effects are also used by this expression
  uduvi.copyUseToDef();
  duvi = uduvi;
}

void ExprWalker::visit(SgSizeOfOp* sgn)
{
  DefUseVarsInfo rduvi;
  // we only need to process if the operand is an expression
  SgExpression* expr = sgn->get_operand_expr();
  // expr can be null if the sizeof operand is a type
  if(expr) {
    rduvi = getDefUseVarsInfo_rec(expr, vidm, false);
    if(!rduvi.isDefSetEmpty())
      rduvi.copyDefToUse();
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgBinaryOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi = getDefUseVarsInfo_rec(lhs, vidm, false);
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(rhs, vidm, false);
  // both operands are uses
  // if they write to any memory location as side-effect
  // copy the defs to uses
  if(!lduvi.isDefSetEmpty())
  {
    lduvi.copyDefToUse();
  }
  if(!rduvi.isDefSetEmpty())
  {
    rduvi.copyDefToUse();
  }
  // union lduvi and rduvi
  duvi = lduvi + rduvi;
}

void ExprWalker::visit(SgFunctionCallExp* sgn)
{
  FunctionCallExpSet& func_set = duvi.getFunctionCallExpSetMod();
  func_set.insert(sgn);
}

void ExprWalker::visit(SgExprListExp* sgn)
{
  SgExpressionPtrList expr_list = sgn->get_expressions();
  SgExpressionPtrList::iterator it = expr_list.begin();
  for( ; it != expr_list.end(); ++it)
  {
    // if they have side-effects we can copy them over
    DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(*it, vidm, false);
    if(!rduvi.isDefSetEmpty())
      rduvi.copyDefToUse();
    duvi = duvi + rduvi;
  }
}

void ExprWalker::visit(SgConditionalExp* sgn)
{
  SgExpression* cond_exp = sgn->get_conditional_exp();
  SgExpression* true_exp = sgn->get_true_exp();
  SgExpression* false_exp = sgn->get_false_exp();

  DefUseVarsInfo cduvi = getDefUseVarsInfo_rec(cond_exp, vidm, false);
  if(!cduvi.isDefSetEmpty()) {
    cduvi.copyDefToUse();
  }

  DefUseVarsInfo tduvi, fduvi;

  if(isModExpr) {
    tduvi = getDefUseVarsInfo_rec(true_exp, vidm, true);
    fduvi = getDefUseVarsInfo_rec(false_exp, vidm, true);
    
  }
  else {
    tduvi = getDefUseVarsInfo_rec(true_exp, vidm, false);
    fduvi = getDefUseVarsInfo_rec(false_exp, vidm, false);
    if(!tduvi.isDefSetEmpty())
      tduvi.copyDefToUse();
    if(!fduvi.isDefSetEmpty())
      fduvi.copyDefToUse();
  }
  duvi = cduvi + tduvi + fduvi;
}

void ExprWalker::visit(SgVarRefExp* sgn)
{
  // recursion base case
  DefUseVarsInfo rduvi = getDefUseVarsInfoLvalue(sgn, vidm, isModExpr);
  duvi = rduvi;
}

void ExprWalker::visit(SgPntrArrRefExp* sgn)
{
  DefUseVarsInfo rduvi = getDefUseVarsInfoLvalue(sgn, vidm, isModExpr);
  duvi = rduvi;
}

void ExprWalker::visit(SgPointerDerefExp* sgn)
{
  // *p + i++ ??
  DefUseVarsInfo rduvi = getDefUseVarsInfoLvalue(sgn, vidm, isModExpr);
  duvi = rduvi;
}

void ExprWalker::visit(SgArrowExp* sgn)
{
  DefUseVarsInfo rduvi = getDefUseVarsInfoLvalue(sgn, vidm, isModExpr);
  duvi = rduvi;
}

void ExprWalker::visit(SgDotExp *sgn)
{
  DefUseVarsInfo rduvi = getDefUseVarsInfoLvalue(sgn, vidm, isModExpr);
  duvi = rduvi;
}

void ExprWalker::visit(SgInitializedName* sgn)
{
  SgInitializer* initializer = sgn->get_initializer();
  if(initializer) {
    DefUseVarsInfo lduvi = getDefUseVarsInfoLvalue(sgn, vidm, true);
    DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(initializer, vidm, false);

    if(! rduvi.isDefSetEmpty())
      rduvi.copyDefToUse();
    duvi = lduvi + rduvi;
  }
}

void ExprWalker::visit(SgAssignInitializer *sgn)
{
  // operand is only used
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();
  duvi = rduvi;
}

void ExprWalker::visit(SgConstructorInitializer *sgn)
{
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(sgn->get_args(), vidm, false);
  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();
  duvi = rduvi;
}

void ExprWalker::visit(SgValueExp* sgn)
{
  // dont need to do anything
}

void ExprWalker::visit(SgFunctionRefExp* sgn)
{
  // not sure
  // does not have any def/use semantics
}

void ExprWalker::visit(SgMemberFunctionRefExp* sgn)
{
  // not sure
  // does not have any def/use semantics
}

void ExprWalker::visit(SgExpression* sgn)
{
  try {
    std::ostringstream oss;
    oss << "Not handling " << sgn->class_name() << " expression \n";
    throw oss.str();
  }
  catch(std::string exp)
  {
    std::cerr << exp << "\n";
    abort();
  }
}

DefUseVarsInfo ExprWalker::getDefUseVarsInfo()
{
  return duvi;
}

LvalueExprWalker::LvalueExprWalker(VariableIdMapping& _vidm, bool _isModExpr)
  : vidm(_vidm), isModExpr(_isModExpr)
{
}

void LvalueExprWalker::visit(SgInitializedName* sgn)
{
  VarsInfo& def_vars_info = duvi.getDefVarsInfoMod();
  VarsInfo& use_vars_info = duvi.getUseVarsInfoMod();
  // determine the type
  SgType* sgn_type = sgn->get_type();
  VariableIdTypeInfo sgn_type_info;

  if(isSgArrayType(sgn_type))
    sgn_type_info = ARRAY;
  else if(isSgPointerType(sgn_type))
    sgn_type_info = POINTER;
  else if(isSgReferenceType(sgn_type))
    sgn_type_info = REFERENCE;
  else
    sgn_type_info = VARIABLE;

  if(isModExpr)
    def_vars_info.first.insert(VariableIdInfo(vidm.variableId(sgn), sgn_type_info));
  else
    use_vars_info.first.insert(VariableIdInfo(vidm.variableId(sgn), sgn_type_info));
}

void LvalueExprWalker::visit(SgVarRefExp* sgn)
{
  VarsInfo& def_vars_info = duvi.getDefVarsInfoMod();
  VarsInfo& use_vars_info = duvi.getUseVarsInfoMod();

  SgType* sgn_type = sgn->get_type();
  VariableIdTypeInfo sgn_type_info;

  if(isSgArrayType(sgn_type))
    sgn_type_info = ARRAY;
  else if(isSgPointerType(sgn_type))
    sgn_type_info = POINTER;
  else if(isSgReferenceType(sgn_type))
    sgn_type_info = REFERENCE;
  else
    sgn_type_info = VARIABLE;

  if(isModExpr)
    def_vars_info.first.insert(VariableIdInfo(vidm.variableId(sgn), sgn_type_info));
  else
    use_vars_info.first.insert(VariableIdInfo(vidm.variableId(sgn), sgn_type_info));
}

void LvalueExprWalker::visit(SgPointerDerefExp* sgn)
{
  // set the flag for the def/use set
  // based on what we are processing

  // we raise the flag
  // we don't know what memory this dereferencing expression is pointing to
  // if this flag is set, then the expression is modifying/using something
  // other than what appears in the expression.
  if(isModExpr)
  {
    VarsInfo& def_s = duvi.getDefVarsInfoMod();
    def_s.second = true;
  }
  else 
  {
    VarsInfo& use_s = duvi.getUseVarsInfoMod();
    use_s.second = true;
  }

  // Process the operand recursively
  // The operand is viewed only as an expression
  // that will only be used. The operand expression can
  // have side-effects that will be captured.
  // For simplicity, we process the operand as non-modifying
  // expression by passing false to the recursive function.
  // It is conservative as we raise the flag and do not
  // give any guarantee about what is used/modfied.
  // In some cases it is possible to find out what is modified
  // in a dereferencing expression. Arrays can also be modified
  // using dereferencing expression. Consider *(arr + 10), it
  // is different from other dereferencing expression as arr is
  // of SgArrayType while p in *p is SgPointerType. However arr
  // can appear deep in the operand expression *(i + b[j]+.. + arr) = exp.
  // We would need to walk this operand expression and extract arr.
  // If the sub-expression is of SgArrayType, we should add it to def_set.
  // In expressions like *(*(arr + i) + j), *(arr + i)
  // is of SgArrayType and adds more complications to the walker. Here we
  // are conservative and just raise the flag when we don't know what
  // exactly is modified.  
  
  SgNode* operand = sgn->get_operand();
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(operand, vidm, false);

  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();

  // update the results
  duvi = rduvi + duvi;
}

void LvalueExprWalker::visit(SgPntrArrRefExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand();
  SgNode* rhs_expr = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi, rduvi;
  if(isModExpr)
  { 
    // consider moving this to ExprWalker instead
    // prune out the sub-tree that is difficult to handle
    if(isSgPntrArrRefExp(lhs_addr) || // for multi-dimensional pointers
       isSgVarRefExp(lhs_addr))       // for array type its variable reference exp
    {
      // we can handle these
      lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, true);
    }
    else 
    { 
      // otherwise some crazy arithmetic is going on to determine
      // the address of the array
      lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, false);
      // copy side-effects and set the flag
      if(!lduvi.isDefSetEmpty())
        lduvi.copyDefToUse();
      
      lduvi.getDefVarsInfoMod().second = true;
    }
    
  }
  else
  {
    lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, false); 
  }
  rduvi = getDefUseVarsInfo_rec(rhs_expr, vidm, false);
  // if we have side-effects copy them over
  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();
  // update the values
  duvi = lduvi + rduvi;
}

void LvalueExprWalker::visit(SgArrowExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand();
  SgNode* rhs_expr = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi, rduvi;
  // only right op is modified
  if(isModExpr)
  {
    rduvi = getDefUseVarsInfo_rec(rhs_expr, vidm, true);
  }
  else
  {
    rduvi = getDefUseVarsInfo_rec(rhs_expr, vidm, false); 
  }
  // left is only used
  lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, false);

  // if we have side-effects from left, copy them
  if(!lduvi.isDefSetEmpty())
    lduvi.copyDefToUse();

  // update the values
  duvi = lduvi + rduvi;
}

void LvalueExprWalker::visit(SgDotExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand();
  SgNode* rhs_expr = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi, rduvi;
  // only right op is modified
  if(isModExpr)
  {
    rduvi = getDefUseVarsInfo_rec(rhs_expr, vidm, true);
  }
  else
  {
    rduvi = getDefUseVarsInfo_rec(rhs_expr, vidm, false); 
  }
  // left is only used
  lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, false);

  // if we have side-effects from left, copy them
  if(!lduvi.isDefSetEmpty())
    lduvi.copyDefToUse();

  // update the values
  duvi = lduvi + rduvi;
}

DefUseVarsInfo LvalueExprWalker::getDefUseVarsInfo()
{
  return duvi;
}

// interface function
DefUseVarsInfo getDefUseVarsInfo(SgNode* sgn, VariableIdMapping& vidm)
{
  return getDefUseVarsInfo_rec(sgn, vidm, false);  
}

// main implementation
DefUseVarsInfo getDefUseVarsInfo_rec(SgNode* sgn, VariableIdMapping& vidm, bool isModExpr)
{
  ExprWalker exprw(vidm, isModExpr);
  sgn->accept(exprw);
  return exprw.getDefUseVarsInfo();
}

DefUseVarsInfo getDefUseVarsInfoLvalue(SgNode* sgn, VariableIdMapping& vidm, bool isModExpr)
{
  LvalueExprWalker lvalw(vidm, isModExpr);
  sgn->accept(lvalw);
  return lvalw.getDefUseVarsInfo();
}
