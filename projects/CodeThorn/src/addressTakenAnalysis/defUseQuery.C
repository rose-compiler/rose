/*****************************************
 * Author: Sriram Aananthakrishnan, 2013 *
 *****************************************/

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
  
  // we are merging two maps
  // <VariableId, type> entry should not be different if present in two maps
  // maps are really small and therefore its ok to
  // insert the map into another and there will should be no collision on the item for the same key

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
    oss << "<" << vidm.uniqueShortVariableName((*it).first) << ", ";
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

void ExprWalker::visit(SgMinusMinusOp* sgn)
{
  DefUseVarsInfo uduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  // all the side-effects and the variable are also used by this expression
  uduvi.copyUseToDef();
  duvi = uduvi;
}

void ExprWalker::visit(SgPlusPlusOp* sgn)
{
  DefUseVarsInfo uduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  // all the side-effects are also used by this expression
  uduvi.copyUseToDef();
  duvi = uduvi;
}

void ExprWalker::visitSgUnaryOpNoMod(SgUnaryOp* sgn)
{
   // its only used
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();
  duvi = rduvi;
}

void ExprWalker::visit(SgCastExp* sgn)
{
  // cast has modifying semantics?
  visitSgUnaryOpNoMod(sgn);
}

void ExprWalker::visit(SgAddressOfOp* sgn)
{
  visitSgUnaryOpNoMod(sgn);
}

void ExprWalker::visit(SgMinusOp* sgn)
{
  visitSgUnaryOpNoMod(sgn);
}

void ExprWalker::visit(SgUnaryAddOp *sgn)
{
  visitSgUnaryOpNoMod(sgn);
}

void ExprWalker::visit(SgNotOp* sgn)
{
  visitSgUnaryOpNoMod(sgn);
}

void ExprWalker::visit(SgBitComplementOp* sgn)
{
  visitSgUnaryOpNoMod(sgn);
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

void ExprWalker::visit(SgAggregateInitializer* sgn)
{
  SgExprListExp* initializers = sgn->get_initializers();
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(initializers, vidm, false);
  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();
  duvi = rduvi;
}

void ExprWalker::visit(SgVarRefExp* sgn)
{
  // get the VariableId
  VariableId vid = vidm.variableId(sgn);
  
  ROSE_ASSERT(vid.getIdCode() != -1);

  // determine type info
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

  VarsInfo& def_vars_info = duvi.getDefVarsInfoMod();
  VarsInfo& use_vars_info = duvi.getUseVarsInfoMod();

  if(isModExpr) {
    def_vars_info.first.insert(VariableIdInfo(vid, sgn_type_info));
  }
  else {
    use_vars_info.first.insert(VariableIdInfo(vid, sgn_type_info));
  }
}

void ExprWalker::visit(SgPntrArrRefExp* sgn)
{
  SgNode* lhs_addr = sgn->get_lhs_operand(); // get the address computation expr of the array
  SgNode* rhs_expr = sgn->get_rhs_operand(); // get the index expression
  DefUseVarsInfo lduvi, rduvi;

  if(isModExpr) {
    // check for the type of address computation expr
    // if p is pointer type in p[expr]
    // raise the flag (unknown location)
    if(isSgVarRefExp(lhs_addr)) {
      SgType* lhs_type_info = isSgVarRefExp(lhs_addr)->get_type();           
      if(isSgPointerType(lhs_type_info)) {        
        lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, false);
        VarsInfo& ldef_vars_info = lduvi.getDefVarsInfoMod();
        ldef_vars_info.second = true;
      }
      else {
        // lhs_address is a SgVarRefExp of array type
        lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, true);
      }
    }
    // for multi-dimensional pointers
    // recurse and collect info
    else if(isSgPntrArrRefExp(lhs_addr)) {
      lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, true);
    }
    else { 
      // otherwise some arithmetic involved to calculate
      // the address of the array
      // for example: (true? a_ptr1 = arrA : a_ptr1 = arrB)[2] = 2;
      // raise the flag and collect with use semantics
      lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, false);
      lduvi.getDefVarsInfoMod().second = true;
      // copy side-effects
      if(!lduvi.isDefSetEmpty())
        lduvi.copyDefToUse();      
    }
  } 
  else { // isModExpr = false
    lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, false); 
    if(!lduvi.isDefSetEmpty())
      lduvi.copyDefToUse();
  }

  // rhs_op of the array is always used
  rduvi = getDefUseVarsInfo_rec(rhs_expr, vidm, false);
  // if we have side-effects copy them over
  if(!rduvi.isDefSetEmpty())
    rduvi.copyDefToUse();
  // update the values
  duvi = lduvi + rduvi;
}

void ExprWalker::visit(SgPointerDerefExp* sgn)
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

void ExprWalker::visit(SgArrowExp* sgn)
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

void ExprWalker::visit(SgDotExp* sgn)
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

void ExprWalker::visit(SgInitializedName* sgn)
{
  VariableId vid = vidm.variableId(sgn);
  
  // some SgInitializedName do not have symbols
  // VariableId is not created for such SgInitializedName
  // check and return if we are processing such SgInitializedName
  if(vid.getIdCode() == -1) {
    cerr << "WARNING: Skipping SgInitializedName sgn->get_name() = " << sgn->get_name() << " with no symbol\n";
    return;
  }

  DefUseVarsInfo rduvi;
  SgInitializer* initializer = sgn->get_initializer();
  if(initializer) {
    rduvi = getDefUseVarsInfo_rec(initializer, vidm, false);
  }

  // SgInitializedName always define a variable
  // it should always be in def_set
  VarsInfo& def_vars_info = duvi.getDefVarsInfoMod();
  
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

  def_vars_info.first.insert(VariableIdInfo(vid, sgn_type_info));

  duvi = duvi + rduvi;
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

void ExprWalker::visit(SgThisExp* sgn)
{
  // we dont know anything about the 'this' exp
  // 'this' cannot be modified
  // it will always be used
  // if we construct VariableId, we can add it to use set
  try {
    std::ostringstream oss;
    oss << "WARNING: Skipping VariableId for 'this' expr\n";
    throw oss.str();
  }
  catch(std::string excep) {
    std::cerr << excep;
  }
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
