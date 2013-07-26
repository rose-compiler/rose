#include "defUseQuery.h"
#include <algorithm>
#include <iterator>
#include <set>

/*************************************************
 *************** DefUseMemObjInfo ****************
 *************************************************/

DefUseMemObjInfo::DefUseMemObjInfo(const MemObjInfo& _dset, const MemObjInfo& _uset, const FunctionCallExpInfo& _fset) :
  def_set(_dset), use_set(_uset), func_set(_fset)
{
}

MemObjInfo DefUseMemObjInfo::getDefMemObjInfo()
{
  return def_set;
}

MemObjInfo DefUseMemObjInfo::getUseMemObjInfo()
{
  return use_set;
}

MemObjInfo& DefUseMemObjInfo::getDefMemObjInfoMod()
{
  return def_set;
}

MemObjInfo& DefUseMemObjInfo::getUseMemObjInfoMod()
{
  return use_set;
}

const MemObjInfo& DefUseMemObjInfo::getDefMemObjInfoRef() const
{
  return def_set;
}

const MemObjInfo& DefUseMemObjInfo::getUseMemObjInfoRef() const
{
  return use_set;
}

FunctionCallExpInfo DefUseMemObjInfo::getFunctionCallExpInfo()
{
  return func_set;
}

FunctionCallExpInfo& DefUseMemObjInfo::getFunctionCallExpInfoMod()
{
  return func_set;
}

const FunctionCallExpInfo& DefUseMemObjInfo::getFunctionCallExpInfoRef() const
{
  return func_set;
}

bool DefUseMemObjInfo::isModByFunction()
{
  return func_set.second;
}

bool DefUseMemObjInfo::isDefSetModByPointer()
{
  return def_set.second;
}

bool DefUseMemObjInfo::isUseSetModByPointer()
{
  return use_set.second;
}

bool DefUseMemObjInfo::isDefSetEmpty()
{
  return def_set.first.size() == 0;
}

bool DefUseMemObjInfo::isFunctionCallExpInfoEmpty()
{
  return func_set.first.size() == 0;
}

bool DefUseMemObjInfo::isUseSetEmpty()
{
  return use_set.first.size() == 0;
}

void DefUseMemObjInfo::copyDefToUse()
{
  use_set.first.insert(def_set.first.begin(), def_set.first.end());
  use_set.second = use_set.second || def_set.second;
}

void DefUseMemObjInfo::copyUseToDef()
{
  def_set.first.insert(use_set.first.begin(), use_set.first.end());
  def_set.second = def_set.second || use_set.second;
}

// combine the two DefUseMemObjInfo functions
DefUseMemObjInfo DefUseMemObjInfo::operator+(const DefUseMemObjInfo& dumo1)
{  
  const MemObjInfo& d1_def_set = dumo1.getDefMemObjInfoRef();
  const MemObjInfo& d1_use_set = dumo1.getUseMemObjInfoRef();
  const FunctionCallExpInfo& d1_func_set = dumo1.getFunctionCallExpInfoRef();

  MemObjInfo rdef_set, ruse_set;
  FunctionCallExpInfo rfunc_set;
  
  set_union(def_set.first, d1_def_set.first, rdef_set.first);
  set_union(use_set.first, d1_use_set.first, ruse_set.first);
  set_union(func_set.first.begin(), func_set.first.end(),
            d1_func_set.first.begin(), d1_func_set.first.end(),
            std::inserter(rfunc_set.first, rfunc_set.first.begin()));

  rdef_set.second = def_set.second || d1_def_set.second;
  ruse_set.second = use_set.second || d1_use_set.second;
  rfunc_set.second = func_set.second || d1_func_set.second;

  return DefUseMemObjInfo(rdef_set, ruse_set, rfunc_set);
}

std::string DefUseMemObjInfo::str()
{
  std::ostringstream oss;
  oss << "def_set:<" << VariableIdSetPrettyPrint::str(def_set.first) << ">\n";
  oss << "use_set:<" << VariableIdSetPrettyPrint::str(use_set.first) << ">\n";
  return oss.str();
}

std::string DefUseMemObjInfo::funcCallExpSetPrettyPrint()
{
  std::ostringstream oss;
  FunctionCallExpSet::iterator it = func_set.first.begin(); 
  oss << "[";
  for( ;it != func_set.first.end(); )
  {
    oss << (*it)->unparseToString();
    it++;
    if(it != func_set.first.end())
      oss << ", ";
  }
  oss << "]";
  return oss.str();
}

std::string DefUseMemObjInfo::str(VariableIdMapping& vidm)
{
  std::ostringstream oss;
  oss << "def_set:<" << def_set.second << "," << VariableIdSetPrettyPrint::str(def_set.first, vidm) << ">\n";
  oss << "use_set:<" << use_set.second << ", "<< VariableIdSetPrettyPrint::str(use_set.first, vidm) << ">\n";
  oss << "func_set:<" << func_set.second << ", " << funcCallExpSetPrettyPrint() << ">\n";
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
  DefUseMemObjInfo ldumo = getDefUseMemObjInfo_rec(lhs, vidm, true);
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(rhs, vidm, false);
  // if the rhs writes to a memory (i.e sideffect)
  // add to the def_set to be unioned in next step
  if(!rdumo.isDefSetEmpty())
  {
    rdumo.copyDefToUse();
  }
  // union ldumo and rdumo
  dumo = ldumo + rdumo;
}

void ExprWalker::visit(SgCompoundAssignOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo = getDefUseMemObjInfo_rec(lhs, vidm, true);
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(rhs, vidm, false);
  // if the rhs writes to a memory (i.e side-effect)
  // add to the def_set to be unioned later
  if(!rdumo.isDefSetEmpty())
  {
    rdumo.copyDefToUse();
  }
  // union ldumo and rdumo
  dumo = ldumo + rdumo;
}

void ExprWalker::visit(SgCastExp* sgn)
{
  SgNode* operand = sgn->get_operand();
  DefUseMemObjInfo opdumo = getDefUseMemObjInfo_rec(operand, vidm, false);
  dumo = opdumo;
}

void ExprWalker::visit(SgAddressOfOp* sgn)
{
  SgNode* operand = sgn->get_operand();
  DefUseMemObjInfo opdumo = getDefUseMemObjInfo_rec(operand, vidm, false);
  dumo = opdumo;
}

void ExprWalker::visit(SgMinusMinusOp* sgn)
{
  DefUseMemObjInfo udumo = getDefUseMemObjInfo_rec(sgn->get_operand(), vidm, false);
  // all the side-effects and the variable are also used by this expression
  udumo.copyUseToDef();
  dumo = udumo;
}
void ExprWalker::visit(SgMinusOp* sgn)
{
  // its only used
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(sgn->get_operand(), vidm, false);
  if(!rdumo.isDefSetEmpty())
    rdumo.copyDefToUse();
  dumo = rdumo;
}

void ExprWalker::visit(SgNotOp* sgn)
{
  // its only used
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(sgn->get_operand(), vidm, false);
  if(!rdumo.isDefSetEmpty())
    rdumo.copyDefToUse();
  dumo = rdumo;
}
  
void ExprWalker::visit(SgPlusPlusOp* sgn)
{
  DefUseMemObjInfo udumo = getDefUseMemObjInfo_rec(sgn->get_operand(), vidm, false);
  // all the side-effects are also used by this expression
  udumo.copyUseToDef();
  dumo = udumo;
}

void ExprWalker::visit(SgBinaryOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo = getDefUseMemObjInfo_rec(lhs, vidm, false);
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(rhs, vidm, false);
  // both operands are uses
  // if they write to any memory location as side-effect
  // copy the defs to uses
  if(!ldumo.isDefSetEmpty())
  {
    ldumo.copyDefToUse();
  }
  if(!rdumo.isDefSetEmpty())
  {
    rdumo.copyDefToUse();
  }
  // union ldumo and rdumo
  dumo = ldumo + rdumo;
}

void ExprWalker::visit(SgFunctionCallExp* sgn)
{
  FunctionCallExpInfo& func_set = dumo.getFunctionCallExpInfoMod();
  func_set.first.insert(sgn);
  func_set.second = true;
}

void ExprWalker::visit(SgExprListExp* sgn)
{
  SgExpressionPtrList expr_list = sgn->get_expressions();
  SgExpressionPtrList::iterator it = expr_list.begin();
  for( ; it != expr_list.end(); ++it)
  {
    // if they have side-effects we can copy them over
    DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(*it, vidm, false);
    if(!rdumo.isDefSetEmpty())
      rdumo.copyDefToUse();
    dumo = dumo + rdumo;
  }
}

void ExprWalker::visit(SgVarRefExp* sgn)
{
  // recursion base case
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, vidm, isModExpr);
  dumo = rdumo;
}

void ExprWalker::visit(SgPntrArrRefExp* sgn)
{
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, vidm, isModExpr);
  dumo = rdumo;
}

void ExprWalker::visit(SgPointerDerefExp* sgn)
{
  // *p + i++ ??
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, vidm, isModExpr);
  dumo = rdumo;
}

void ExprWalker::visit(SgArrowExp* sgn)
{
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, vidm, isModExpr);
  dumo = rdumo;
}

void ExprWalker::visit(SgDotExp *sgn)
{
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, vidm, isModExpr);
  dumo = rdumo;
}

void ExprWalker::visit(SgInitializedName* sgn)
{
  SgInitializer* initializer = sgn->get_initializer();
  
  // only then we process
  if(isSgAssignInitializer(initializer))
  {
    DefUseMemObjInfo ldumo = getDefUseMemObjInfoLvalue(sgn, vidm, true);
    DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(isSgAssignInitializer(initializer)->get_operand(),
                                                     vidm,
                                                     false);
    if(! rdumo.isDefSetEmpty())
      rdumo.copyDefToUse();
    dumo = ldumo + rdumo;
  }  
}

void ExprWalker::visit(SgAssignInitializer *sgn)
{
  // operand is only used
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(sgn->get_operand(), vidm, false);
  if(!rdumo.isDefSetEmpty())
    rdumo.copyDefToUse();
  dumo = rdumo;
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

DefUseMemObjInfo ExprWalker::getDefUseMemObjInfo()
{
  return dumo;
}

LvalueExprWalker::LvalueExprWalker(VariableIdMapping& _vidm, bool _isModExpr)
  : vidm(_vidm), isModExpr(_isModExpr)
{
}

void LvalueExprWalker::visit(SgInitializedName* sgn)
{
  MemObjInfo& def_set = dumo.getDefMemObjInfoMod();
  MemObjInfo& use_set = dumo.getUseMemObjInfoMod();
  if(isModExpr)
    def_set.first.insert(vidm.variableId(sgn));
  else
    use_set.first.insert(vidm.variableId(sgn));
}

void LvalueExprWalker::visit(SgVarRefExp* sgn)
{
  MemObjInfo& def_set = dumo.getDefMemObjInfoMod();
  MemObjInfo& use_set = dumo.getUseMemObjInfoMod();
  // insert into def_set if on lhs
  if(isModExpr)
  {
    def_set.first.insert(vidm.variableId(sgn));
  }
  // insert into use_set otherwise
  else
  {
    use_set.first.insert(vidm.variableId(sgn));
  }
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
    MemObjInfo& def_s = dumo.getDefMemObjInfoMod();
    def_s.second = true;
  }
  else 
  {
    MemObjInfo& use_s = dumo.getUseMemObjInfoMod();
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
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(operand, vidm, false);

  if(!rdumo.isDefSetEmpty())
    rdumo.copyDefToUse();

  // update the results
  dumo = rdumo + dumo;
}

void LvalueExprWalker::visit(SgPntrArrRefExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand();
  SgNode* rhs_expr = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo, rdumo;
  if(isModExpr)
  {
    ldumo = getDefUseMemObjInfo_rec(lhs_addr, vidm, true);
  }
  else
  {
    ldumo = getDefUseMemObjInfo_rec(lhs_addr, vidm, false); 
  }
  rdumo = getDefUseMemObjInfo_rec(rhs_expr, vidm, false);
  // if we have side-effects copy them over
  if(!rdumo.isDefSetEmpty())
    rdumo.copyDefToUse();
  // update the values
  dumo = ldumo + rdumo;
}

void LvalueExprWalker::visit(SgArrowExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand();
  SgNode* rhs_expr = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo, rdumo;
  // only right op is modified
  if(isModExpr)
  {
    rdumo = getDefUseMemObjInfo_rec(rhs_expr, vidm, true);
  }
  else
  {
    rdumo = getDefUseMemObjInfo_rec(rhs_expr, vidm, false); 
  }
  // left is only used
  ldumo = getDefUseMemObjInfo_rec(lhs_addr, vidm, false);

  // if we have side-effects from left, copy them
  if(!ldumo.isDefSetEmpty())
    ldumo.copyDefToUse();

  // update the values
  dumo = ldumo + rdumo;
}

void LvalueExprWalker::visit(SgDotExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand();
  SgNode* rhs_expr = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo, rdumo;
  // only right op is modified
  if(isModExpr)
  {
    rdumo = getDefUseMemObjInfo_rec(rhs_expr, vidm, true);
  }
  else
  {
    rdumo = getDefUseMemObjInfo_rec(rhs_expr, vidm, false); 
  }
  // left is only used
  ldumo = getDefUseMemObjInfo_rec(lhs_addr, vidm, false);

  // if we have side-effects from left, copy them
  if(!ldumo.isDefSetEmpty())
    ldumo.copyDefToUse();

  // update the values
  dumo = ldumo + rdumo;
}

DefUseMemObjInfo LvalueExprWalker::getDefUseMemObjInfo()
{
  return dumo;
}

// interface function
DefUseMemObjInfo getDefUseMemObjInfo(SgNode* sgn, VariableIdMapping& vidm)
{
  return getDefUseMemObjInfo_rec(sgn, vidm, false);  
}

// main implementation
DefUseMemObjInfo getDefUseMemObjInfo_rec(SgNode* sgn, VariableIdMapping& vidm, bool isModExpr)
{
  ExprWalker exprw(vidm, isModExpr);
  sgn->accept(exprw);
  return exprw.getDefUseMemObjInfo();
}

DefUseMemObjInfo getDefUseMemObjInfoLvalue(SgNode* sgn, VariableIdMapping& vidm, bool isModExpr)
{
  LvalueExprWalker lvalw(vidm, isModExpr);
  sgn->accept(lvalw);
  return lvalw.getDefUseMemObjInfo();
}
