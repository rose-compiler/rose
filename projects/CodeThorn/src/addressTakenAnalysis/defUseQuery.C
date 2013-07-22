#include "defUseQuery.h"

/*************************************************
 ***************** DefUseMemObjInfo ******************
 *************************************************/

DefUseMemObjInfo::DefUseMemObjInfo(const VariableIdSet& _def_set, const VariableIdSet& _use_set) :
  def_set(_def_set), use_set(_use_set)
{
  ptr_modify = false;
  func_modify = false;
}

VariableIdSet DefUseMemObjInfo::getDefSet()
{
  return def_set;
}

VariableIdSet DefUseMemObjInfo::getUseSet()
{
  return use_set;
}

const VariableIdSet& DefUseMemObjInfo::getDefSetRef() const
{
  return def_set;
}

const VariableIdSet& DefUseMemObjInfo::getUseSetRef() const
{
  return use_set;
}

VariableIdSet& DefUseMemObjInfo::getDefSetRefMod()
{
  return def_set;
}

VariableIdSet& DefUseMemObjInfo::getUseSetRefMod()
{
  return use_set;
}

bool DefUseMemObjInfo::isModByPointer()
{
  return ptr_modify;
}

bool DefUseMemObjInfo::isModByFunction()
{
  return func_modify;
}

bool DefUseMemObjInfo::isDefSetEmpty()
{
  return def_set.size() == 0;
}

bool DefUseMemObjInfo::isUseSetEmpty()
{
  return use_set.size() == 0;
}

void DefUseMemObjInfo::copyDefToUseSet()
{
  use_set.insert(def_set.begin(), def_set.end());
}

void DefUseMemObjInfo::copyUseToDefSet()
{
  def_set.insert(use_set.begin(), use_set.end());
}

DefUseMemObjInfo DefUseMemObjInfo::operator+(const DefUseMemObjInfo& dumo1)
{
  const VariableIdSet& d1_def_set = dumo1.getDefSetRef();
  const VariableIdSet& d1_use_set = dumo1.getUseSetRef();
  VariableIdSet rdef_set, ruse_set;
  set_union(def_set, d1_def_set, rdef_set);
  set_union(use_set, d1_use_set, ruse_set);
  return DefUseMemObjInfo(rdef_set, ruse_set);  
}

std::string DefUseMemObjInfo::str()
{
  std::ostringstream oss;
  oss << "def_set:<" << VariableIdSetPrettyPrint::str(def_set) << ">\n";
  oss << "use_set:<" << VariableIdSetPrettyPrint::str(use_set) << ">\n";
  return oss.str();
}

std::string DefUseMemObjInfo::str(VariableIdMapping& vidm)
{
  std::ostringstream oss;
  oss << "def_set:<" << VariableIdSetPrettyPrint::str(def_set, vidm) << ">\n";
  oss << "use_set:<" << VariableIdSetPrettyPrint::str(use_set, vidm) << ">\n";
  return oss.str();
}


ExprVisitorPattern::ExprVisitorPattern(FlowInsensitivePointerInfo& _fipi, bool _isModExpr) :
  fipi(_fipi), isModExpr(_isModExpr)
{
  // default constructor
}

void ExprVisitorPattern::visit(SgAssignOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo = getDefUseMemObjInfo_rec(lhs, fipi, true);
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(rhs, fipi, false);
  // if the rhs writes to a memory (i.e sideffect)
  // add to the def_set to be unioned in next step
  if(!rdumo.isDefSetEmpty())
  {
    rdumo.copyDefToUseSet();
  }
  // union ldumo and rdumo
  dumo = ldumo + rdumo;
}

void ExprVisitorPattern::visit(SgCompoundAssignOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo = getDefUseMemObjInfo_rec(lhs, fipi, true);
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(rhs, fipi, false);
  // if the rhs writes to a memory (i.e side-effect)
  // add to the def_set to be unioned later
  if(!rdumo.isDefSetEmpty())
  {
    rdumo.copyDefToUseSet();
  }
  // union ldumo and rdumo
  dumo = ldumo + rdumo;
}

void ExprVisitorPattern::visit(SgCastExp* sgn)
{
  SgNode* operand = sgn->get_operand();
  DefUseMemObjInfo opdumo = getDefUseMemObjInfo_rec(operand, fipi, false);
  dumo = opdumo;
}

void ExprVisitorPattern::visit(SgAddressOfOp* sgn)
{
  SgNode* operand = sgn->get_operand();
  DefUseMemObjInfo opdumo = getDefUseMemObjInfo_rec(operand, fipi, false);
  dumo = opdumo;
}

void ExprVisitorPattern::visit(SgBinaryOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo = getDefUseMemObjInfo_rec(lhs, fipi, false);
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(rhs, fipi, false);
  // both operands are uses
  // if they write to any memory location as side-effect
  // copy the defs to uses
  if(!ldumo.isDefSetEmpty())
  {
    ldumo.copyDefToUseSet();
  }
  if(!rdumo.isDefSetEmpty())
  {
    rdumo.copyDefToUseSet();
  }
  // union ldumo and rdumo
  dumo = ldumo + rdumo;
}

void ExprVisitorPattern::visit(SgVarRefExp* sgn)
{
  // recursion base case
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, fipi, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgPntrArrRefExp* sgn)
{
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, fipi, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgPointerDerefExp* sgn)
{
  // *p + i++ ??
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, fipi, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgArrowExp* sgn)
{
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, fipi, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgDotExp *sgn)
{
  DefUseMemObjInfo rdumo = getDefUseMemObjInfoLvalue(sgn, fipi, isModExpr);
  dumo = rdumo;
}

DefUseMemObjInfo ExprVisitorPattern::getDefUseMemObjInfo()
{
  return dumo;
}

LvalueVisitorPattern::LvalueVisitorPattern(FlowInsensitivePointerInfo& _fipi, VariableIdMapping& _vidm, bool _isModExpr)
  : fipi(_fipi), vidm(_vidm), isModExpr(_isModExpr)
{
}

void LvalueVisitorPattern::visit(SgVarRefExp* sgn)
{
  VariableIdSet& def_set = dumo.getDefSetRefMod();
  VariableIdSet& use_set = dumo.getUseSetRefMod();
  // insert into def_set if on lhs
  if(isModExpr)
  {
    def_set.insert(vidm.variableId(sgn));
  }
  // insert into use_set otherwise
  else
  {
    use_set.insert(vidm.variableId(sgn));
  }
}

void LvalueVisitorPattern::visit(SgPointerDerefExp* sgn)
{
  VariableIdSet& def_set = dumo.getDefSetRefMod();
  VariableIdSet& use_set = dumo.getUseSetRefMod();
  VariableIdSet modbyptr = fipi.getMemModByPointer();

  // process the operand recursively
  // to find out the used/def
  SgNode* operand = sgn->get_operand();
  DefUseMemObjInfo rdumo = getDefUseMemObjInfo_rec(operand, fipi, false);

  if(!rdumo.isDefSetEmpty())
    rdumo.copyDefToUseSet();

  // now insert the objects that can be
  // accessed by pointer to def/use set
  // inserting these sets multiple times
  // should handle this cleanly
  if(isModExpr)
  {
    def_set.insert(modbyptr.begin(), modbyptr.end());
  }
  else
  {
    use_set.insert(modbyptr.begin(), modbyptr.end());
  }
  // update the results
  dumo = rdumo + dumo;
}

void LvalueVisitorPattern::visit(SgPntrArrRefExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand();
  SgNode* rhs_expr = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo, rdumo;
  if(isModExpr)
  {
    ldumo = getDefUseMemObjInfo_rec(lhs_addr, fipi, true);
  }
  else
  {
    ldumo = getDefUseMemObjInfo_rec(lhs_addr, fipi, false); 
  }
  rdumo = getDefUseMemObjInfo_rec(rhs_expr, fipi, false);
  // if we have side-effects copy them over
  if(!rdumo.isDefSetEmpty())
    rdumo.copyDefToUseSet();
  // update the values
  dumo = ldumo + rdumo;
}

void LvalueVisitorPattern::visit(SgArrowExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand();
  SgNode* rhs_expr = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo, rdumo;
  // only right op is modified
  if(isModExpr)
  {
    rdumo = getDefUseMemObjInfo_rec(rhs_expr, fipi, true);
  }
  else
  {
    rdumo = getDefUseMemObjInfo_rec(rhs_expr, fipi, false); 
  }
  // left is only used
  ldumo = getDefUseMemObjInfo_rec(lhs_addr, fipi, false);

  // if we have side-effects from left, copy them
  if(!ldumo.isDefSetEmpty())
    ldumo.copyDefToUseSet();

  // update the values
  dumo = ldumo + rdumo;
}

void LvalueVisitorPattern::visit(SgDotExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand();
  SgNode* rhs_expr = sgn->get_rhs_operand();
  DefUseMemObjInfo ldumo, rdumo;
  // only right op is modified
  if(isModExpr)
  {
    rdumo = getDefUseMemObjInfo_rec(rhs_expr, fipi, true);
  }
  else
  {
    rdumo = getDefUseMemObjInfo_rec(rhs_expr, fipi, false); 
  }
  // left is only used
  ldumo = getDefUseMemObjInfo_rec(lhs_addr, fipi, false);

  // if we have side-effects from left, copy them
  if(!ldumo.isDefSetEmpty())
    ldumo.copyDefToUseSet();

  // update the values
  dumo = ldumo + rdumo;
}

DefUseMemObjInfo LvalueVisitorPattern::getDefUseMemObjInfo()
{
  return dumo;
}

// interface function
DefUseMemObjInfo getDefUseMemObjInfo(SgNode* sgn, FlowInsensitivePointerInfo& fipi)
{
  return getDefUseMemObjInfo_rec(sgn, fipi, false);  
}

// main implementation
DefUseMemObjInfo getDefUseMemObjInfo_rec(SgNode* sgn, FlowInsensitivePointerInfo& fipi, bool isModExpr)
{
  ExprVisitorPattern expvp(fipi, isModExpr);
  sgn->accept(expvp);
  return expvp.getDefUseMemObjInfo();
}

DefUseMemObjInfo getDefUseMemObjInfoLvalue(SgNode* sgn, FlowInsensitivePointerInfo& fipi, bool isModExpr)
{
  LvalueVisitorPattern lvalvp(fipi, fipi.getVariableIdMapping(), isModExpr);
  sgn->accept(lvalvp);
  return lvalvp.getDefUseMemObjInfo();
}
