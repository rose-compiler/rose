#include "defUseQuery.h"

/*************************************************
 ***************** DefUseMemObj ******************
 *************************************************/

DefUseMemObj::DefUseMemObj(const VariableIdSet& _def_set, const VariableIdSet& _use_set) :
  def_set(_def_set), use_set(_use_set)
{
  ptr_modify = false;
  func_modify = false;
}

VariableIdSet DefUseMemObj::getDefSet()
{
  return def_set;
}

VariableIdSet DefUseMemObj::getUseSet()
{
  return use_set;
}

const VariableIdSet& DefUseMemObj::getDefSetRef() const
{
  return def_set;
}

const VariableIdSet& DefUseMemObj::getUseSetRef() const
{
  return use_set;
}

VariableIdSet& DefUseMemObj::getDefSetRefMod()
{
  return def_set;
}

VariableIdSet& DefUseMemObj::getUseSetRefMod()
{
  return use_set;
}

bool DefUseMemObj::isModByPointer()
{
  return ptr_modify;
}

bool DefUseMemObj::isModByFunction()
{
  return func_modify;
}

bool DefUseMemObj::isDefSetEmpty()
{
  return def_set.size() == 0;
}

bool DefUseMemObj::isUseSetEmpty()
{
  return use_set.size() == 0;
}

void DefUseMemObj::copyDefToUseSet()
{
  use_set.insert(def_set.begin(), def_set.end());
}

void DefUseMemObj::copyUseToDefSet()
{
  def_set.insert(use_set.begin(), use_set.end());
}

DefUseMemObj DefUseMemObj::operator+(const DefUseMemObj& dumo1)
{
  const VariableIdSet& d1_def_set = dumo1.getDefSetRef();
  const VariableIdSet& d1_use_set = dumo1.getUseSetRef();
  VariableIdSet rdef_set, ruse_set;
  set_union(def_set, d1_def_set, rdef_set);
  set_union(use_set, d1_use_set, ruse_set);
  return DefUseMemObj(rdef_set, ruse_set);  
}

std::string DefUseMemObj::str()
{
  std::ostringstream oss;
  oss << "def_set:<" << VariableIdSetPrettyPrint::str(def_set) << ">\n";
  oss << "use_set:<" << VariableIdSetPrettyPrint::str(use_set) << ">\n";
  return oss.str();
}

std::string DefUseMemObj::str(VariableIdMapping& vidm)
{
  std::ostringstream oss;
  oss << "def_set:<" << VariableIdSetPrettyPrint::str(def_set, vidm) << ">\n";
  oss << "use_set:<" << VariableIdSetPrettyPrint::str(use_set, vidm) << ">\n";
  return oss.str();
}


ExprVisitorPattern::ExprVisitorPattern(FlowInsensitivePointerAnalysis& _fipa, bool _isModExpr) :
  fipa(_fipa), isModExpr(_isModExpr)
{
  // default constructor
}

void ExprVisitorPattern::visit(SgAssignOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseMemObj ldumo = getDefUseMemObj_rec(lhs, fipa, true);
  DefUseMemObj rdumo = getDefUseMemObj_rec(rhs, fipa, false);
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
  DefUseMemObj ldumo = getDefUseMemObj_rec(lhs, fipa, true);
  DefUseMemObj rdumo = getDefUseMemObj_rec(rhs, fipa, false);
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
  DefUseMemObj opdumo = getDefUseMemObj_rec(operand, fipa, false);
  dumo = opdumo;
}

void ExprVisitorPattern::visit(SgAddressOfOp* sgn)
{
  SgNode* operand = sgn->get_operand();
  DefUseMemObj opdumo = getDefUseMemObj_rec(operand, fipa, false);
  dumo = opdumo;
}

void ExprVisitorPattern::visit(SgBinaryOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseMemObj ldumo = getDefUseMemObj_rec(lhs, fipa, false);
  DefUseMemObj rdumo = getDefUseMemObj_rec(rhs, fipa, false);
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
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgPntrArrRefExp* sgn)
{
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgPointerDerefExp* sgn)
{
  // *p + i++ ??
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgArrowExp* sgn)
{
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgDotExp *sgn)
{
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, isModExpr);
  dumo = rdumo;
}

DefUseMemObj ExprVisitorPattern::getDefUseMemObj()
{
  return dumo;
}

LvalueVisitorPattern::LvalueVisitorPattern(FlowInsensitivePointerAnalysis& _fipa, VariableIdMapping& _vidm, bool _isModExpr)
  : fipa(_fipa), vidm(_vidm), isModExpr(_isModExpr)
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
  VariableIdSet modbyptr = fipa.getMemModByPointer();

  // process the operand recursively
  // to find out the used/def
  SgNode* operand = sgn->get_operand();
  DefUseMemObj rdumo = getDefUseMemObj_rec(operand, fipa, false);

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
  DefUseMemObj ldumo, rdumo;
  if(isModExpr)
  {
    ldumo = getDefUseMemObj_rec(lhs_addr, fipa, true);
  }
  else
  {
    ldumo = getDefUseMemObj_rec(lhs_addr, fipa, false); 
  }
  rdumo = getDefUseMemObj_rec(rhs_expr, fipa, false);
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
  DefUseMemObj ldumo, rdumo;
  // only right op is modified
  if(isModExpr)
  {
    rdumo = getDefUseMemObj_rec(rhs_expr, fipa, true);
  }
  else
  {
    rdumo = getDefUseMemObj_rec(rhs_expr, fipa, false); 
  }
  // left is only used
  ldumo = getDefUseMemObj_rec(lhs_addr, fipa, false);

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
  DefUseMemObj ldumo, rdumo;
  // only right op is modified
  if(isModExpr)
  {
    rdumo = getDefUseMemObj_rec(rhs_expr, fipa, true);
  }
  else
  {
    rdumo = getDefUseMemObj_rec(rhs_expr, fipa, false); 
  }
  // left is only used
  ldumo = getDefUseMemObj_rec(lhs_addr, fipa, false);

  // if we have side-effects from left, copy them
  if(!ldumo.isDefSetEmpty())
    ldumo.copyDefToUseSet();

  // update the values
  dumo = ldumo + rdumo;
}

DefUseMemObj LvalueVisitorPattern::getDefUseMemObj()
{
  return dumo;
}

// interface function
DefUseMemObj getDefUseMemObj(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa)
{
  return getDefUseMemObj_rec(sgn, fipa, false);  
}

// main implementation
DefUseMemObj getDefUseMemObj_rec(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa, bool isModExpr)
{
  ExprVisitorPattern expvp(fipa, isModExpr);
  sgn->accept(expvp);
  return expvp.getDefUseMemObj();
}

DefUseMemObj getDefUseMemObjLvalue(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa, bool isModExpr)
{
  LvalueVisitorPattern lvalvp(fipa, fipa.getVariableIdMapping(), isModExpr);
  sgn->accept(lvalvp);
  return lvalvp.getDefUseMemObj();
}
