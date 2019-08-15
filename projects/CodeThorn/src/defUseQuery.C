/*****************************************
 * Author: Sriram Aananthakrishnan, 2013 *
 *****************************************/

#include "sage3basic.h"
#include "defUseQuery.h"
#include <algorithm>
#include <iterator>
#include <set>

using namespace CodeThorn;
using namespace CodeThorn;
using namespace std;

/*********************
 * utility functions *
 *********************/

VariableIdTypeInfo getVariableIdTypeInfo(VariableId vid, VariableIdMapping& vidm)
{
  SgSymbol* symb = vidm.getSymbol(vid); ROSE_ASSERT(symb);
  SgType* sgn_type = symb->get_type();
  VariableIdTypeInfo sgn_type_info;

  if(isSgArrayType(sgn_type))
    sgn_type_info = arrayType;
  else if(isSgPointerType(sgn_type))
    sgn_type_info = pointerType;
  else if(isSgReferenceType(sgn_type))
    sgn_type_info = referenceType;
  else if(isSgClassType(sgn_type))
    sgn_type_info = classType;
  else
    sgn_type_info = variableType;

  return sgn_type_info;
}

std::string variableIdTypeInfoToString(VariableIdTypeInfo vid_type_info)
{
  std::ostringstream oss;
  switch(vid_type_info) {
  case variableType:
    oss << "var";
    break;
  case arrayType:
    oss << "array";
    break;
  case pointerType:
    oss << "ptr";
    break;
  case classType:
    oss << "class";
    break;
  case referenceType:
    oss << "ref";
    break;
  }
  return oss.str();
}

/******************
 * DefUseVarsInfo *
 ******************/

void DefUseVarsInfo::copyUseToDef()
{
  def_vars_info.first.insert(use_vars_info.first.begin(), use_vars_info.first.end());
  def_vars_info.second = def_vars_info.second || use_vars_info.second;
}

// combine the two DefUseVarsInfo functions
DefUseVarsInfo operator+(const DefUseVarsInfo& duvi1, const DefUseVarsInfo& duvi2)
{  
  const VarsInfo& duvi1DefVarsInfo = duvi1.getDefVarsInfoRef();
  const VariableIdInfoMap& duvi1DefVarsInfoMap = duvi1DefVarsInfo.first;

  const VarsInfo& duvi1UseVarsInfo = duvi1.getUseVarsInfoRef();
  const VariableIdInfoMap& duvi1UseVarsInfoMap = duvi1UseVarsInfo.first;

  const FunctionCallExpSet& duvi1FuncSet = duvi1.getFunctionCallExpSetRef();

  const VarsInfo& duvi2DefVarsInfo = duvi2.getDefVarsInfoRef();
  const VariableIdInfoMap& duvi2DefVarsInfoMap = duvi2DefVarsInfo.first;

  const VarsInfo& duvi2UseVarsInfo = duvi2.getUseVarsInfoRef();
  const VariableIdInfoMap& duvi2UseVarsInfoMap = duvi2UseVarsInfo.first;

  const FunctionCallExpSet& duvi2FuncSet = duvi2.getFunctionCallExpSetRef();

  VarsInfo rduviDefVarsInfo, rduviUseVarsInfo;
  VariableIdInfoMap& rduviDefVarsInfoMap = rduviDefVarsInfo.first;
  VariableIdInfoMap& rduviUseVarsInfoMap = rduviUseVarsInfo.first;
  FunctionCallExpSet rduviFuncSet;
  
  // we are merging two maps
  // <VariableId, type> entry should not be different if present in two maps
  // maps are really small and therefore its ok to
  // insert the map into another and there will should be no collision on the item for the same key

  rduviDefVarsInfoMap.insert(duvi1DefVarsInfoMap.begin(), duvi1DefVarsInfoMap.end());
  rduviDefVarsInfoMap.insert(duvi2DefVarsInfoMap.begin(), duvi2DefVarsInfoMap.end());

  rduviUseVarsInfoMap.insert(duvi1UseVarsInfoMap.begin(), duvi1UseVarsInfoMap.end());
  rduviUseVarsInfoMap.insert(duvi2UseVarsInfoMap.begin(), duvi2UseVarsInfoMap.end());
  
  set_union(duvi1FuncSet.begin(), duvi1FuncSet.end(),
            duvi2FuncSet.begin(), duvi2FuncSet.end(),
            std::inserter(rduviFuncSet, rduviFuncSet.begin()));

  rduviDefVarsInfo.second = duvi1DefVarsInfo.second || duvi2DefVarsInfo.second;
  rduviUseVarsInfo.second = duvi1UseVarsInfo.second || duvi2UseVarsInfo.second;
 
  bool rduviUseAfterDef = duvi1.getUseAfterDef() || duvi2.getUseAfterDef();

  return DefUseVarsInfo(rduviDefVarsInfo, rduviUseVarsInfo, rduviFuncSet, rduviUseAfterDef);
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
    oss << "<" << vidm.uniqueVariableName((*it).first) << ", " << variableIdTypeInfoToString((*it).second);
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
  oss << "side-effect: " << (useAfterDef? "true" : "false") << "\n";
  oss << "def_vars_info: " << varsInfoPrettyPrint(def_vars_info, vidm) << "\n";
  oss << "use_vars_info: " << varsInfoPrettyPrint(use_vars_info, vidm) << "\n";
  oss << "func_set: " << functionCallExpSetPrettyPrint(func_set) << "\n";
  return oss.str();
}

void DefUseVarsInfo::addAllArrayElements(SgInitializedName* array_name, VariableIdMapping& vidm, bool def)
{
  assert(array_name);
  VariableId array_var_id = vidm.variableId(array_name);
  SgArrayType* array_type = isSgArrayType(array_name->get_type());
  if (!array_type)
    return;
  int elements = vidm.getArrayElementCount(array_type);
  if (!elements)
    elements = vidm.getArrayDimensionsFromInitializer(isSgAggregateInitializer(array_name->get_initializer()));
  VarsInfo& info = (def ? def_vars_info : use_vars_info);
  VariableIdTypeInfo sgn_type_info_elem = getVariableIdTypeInfo(array_var_id, vidm);
  for (int e = 0; e < elements; e++) {
    cout<<"Error: DefUseVarsInfo::addAllArrayElements: outdated use of array element ids. Needs to be updated to abstract values."<<endl;
    exit(1);
    VariableId element_id = vidm.variableIdOfArrayElement(array_var_id, e);
    info.first.insert(VariableIdInfo(element_id, sgn_type_info_elem));
  }
}

/**************
 * ExprWalker *
 **************/

ExprWalker::ExprWalker(VariableIdMapping& _vidm, bool _isModExpr) :
  vidm(_vidm), isModExpr(_isModExpr)
{
  // default constructor
}

/*****************************************************
 * ExprWalker for SgUnaryOp with modifying semantics *
 *****************************************************/

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

/*********************************************************
 * ExprWalker for SgUnaryOp with non-modifying semantics *
 *********************************************************/

void ExprWalker::visitSgUnaryOpNoMod(SgUnaryOp* sgn)
{
   // its only used
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
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

void ExprWalker::visit(SgThrowOp* sgn)
{
  SgNode* operand = sgn->get_operand();
  DefUseVarsInfo rduvi;
  // operand can be empty
  if(operand) {
    rduvi = getDefUseVarsInfo_rec(operand, vidm, false);
    if(rduvi.isUseAfterDefCandidate()) {
      rduvi.setUseAfterDef();
    }
  }
  duvi = rduvi;
}

/******************************************************
 * ExprWalker for SgBinaryOp with modifying semantics *
 ******************************************************/

void ExprWalker::visit(SgAssignOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi = getDefUseVarsInfo_rec(lhs, vidm, true);
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(rhs, vidm, false);
  // if the rhs writes to a memory (i.e sideffect)
  // raise the flag
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  // union lduvi and rduvi
  duvi = lduvi + rduvi;
}

void ExprWalker::visit(SgCompoundAssignOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  // schroder3 (2016-08-22): Added second traversal of lhs because the lhs is
  //  read/ used AND written/ defd:
  DefUseVarsInfo lduviMod = getDefUseVarsInfo_rec(lhs, vidm, true);
  DefUseVarsInfo lduviNonMod = getDefUseVarsInfo_rec(lhs, vidm, false);
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(rhs, vidm, false);
  // if the rhs writes to a memory (i.e side-effect)
  // raise the side-effect flag
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  // union lduvi and rduvi
  duvi = lduviMod + lduviNonMod + rduvi;
}

/*************************************************
 * ExprWalker for SgBinaryOp that can be lvalues *
 *************************************************/

void ExprWalker::visit(SgPntrArrRefExp* ref)
{
  SgExpression* lhs_addr = ref->get_lhs_operand(); // get the address computation expr of the array
  SgExpression* rhs_expr = ref->get_rhs_operand(); // get the index expression

  if (vidm.getModeVariableIdForEachArrayElement()) {
    // Handle array elements specifically.
    // Used in RD analysis.
    SgExpression* array_expr;
    SageInterface::isArrayReference(ref, &array_expr);
    SgVarRefExp* array_var = isSgVarRefExp(array_expr);
    VariableId array_reference = vidm.idForArrayRef(ref);
    if (array_reference.isValid()) {
      // We know which element is referenced,
      // mark that element.
      VariableIdTypeInfo sgn_type_info_elem = getVariableIdTypeInfo(array_reference, vidm);
      VarsInfo& def_vars_info = duvi.getDefVarsInfoMod();
      VarsInfo& use_vars_info = duvi.getUseVarsInfoMod();

      if (isModExpr) {
        def_vars_info.first.insert(VariableIdInfo(array_reference, sgn_type_info_elem));
      } else {
        use_vars_info.first.insert(VariableIdInfo(array_reference, sgn_type_info_elem));
      }
    } else {
      // An unknown element is referenced,
      // mark all array elements.
      duvi.addAllArrayElements(SageInterface::convertRefToInitializedName(array_var), vidm, isModExpr);
    }
    return;
  }

  DefUseVarsInfo lduvi, rduvi;
  // check for the type of address computation expr
  // if p is pointer type in p[expr]
  // raise the flag (unknown location)
  SgType* lhs_type_info = lhs_addr->get_type();
  // process with use semantics only if the address computation
  // expression is of SgArrayType
  if(isSgArrayType(lhs_type_info)) {
    if(isModExpr) { // for def case
      lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, true);
    }
    else { // for the use case
      lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, false);
      // check for expression side-effect
      if(lduvi.isUseAfterDefCandidate()) {
        lduvi.setUseAfterDef();
      }
    }
  }
  else {
    // otherwise we dont know what exactly is modified
    // collect it with use semantics
    lduvi = getDefUseVarsInfo_rec(lhs_addr, vidm, false);
    // check for expression side-effect
    if(lduvi.isUseAfterDefCandidate()) {
      lduvi.setUseAfterDef();
    }
    // raise the appropriate flag
    // since the operand is no longer array
    if(isModExpr) {      
    lduvi.getDefVarsInfoMod().second = true;
    }
    else {
      lduvi.getUseVarsInfoMod().second = true;
    }
  }

  // rhs_op of the array is always used
  rduvi = getDefUseVarsInfo_rec(rhs_expr, vidm, false);
  // check for expression side-effects
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
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

  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }

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
  if(lduvi.isUseAfterDefCandidate()) {
    lduvi.setUseAfterDef();
  }

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
  if(lduvi.isUseAfterDefCandidate()) {
    lduvi.setUseAfterDef();
  }

  // update the values
  duvi = lduvi + rduvi;
}

// (a, b) = 10 is legal
// b is modified
// process first operand with no-mod semantics
// second operand is processed with mod semantics if the flag is true
// keep only the side-effects from first operand and drop the uses
// 
void ExprWalker::visit(SgCommaOpExp* sgn)
{
  SgNode* lhs_op = sgn->get_lhs_operand();
  SgNode* rhs_op = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi, rduvi;
  if(isModExpr) {
    lduvi = getDefUseVarsInfo_rec(lhs_op, vidm, false);
    rduvi = getDefUseVarsInfo_rec(rhs_op, vidm, true);
  }
  else {
    lduvi = getDefUseVarsInfo_rec(lhs_op, vidm, false);
    rduvi = getDefUseVarsInfo_rec(rhs_op, vidm, false);
    if(rduvi.isUseAfterDefCandidate()) {
      rduvi.setUseAfterDef();
    }
  }
  // first operand is always just evaluated
  // check for side-effect on the first operand
  if(lduvi.isUseAfterDefCandidate()) {
    lduvi.setUseAfterDef();
  }
  
  // (i=j+1, a) = expr
  // combine the lhs and rhs of SgCommaOp only 
  // if the lhs does something
  if(!lduvi.isDefSetEmpty()) {
    duvi = lduvi + rduvi;
  }
  // drop the lhs if it is not doing anything
  // (a, b, c, d, e) : only e is relevant
  // NOTE: should we collect a,b,c,d in use_vars_info ?
  else {
    duvi = rduvi;
  }
}

void ExprWalker::visit(SgDotStarOp* sgn)
{
  // dot star operator has implicit dereferencing semantics
  // flag is raised depending on whether we are on lhs or rhs
  // operands are collected with use semantics
  SgNode* lhs_op = sgn->get_lhs_operand();
  SgNode* rhs_op = sgn->get_rhs_operand();  
  DefUseVarsInfo lduvi, rduvi;
  lduvi = getDefUseVarsInfo_rec(lhs_op, vidm, false);
  rduvi = getDefUseVarsInfo_rec(rhs_op, vidm, false);
  if(lduvi.isUseAfterDefCandidate()) {
    lduvi.setUseAfterDef();
  }
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = lduvi + rduvi;
  
  // raise flag based on which side of SgAssignOp
  if(isModExpr) {
    VarsInfo& def_vars_info = duvi.getDefVarsInfoMod();
    def_vars_info.second = true;
  }
  else {
    VarsInfo& use_vars_info = duvi.getUseVarsInfoMod();
    use_vars_info.second = true;
  }   
}

void ExprWalker::visit(SgArrowStarOp* sgn)
{
  // arrow star operator has implicit dereferencing semantics
  // raise the flag on both def and use
  // flag is raised depending on whether we are on lhs or rhs
  // operands are collected with use semantics
  SgNode* lhs_op = sgn->get_lhs_operand();
  SgNode* rhs_op = sgn->get_rhs_operand();  
  DefUseVarsInfo lduvi, rduvi;
  lduvi = getDefUseVarsInfo_rec(lhs_op, vidm, false);
  rduvi = getDefUseVarsInfo_rec(rhs_op, vidm, false);
  if(lduvi.isUseAfterDefCandidate()) {
    lduvi.setUseAfterDef();
  }
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = lduvi + rduvi;
  
  // raise flag based on which side of SgAssignOp
  if(isModExpr) {
    VarsInfo& def_vars_info = duvi.getDefVarsInfoMod();
    def_vars_info.second = true;
  }
  else {
    VarsInfo& use_vars_info = duvi.getUseVarsInfoMod();
    use_vars_info.second = true;
  }   
}

/**********************************************************
 * ExprWalker for SgBinaryOp with non-modifying semantics *
 **********************************************************/

void ExprWalker::visitSgBinaryOpNoMod(SgBinaryOp* sgn)
{
  SgNode* lhs = sgn->get_lhs_operand();
  SgNode* rhs = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi = getDefUseVarsInfo_rec(lhs, vidm, false);
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(rhs, vidm, false);
  // both operands are uses
  // if they write to any memory location as side-effect
  // copy the defs to uses
  if(lduvi.isUseAfterDefCandidate()) {
    lduvi.setUseAfterDef();
  }
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  // union lduvi and rduvi
  duvi = lduvi + rduvi;
}

void ExprWalker::visit(SgAddOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgAndOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgBitAndOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgBitOrOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgBitXorOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgDivideOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgEqualityOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgGreaterOrEqualOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgGreaterThanOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgLessOrEqualOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgLessThanOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgLshiftOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgModOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgMultiplyOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgNotEqualOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgOrOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgRshiftOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

void ExprWalker::visit(SgSubtractOp* sgn)
{
  visitSgBinaryOpNoMod(sgn);
}

/****************************************
 * ExprWalker for expr that can lvalues *
 ****************************************/

void ExprWalker::visit(SgFunctionCallExp* sgn)
{
  // schroder3 (2016-08-22): Added traversal of arguments because the variables inside them
  //  are used.
  SgExpressionPtrList& arguments = sgn->get_args()->get_expressions();
  for(SgExpressionPtrList::const_iterator i = arguments.begin(); i != arguments.end(); ++i) {
    DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(*i, vidm, false);
    if(rduvi.isUseAfterDefCandidate()) {
      rduvi.setUseAfterDef();
    }
    duvi = duvi + rduvi;
  }

  FunctionCallExpSet& func_set = duvi.getFunctionCallExpSetMod();
  func_set.insert(sgn);
}

void ExprWalker::visit(SgConditionalExp* sgn)
{
  SgExpression* cond_exp = sgn->get_conditional_exp();
  SgExpression* true_exp = sgn->get_true_exp();
  SgExpression* false_exp = sgn->get_false_exp();

  DefUseVarsInfo cduvi = getDefUseVarsInfo_rec(cond_exp, vidm, false);
  if(cduvi.isUseAfterDefCandidate()) {
    cduvi.setUseAfterDef();
  }

  DefUseVarsInfo tduvi, fduvi;

  if(isModExpr) {
    tduvi = getDefUseVarsInfo_rec(true_exp, vidm, true);
    fduvi = getDefUseVarsInfo_rec(false_exp, vidm, true);
    
  }
  else {
    tduvi = getDefUseVarsInfo_rec(true_exp, vidm, false);
    fduvi = getDefUseVarsInfo_rec(false_exp, vidm, false);
    if(tduvi.isUseAfterDefCandidate()) {
      tduvi.setUseAfterDef();
    }
    if(fduvi.isUseAfterDefCandidate()) {
      fduvi.setUseAfterDef();
    }
  }
  duvi = cduvi + tduvi + fduvi;
}

/************************************************
 * ExprWalker for expr that can only be rvalues *
 ************************************************/

void ExprWalker::visit(SgExprListExp* sgn)
{
  SgExpressionPtrList expr_list = sgn->get_expressions();
  SgExpressionPtrList::iterator it = expr_list.begin();
  for( ; it != expr_list.end(); ++it)
  {
    // if they have side-effects we can copy them over
    DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(*it, vidm, false);
    if(rduvi.isUseAfterDefCandidate()) {
      rduvi.setUseAfterDef();
    }
    duvi = duvi + rduvi;
  }
}

void ExprWalker::visit(SgSizeOfOp* sgn)
{
  DefUseVarsInfo rduvi;
  // we only need to process if the operand is an expression
  SgExpression* expr = sgn->get_operand_expr();
  // expr can be null if the sizeof operand is a type
  if(expr) {
    rduvi = getDefUseVarsInfo_rec(expr, vidm, false);
    if(rduvi.isUseAfterDefCandidate()) {
      rduvi.setUseAfterDef();
    }
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgDeleteExp* sgn)
{
  DefUseVarsInfo rduvi;
  SgExpression* expr = sgn->get_variable();
  rduvi = getDefUseVarsInfo_rec(expr, vidm, false);
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgNewExp* sgn)
{
  DefUseVarsInfo pduvi, bduvi, cduvi;
  SgExprListExp* expr_list = sgn->get_placement_args();
  if(expr_list) {
    pduvi = getDefUseVarsInfo_rec(expr_list, vidm, false);
    if(pduvi.isUseAfterDefCandidate()) {
      pduvi.setUseAfterDef();
    }
  }

  SgConstructorInitializer* c_initializer = sgn->get_constructor_args();
  if(c_initializer) {
    cduvi = getDefUseVarsInfo_rec(c_initializer, vidm, false);
    if(cduvi.isUseAfterDefCandidate()) {
      cduvi.setUseAfterDef();
    }
  }

  SgExpression* builtin_args = sgn->get_builtin_args();
  if(builtin_args) {
    bduvi = getDefUseVarsInfo_rec(builtin_args, vidm, false);
    if(bduvi.isUseAfterDefCandidate()) {
      bduvi.setUseAfterDef();
    }
  }

  duvi = pduvi + cduvi + bduvi;
}

void ExprWalker::visit(SgTypeIdOp* sgn)
{
  DefUseVarsInfo rduvi;
  SgExpression* expr = sgn->get_operand_expr();
  // expr can be empty for types
  if(expr) {
    rduvi = getDefUseVarsInfo_rec(expr, vidm, false);
    if(rduvi.isUseAfterDefCandidate()) {
      rduvi.setUseAfterDef();
    }
    duvi = rduvi;
  }
}

void ExprWalker::visit(SgVarArgOp* sgn)
{
  SgNode* operand = sgn->get_operand_expr();
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(operand, vidm, false);
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgVarArgStartOp* sgn)
{
  SgNode* first_op = sgn->get_lhs_operand();
  SgNode* second_op = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi, rduvi;
  lduvi = getDefUseVarsInfo_rec(first_op, vidm, false);
  rduvi = getDefUseVarsInfo_rec(second_op, vidm, false);
  if(lduvi.isUseAfterDefCandidate()) {
    lduvi.setUseAfterDef();
  }
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = lduvi + rduvi;
}

void ExprWalker::visit(SgVarArgStartOneOperandOp* sgn)
{
  SgNode* operand = sgn->get_operand_expr();
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(operand, vidm, false);
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgVarArgEndOp* sgn)
{
  SgNode* operand = sgn->get_operand_expr();
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(operand, vidm, false);
  // not sure if assignment to arguments of t
  // these macros is possible
  // checking for side-effects anyway
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgVarArgCopyOp* sgn)
{
  SgNode* first_op = sgn->get_lhs_operand();
  SgNode* second_op = sgn->get_rhs_operand();
  DefUseVarsInfo lduvi, rduvi;
  lduvi = getDefUseVarsInfo_rec(first_op, vidm, false);
  rduvi = getDefUseVarsInfo_rec(second_op, vidm, false);
  if(lduvi.isUseAfterDefCandidate()) {
    lduvi.setUseAfterDef();
  }
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = lduvi + rduvi;
}

/***************************************************
 * ExprWalker for different intializer expressions *
 ***************************************************/

void ExprWalker::visit(SgAssignInitializer *sgn)
{
  // operand is only used
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(sgn->get_operand(), vidm, false);
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgConstructorInitializer *sgn)
{
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(sgn->get_args(), vidm, false);
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgAggregateInitializer* sgn)
{
  SgExprListExp* initializers = sgn->get_initializers();
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(initializers, vidm, false);
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgCompoundInitializer* sgn)
{
  SgExprListExp* initializers = sgn->get_initializers();
  DefUseVarsInfo rduvi = getDefUseVarsInfo_rec(initializers, vidm, false);
  if(rduvi.isUseAfterDefCandidate()) {
    rduvi.setUseAfterDef();
  }
  duvi = rduvi;
}

void ExprWalker::visit(SgDesignatedInitializer* sgn)
{
  // only applicable to C
  // designated initializer assigns members of variables, struct and union
  // entire struct/array/union is initialized by a parent initializer
  // SgDesignatedInitializer only initializes individual members
  // array example: int arr[4] = { 1, [2] = 4, [3] = 5}; 
  // SgDesignatedInitializer is used to initialize element [2] and [3]
  // No VariableId for arr[2] or arr[3]
  // struct example; struct { int a; }; struct A sA = { .a = 0};
  // here member 'a' is initalized using SgDesignatedInitializer
  // In the struct example 'a' has a VariableId whereas in the array example
  // we don't have any VariableId
  // we cannot be always consistent on what is defined
  // simply collect all the variables that can be used
  
  SgInitializer* initializer = sgn->get_memberInit();
  duvi = getDefUseVarsInfo_rec(initializer, vidm, false);
  if(duvi.isUseAfterDefCandidate()) {
    duvi.setUseAfterDef();
  }
}

/****************************
 * ExprWalker for basic cases
 ****************************/

void ExprWalker::visit(SgVarRefExp* sgn)
{
  // get the VariableId
  VariableId vid = vidm.variableId(sgn);
  
  ROSE_ASSERT(vid.isValid());

  // determine type info
  VariableIdTypeInfo sgn_type_info = getVariableIdTypeInfo(vid, vidm); 

  VarsInfo& def_vars_info = duvi.getDefVarsInfoMod();
  VarsInfo& use_vars_info = duvi.getUseVarsInfoMod();

  if(vidm.getModeVariableIdForEachArrayElement() && sgn_type_info == arrayType) {
    // If found a reference to whole array, def/use all its elements.
    duvi.addAllArrayElements(SageInterface::convertRefToInitializedName(sgn), vidm, isModExpr);
  } else if(isModExpr) {
    def_vars_info.first.insert(VariableIdInfo(vid, sgn_type_info));
  } else {
    use_vars_info.first.insert(VariableIdInfo(vid, sgn_type_info));
  }
}

void ExprWalker::visit(SgInitializedName* sgn)
{
  VariableId vid = vidm.variableId(sgn);
  
  // some SgInitializedName do not have symbols
  // VariableId is not created for such SgInitializedName
  // check and return if we are processing such SgInitializedName
  if(!vid.isValid()) {
    //cerr << "WARNING: Skipping SgInitializedName sgn->get_name() = " << sgn->get_name() << " with no symbol\n";
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
  
  // determine the type info
  VariableIdTypeInfo sgn_type_info = getVariableIdTypeInfo(vid, vidm);

  if(vidm.getModeVariableIdForEachArrayElement() && sgn_type_info == arrayType)
    // When defining an array, define all its elements.
    duvi.addAllArrayElements(sgn, vidm, true);
  else
    def_vars_info.first.insert(VariableIdInfo(vid, sgn_type_info));

  duvi = duvi + rduvi;
}

void ExprWalker::visit(SgValueExp* sgn)
{
  // dont need to do anything
}

void ExprWalker::visit(SgNullExpression* sgn)
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
  //std::ostringstream oss;
  //oss << "WARNING: Skipping VariableId for 'this' expr\n";
  //throw oss.str();
}

void ExprWalker::visit(SgClassNameRefExp* sgn)
{
  // no variableid
}

void ExprWalker::visit(SgLabelRefExp* sgn)
{
  // no variableid
}

void ExprWalker::visit(SgTemplateFunctionRefExp* sgn)
{
}
void ExprWalker::visit(SgTemplateMemberFunctionRefExp* sgn)
{
}
void ExprWalker::visit(SgTypeTraitBuiltinOperator* sgn)
{
}

void ExprWalker::visit(SgPseudoDestructorRefExp* sgn)
{
}

void ExprWalker::visit(SgStatementExpression* sgn)
{
  // not C++
  // gnu extension
  // this would require to get the statement
  // and perform the query on all expressions in the statement
  // can support this when the interface can take statement nodes
}

void ExprWalker::visit(SgAsmOp* sgn)
{
  // these are extensions
}

// we should not reach here
void ExprWalker::visit(SgExpression* sgn)
{
  std::ostringstream oss;
  oss << "Not handling " << sgn->class_name() << " expression \n";
  throw std::runtime_error(oss.str());
}

DefUseVarsInfo ExprWalker::getDefUseVarsInfo()
{
  return duvi;
}

/**************************************
 * DefUseVarsInfo Interface Functions *
 **************************************/

DefUseVarsInfo getDefUseVarsInfo(SgNode* sgn, VariableIdMapping& vidm)
{
  ROSE_ASSERT(sgn);
  if(SgExpression* expr = isSgExpression(sgn)) {
    return getDefUseVarsInfoExpr(expr, vidm);
  }
  else if(SgInitializedName* initName = isSgInitializedName(sgn)) {
    return getDefUseVarsInfoInitializedName(initName, vidm);
  }
  else if(SgVariableDeclaration* varDecl = isSgVariableDeclaration(sgn)) {
    return getDefUseVarsInfoVariableDeclaration(varDecl, vidm);
  }
  else {
    std::ostringstream oss;
    oss << "getDefUseVarsInfo() query can only be called on SgExpression/SgInitializedName/SgVariableDeclaration\n";
    throw std::runtime_error(oss.str());
  }
}

DefUseVarsInfo getDefUseVarsInfoExpr(SgExpression* sgn, VariableIdMapping& vidm)
{
  return getDefUseVarsInfo_rec(sgn, vidm, false);  
}

DefUseVarsInfo getDefUseVarsInfoInitializedName(SgInitializedName* sgn, VariableIdMapping& vidm)
{
  return getDefUseVarsInfo_rec(sgn, vidm, false);  
}

DefUseVarsInfo getDefUseVarsInfoVariableDeclaration(SgVariableDeclaration* sgn, VariableIdMapping& vidm)
{
  SgInitializedNamePtrList variables = sgn->get_variables();
  DefUseVarsInfo duvi, tduvi;
  SgInitializedNamePtrList::const_iterator it = variables.begin();
  for( ; it != variables.end(); ++it) {
    tduvi = getDefUseVarsInfo_rec(*it, vidm, false);
    duvi = duvi + tduvi;
  }
  return duvi;
}

// main implementation
DefUseVarsInfo getDefUseVarsInfo_rec(SgNode* sgn, VariableIdMapping& vidm, bool isModExpr)
{
  ExprWalker exprw(vidm, isModExpr);
  sgn->accept(exprw);
  return exprw.getDefUseVarsInfo();
}
