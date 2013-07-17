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


ExprVisitorPattern::ExprVisitorPattern(FlowInsensitivePointerAnalysis& _fipa, DefUseMemObj _dumo, bool _isModExpr) :
  fipa(_fipa), dumo(_dumo), isModExpr(_isModExpr)
{
  // default constructor
}

void ExprVisitorPattern::visit(SgAssignOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseMemObj ldumo = getDefUseMemObj_rec(lhs, fipa, dumo, true);
  DefUseMemObj rdumo = getDefUseMemObj_rec(rhs, fipa, dumo, false);
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
  DefUseMemObj ldumo = getDefUseMemObj_rec(lhs, fipa, dumo, true);
  DefUseMemObj rdumo = getDefUseMemObj_rec(rhs, fipa, dumo, false);
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
  DefUseMemObj opdumo = getDefUseMemObj_rec(operand, fipa, dumo, false);
  dumo = opdumo;
}

void ExprVisitorPattern::visit(SgBinaryOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseMemObj ldumo = getDefUseMemObj_rec(lhs, fipa, dumo, false);
  DefUseMemObj rdumo = getDefUseMemObj_rec(rhs, fipa, dumo, false);
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
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, dumo, isModExpr);
  dumo = dumo + rdumo;
}

void ExprVisitorPattern::visit(SgPntrArrRefExp* sgn)
{
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, dumo, isModExpr);
  // if there are side-effects copy them to use
  if(!rdumo.isDefSetEmpty())
  {
    rdumo.copyDefToUseSet();
  }
  dumo = dumo + rdumo;
}

void ExprVisitorPattern::visit(SgPointerDerefExp* sgn)
{
  // *p + i++ ??
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, dumo, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgArrowExp* sgn)
{
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, dumo, isModExpr);
  dumo = rdumo;
}

void ExprVisitorPattern::visit(SgDotExp *sgn)
{
  DefUseMemObj rdumo = getDefUseMemObjLvalue(sgn, fipa, dumo, isModExpr);
  dumo = rdumo;
}

DefUseMemObj ExprVisitorPattern::getDefUseMemObj()
{
  return dumo;
}

LvalueVisitorPattern::LvalueVisitorPattern(FlowInsensitivePointerAnalysis& _fipa, VariableIdMapping& _vidm, DefUseMemObj _dumo, bool _isModExpr)
  : fipa(_fipa), vidm(_vidm), dumo(_dumo), isModExpr(_isModExpr)
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

  if(isModExpr)
  {
    def_set.insert(modbyptr.begin(), modbyptr.end());
  }
  else
  {
    use_set.insert(modbyptr.begin(), modbyptr.end());
    DefUseMemObj rdumo = getDefUseMemObj_rec(sgn->get_operand(), fipa, dumo, false);
    // if there are side-effects copy them over
    if(!rdumo.isDefSetEmpty())
      rdumo.copyDefToUseSet();
    dumo = dumo + rdumo;
  }
}

void LvalueVisitorPattern::visit(SgPntrArrRefExp* sgn)
{
  ROSE_ASSERT(0);
}

void LvalueVisitorPattern::visit(SgArrowExp* sgn)
{
  ROSE_ASSERT(0);
}

void LvalueVisitorPattern::visit(SgDotExp* sgn)
{
  ROSE_ASSERT(0);
}

DefUseMemObj LvalueVisitorPattern::getDefUseMemObj()
{
  return dumo;
}

// interface function
DefUseMemObj getDefUseMemObj(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa)
{
  return getDefUseMemObj_rec(sgn, fipa, DefUseMemObj(), false);  
}

// main implementation
DefUseMemObj getDefUseMemObj_rec(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa, DefUseMemObj dumo, bool isModExpr)
{
  ExprVisitorPattern expvp(fipa, dumo, isModExpr);
  sgn->accept(expvp);
  return expvp.getDefUseMemObj();
}

DefUseMemObj getDefUseMemObjLvalue(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa, DefUseMemObj dumo, bool isModExpr)
{
  LvalueVisitorPattern lvalvp(fipa, fipa.getVariableIdMapping(), dumo, isModExpr);
  sgn->accept(lvalvp);
  return lvalvp.getDefUseMemObj();
}
