/*****************************************
 * Author: Sriram Aananthakrishnan, 2013 *
 *****************************************/

#include "sage3basic.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "addressTakenAnalysis.h"

using namespace CodeThorn;

/*************************************************
 ***************** ProcessQuery  *****************
 *************************************************/

MatchResult& SPRAY::ProcessQuery::getMatchResult()
{
  return match_result;
}

void SPRAY::ProcessQuery::printMatchResult()
{
  // MatchResult is list of maps
  // each map corresponds to one particular match instance
  for(MatchResult::iterator it = match_result.begin(); it != match_result.end(); it++)
  {
    for(SingleMatchVarBindings::iterator smbIt = (*it).begin(); smbIt != (*it).end(); smbIt++)
    {
      std::cout << "MATCH=";
      SgNode* matchedTerm = (*smbIt).second;
      ROSE_ASSERT(matchedTerm != NULL);
      std::cout << "  VAR: " << (*smbIt).first << "=" << 
        astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
  }
}

void SPRAY::ProcessQuery::clearMatchResult()
{
  match_result.clear();
}

MatchResult& SPRAY::ProcessQuery::operator()(std::string query, SgNode* root)
{
  AstMatching m;
  match_result = m.performMatching(query, root);
  return match_result;
}

/*************************************************
 ************* ComputeAddressTakenInfo  **********
 *************************************************/

SPRAY::ComputeAddressTakenInfo::AddressTakenInfo SPRAY::ComputeAddressTakenInfo::getAddressTakenInfo()
{
  return addressTakenInfo;
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::debugPrint(SgNode* sgn)
{
  std::cerr << sgn->class_name() << ": " << astTermWithNullValuesToString(sgn) << ", " \
  << sgn->unparseToString() << ", " \
  << sgn->get_file_info()->get_filenameString() << ", " \
  << sgn->get_file_info()->get_line() << ", " \
  << endl;
}

// base case for the recursion
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgVarRefExp *sgn)
{ 
  if(debuglevel > 0) debugPrint(sgn);
  VariableId id = cati.vidm.variableId(sgn);
  ROSE_ASSERT(id.isValid());
  // insert the id into VariableIdSet
  cati.addressTakenInfo.second.insert(id);
}

// only the rhs_op of SgDotExp is modified
// recurse on rhs_op
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgDotExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// only the rhs_op of SgDotExp is modified
// recurse on rhs_op
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgArrowExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// For example q = &(*p) where both q and p are pointer types
// In the example, q can potentially modify all variables pointed to by p
// same as writing q = p.
// Since we dont know anything about p, q can potentially modify all 
// the elements in addressTakenSet as result of the above expression 
// As a result, the variables whose addresses can be taken is the entire 
// set as we dont have any idea about p
// We dont need to add any new variable to addressTakenSet 
// as a consequence of the expressions similar to above.
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgPointerDerefExp* sgn)
{
 if(debuglevel > 0) debugPrint(sgn);
  // we raise a flag
  cati.addressTakenInfo.first = true;
}

// For example &(A[B[C[..]]]) or &(A[x][x][x])
// any pointer can that takes this address can modify
// contents of only A. The inner index expressions are r-values
// it is sufficient to add A to addressTakenSet
// keep recursing on the lhs until we find A
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgPntrArrRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // collect the info only if its array type
  // otherwise some pointer arithmetic is going on to compute the address of the array
  SgNode* arr_op = sgn->get_lhs_operand();
  if(isSgArrayType(arr_op)) {    
    arr_op->accept(*this);
    }
  else {
    // raise the flag as we dont know whose address is taken
    cati.addressTakenInfo.first = true;
  }
}

// &(a = expr)
// a's address is taken here
// process the lhs recursively
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgAssignOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* lhs_op = sgn->get_lhs_operand();
  lhs_op->accept(*this);
}

// &(++i)
// prefix increments first and the result can be used as lvalue (in C++)
// postfix uses the operand as lvalue and increments later and therefore
// postfix increment cannot be lvalue
// both prefix/postfix are illegal in C
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgPlusPlusOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* operand = sgn->get_operand();
  operand->accept(*this);
}

// same as prefix increment
// &(--i)
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgMinusMinusOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* operand = sgn->get_operand();
  operand->accept(*this);
}

// &(a+1, b)
// b's address is taken
// keep recursing on the rhs_op
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgCommaOpExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// if we see SgConditionalExp as operand of &
// both true and false branch are lvalues
// recurse on both of them to pick up the lvalues
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgConditionalExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* true_exp = sgn->get_true_exp();
  SgNode* false_exp = sgn->get_false_exp();
  true_exp->accept(*this);
  false_exp->accept(*this);
}

// cast can be lvalue
// example :  &((struct _Rep *)((this) -> _M_data()))[-1] expr from stl_list
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgCastExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* operand = sgn->get_operand();
  operand->accept(*this);
}

// void f() { }
// void (*f_ptr)() = &f;
// & on SgFunctionRefExp is redundant as the functions
// are implicity converted to function pointer
//
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // raise the flag
  cati.addressTakenInfo.first = true;
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgMemberFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // raise the flag
  // functions can potentially modify anything
  cati.addressTakenInfo.first = true;
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgTemplateFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // raise the flag
  // functions can potentially modify anything
  cati.addressTakenInfo.first = true;
}
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgTemplateMemberFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // raise the flag
  // functions can potentially modify anything
  cati.addressTakenInfo.first = true;
}

// A& foo() { return A(); }
// &(foo())
// if foo() returns a reference then foo() returns a lvalue
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgFunctionCallExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // we can look at its defintion and process the return expression ?
  // function calls can modify anything
  // raise the flag more analysis required
  cati.addressTakenInfo.first = true;
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgNode* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  std::cerr << "unhandled operand " << sgn->class_name() << " of SgAddressOfOp in AddressTakenAnalysis\n";
  std::cerr << sgn->unparseToString() << std::endl;
  ROSE_ASSERT(0);
}

void SPRAY::ComputeAddressTakenInfo::computeAddressTakenInfo(SgNode* root)
{
  // query to match all SgAddressOfOp subtrees
  // process query
  ProcessQuery collectSgAddressOfOp;
  // TODO: not sufficient to pick up address taken by function pointers
  std::string matchquery;

// "#SgTemplateArgument|"
// "#SgTemplateArgumentList|"
// "#SgTemplateParameter|"
// "#SgTemplateParameterVal|"
// "#SgTemplateParamterList|"
  
  // skipping all template declaration specific nodes as they dont have any symbols
  // we still traverse SgTemplateInstatiation*
  matchquery = \
    "#SgTemplateClassDeclaration|"\
    "#SgTemplateFunctionDeclaration|"\
    "#SgTemplateMemberFunctionDeclaration|"\
    "#SgTemplateVariableDeclaration|" \
    "#SgTemplateClassDefinition|"\
    "#SgTemplateFunctionDefinition|"\
    "$HEAD=SgAddressOfOp($OP)";
    
  MatchResult& matches = collectSgAddressOfOp(matchquery, root);
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); ++it) {
    SgNode* matchedOperand = (*it)["$OP"];
    // SgNode* head = (*it)["$HEAD"];
    // debugPrint(head); debugPrint(matchedOperand);
    OperandToVariableId optovid(*this);
    matchedOperand->accept(optovid);
  }              
}

// pretty print
void SPRAY::ComputeAddressTakenInfo::printAddressTakenInfo()
{
  std::cout << "addressTakenSet: [" << (addressTakenInfo.first? "true, " : "false, ")
            << VariableIdSetPrettyPrint::str(addressTakenInfo.second, vidm) << "]\n";
}

/*************************************************
 **************** TypeAnalysis *******************
 *************************************************/

VariableIdSet SPRAY::CollectTypeInfo::getPointerTypeSet()
{
  return pointerTypeSet;
}

VariableIdSet SPRAY::CollectTypeInfo::getArrayTypeSet()
{
  return arrayTypeSet;
}

VariableIdSet SPRAY::CollectTypeInfo::getReferenceTypeSet()
{
  return referenceTypeSet;
}


void SPRAY::CollectTypeInfo::collectTypes()
{
  if(varsUsed.size() == 0) {
    // get the entire set from VariableIdMapping
    varsUsed = vidm.getVariableIdSet();
  }

  for(VariableIdSet::iterator it = varsUsed.begin(); it != varsUsed.end(); ++it)
  {
    SgSymbol* v_symbol = vidm.getSymbol(*it);
    SgType* v_type = v_symbol->get_type();
    // Note on function pointer types
    // function pointer can modify any variable
    // not just the variables in the addressTakenSet
    // answering function pointer derefence requires side
    // effect analysis to determine the list of variables
    // can modify. Currenty we ignore function pointers as
    // the goal of this analysis is supposed to be simple.
    if(isSgPointerType(v_type))
    {
      SgType* baseType = v_type->findBaseType();
      // perhaps its worthwile to keep them in
      // a separte set and not support any dereferencing
      // queries rather than not adding them
      if(!isSgFunctionType(baseType))
      {
        pointerTypeSet.insert(*it);
      }
    }
    else if(isSgArrayType(v_type))
    {
      arrayTypeSet.insert(*it);
    }
    else if(isSgReferenceType(v_type))
    {
      referenceTypeSet.insert(*it);
    }
  }
}

void SPRAY::CollectTypeInfo::printPointerTypeSet()
{
  std::cout << "pointerTypeSet: " << VariableIdSetPrettyPrint::str(pointerTypeSet, vidm) << "\n";
}

void SPRAY::CollectTypeInfo::printArrayTypeSet()
{
  std::cout << "arrayTypeSet: " << VariableIdSetPrettyPrint::str(arrayTypeSet, vidm) << "\n";
}

void SPRAY::CollectTypeInfo::printReferenceTypeSet()
{
  std::cout << "referenceTypeSet: " << VariableIdSetPrettyPrint::str(referenceTypeSet, vidm) << "\n";
}

/*************************************************
 ********** FlowInsensitivePointerInfo  **********
 *************************************************/

void SPRAY::FlowInsensitivePointerInfo::collectInfo()
{
  compAddrTakenInfo.computeAddressTakenInfo(root);
  collTypeInfo.collectTypes();
}

void SPRAY::FlowInsensitivePointerInfo::printInfoSets()
{
  compAddrTakenInfo.printAddressTakenInfo();
  collTypeInfo.printPointerTypeSet();
  collTypeInfo.printArrayTypeSet();
  collTypeInfo.printReferenceTypeSet();
}

VariableIdMapping& SPRAY::FlowInsensitivePointerInfo::getVariableIdMapping()
{
  return vidm;
}

VariableIdSet SPRAY::FlowInsensitivePointerInfo::getMemModByPointer()
{
  VariableIdSet unionSet;
  VariableIdSet addrTakenSet = (compAddrTakenInfo.getAddressTakenInfo()).second;
  VariableIdSet arrayTypeSet = collTypeInfo.getArrayTypeSet();
  
  // std::set_union(addrTakenSet.begin(), addrTakenSet.end(),
  //                       arrayTypeSet.begin(), arrayTypeSet.end(),
  //                       unionSet.begin());

  // we can perhaps cache this for efficiency
  // to answer queries for multiple dereferencing queries
  SPRAY::set_union(addrTakenSet, arrayTypeSet, unionSet);

  return unionSet;
}
