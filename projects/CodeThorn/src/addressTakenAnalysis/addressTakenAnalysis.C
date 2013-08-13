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

MatchResult ProcessQuery::getMatchResult()
{
  return match_result;
}

void ProcessQuery::printMatchResult()
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

void ProcessQuery::clearMatchResult()
{
  match_result.clear();
}

MatchResult ProcessQuery::operator()(std::string query, SgNode* root)
{
  AstMatching m;
  match_result = m.performMatching(query, root);
  return match_result;
}

/*************************************************
 ************* ComputeAddressTakenInfo  **********
 *************************************************/

VariableIdSet ComputeAddressTakenInfo::getAddressTakenSet()
{
  return addressTakenSet;
}

void ComputeAddressTakenInfo::throwIfUnInitException()
{
    if(!vidm.isUniqueVariableSymbolMapping())
      throw std::runtime_error("Variable symbol mapping not computed\n");
}

// base case for the recursion
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgVarRefExp *sgn)
{  
  cati.addressTakenSet.insert(cati.vidm.variableId(sgn));
}

// only the rhs_op of SgDotExp is modified
// recurse on rhs_op
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgDotExp* sgn)
{
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// only the rhs_op of SgDotExp is modified
// recurse on rhs_op
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgArrowExp* sgn)
{
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
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgPointerDerefExp* sgn)
{
  // we dont do anything here
}

// For example &(A[B[C[..]]]) or &(A[x][x][x])
// any pointer can that takes this address can modify
// contents of only A. The inner index expressions are r-values
// it is sufficient to add A to addressTakenSet
// keep recursing on the lhs until we find A
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgPntrArrRefExp* sgn)
{
  SgNode* arr_op = sgn->get_lhs_operand();
  // SgVarRefExp* arr_name = isSgVarRefExp(arr_op); ROSE_ASSERT(arr_name != NULL);
  // cati.addressTakenSet.insert(cati.vidm.variableId(arr_name));
  arr_op->accept(*this);
}

// &(a = expr)
// a's address is taken here
// process the lhs recursively
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgAssignOp* sgn)
{
  SgNode* lhs_op = sgn->get_lhs_operand();
  lhs_op->accept(*this);
}

// &(a+1, b)
// b's address is taken
// keep recursing on the rhs_op
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgCommaOpExp* sgn)
{
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// if we see SgConditionalExp as operand of &
// both true and false branch are lvalues
// recurse on both of them to pick up the lvalues
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgConditionalExp* sgn)
{
  SgNode* true_exp = sgn->get_true_exp();
  SgNode* false_exp = sgn->get_false_exp();
  true_exp->accept(*this);
  false_exp->accept(*this);
}

// void f() { }
// void (*f_ptr)() = &f;
// & on SgFunctionRefExp is redundant as the functions
// are implicity converted to function pointer
//
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgFunctionRefExp* sgn)
{
  // not sure what do to do here
  // we dont have VariabldId for SgFunctionRefExp
  ROSE_ASSERT(0);
}

// A& foo() { return A(); }
// &(foo())
// if foo() returns a reference then foo() returns a lvalue
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgFunctionCallExp* sgn)
{
  // we can look at its defintion and process the return expression ?
  ROSE_ASSERT(0);
}

void ComputeAddressTakenInfo::OperandToVariableId::visit(SgNode* sgn)
{
  std::cerr << "unhandled operand " << sgn->class_name() << " of SgAddressOfOp in AddressTakenAnalysis\n";
  ROSE_ASSERT(0);
}

void ComputeAddressTakenInfo::computeAddressTakenSet(SgNode* root)
{
  throwIfUnInitException();
  // query to match all SgAddressOfOp subtrees
  // process query
  ProcessQuery procQuery;
  // TODO: not sufficient to pick up address taken by function pointers
  MatchResult matches = procQuery("$HEAD=SgAddressOfOp($OP)", root);
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); it++)
  {
    SgNode* matchedOperand = (*it)["$OP"];
    OperandToVariableId optovid(*this);
    matchedOperand->accept(optovid);
  }
}

// pretty print
void ComputeAddressTakenInfo::printAddressTakenSet()
{
  std::cout << "addressTakenSet: " << VariableIdSetPrettyPrint::str(addressTakenSet, vidm) << "\n";
}

/*************************************************
 **************** TypeAnalysis *******************
 *************************************************/

void CollectTypeInfo::throwIfUnInitException()
{
    if(!vidm.isUniqueVariableSymbolMapping())
      throw std::runtime_error("Variable symbol mapping not computed\n");
}

VariableIdSet CollectTypeInfo::getPointerTypeSet()
{
  return pointerTypeSet;
}

VariableIdSet CollectTypeInfo::getArrayTypeSet()
{
  return arrayTypeSet;
}

VariableIdSet CollectTypeInfo::getReferenceTypeSet()
{
  return referenceTypeSet;
}


void CollectTypeInfo::collectTypes()
{
  throwIfUnInitException();
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

void CollectTypeInfo::printPointerTypeSet()
{
  std::cout << "pointerTypeSet: " << VariableIdSetPrettyPrint::str(pointerTypeSet, vidm) << "\n";
}

void CollectTypeInfo::printArrayTypeSet()
{
  std::cout << "arrayTypeSet: " << VariableIdSetPrettyPrint::str(arrayTypeSet, vidm) << "\n";
}

void CollectTypeInfo::printReferenceTypeSet()
{
  std::cout << "referenceTypeSet: " << VariableIdSetPrettyPrint::str(referenceTypeSet, vidm) << "\n";
}

/*************************************************
 ********** FlowInsensitivePointerInfo  **********
 *************************************************/

void FlowInsensitivePointerInfo::collectInfo()
{
  compAddrTakenInfo.computeAddressTakenSet(root);
  collTypeInfo.collectTypes();
}

void FlowInsensitivePointerInfo::printInfoSets()
{
  compAddrTakenInfo.printAddressTakenSet();
  collTypeInfo.printPointerTypeSet();
  collTypeInfo.printArrayTypeSet();
  collTypeInfo.printReferenceTypeSet();
}

VariableIdMapping& FlowInsensitivePointerInfo::getVariableIdMapping()
{
  return vidm;
}

VariableIdSet FlowInsensitivePointerInfo::getMemModByPointer()
{
  VariableIdSet unionSet;
  VariableIdSet addrTakenSet = compAddrTakenInfo.getAddressTakenSet();
  VariableIdSet arrayTypeSet = collTypeInfo.getArrayTypeSet();
  
  // std::set_union(addrTakenSet.begin(), addrTakenSet.end(),
  //                       arrayTypeSet.begin(), arrayTypeSet.end(),
  //                       unionSet.begin());

  // we can perhaps cache this for efficiency
  // to answer queries for multiple dereferencing queries
  set_union(addrTakenSet, arrayTypeSet, unionSet);

  return unionSet;
}
