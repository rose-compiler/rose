#include <iostream>
#include <fstream>
#include <algorithm>
#include "addressTakenAnalysis.h"

// utility functions
void set_union(const VariableIdSet& set1, const VariableIdSet& set2, VariableIdSet& rset)
{
  VariableIdSet::const_iterator it1 = set1.begin();
  VariableIdSet::const_iterator it2 = set2.begin();
  VariableIdSet::iterator rit = rset.begin();
  
  // // re-implementation of set-union
  while(true)
  {
    if(it1 == set1.end())
    {
      rset.insert(it2, set2.end());
      break;
    }
    if(it2 == set2.end())
    {
      rset.insert(it1, set1.end());
      break;
    }
    
    if(*it1 < *it2)
    {
      rset.insert(rit, *it1); ++it1; ++rit;
    }
    else if(*it2 < *it1)
    {
      rset.insert(rit, *it2); ++it2; ++rit;
    }
    else
    {
      rset.insert(rit, *it1); ++it1; ++it2; ++rit;
    }
  }
}

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
      std::cout << "  VAR: " << (*smbIt).first << "=" << astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
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
  try
  {
    if(!vidm.isUniqueVariableSymbolMapping())
      throw;
  }
  catch(...)
  {
    std::cerr << "Analysis Not initialized: Variable symbol mapping not computed\n";
  }
}

void ComputeAddressTakenInfo::OperandToVariableId::visit(SgVarRefExp *sgn)
{  
  cati.addressTakenSet.insert(cati.vidm.variableId(sgn));
}

void ComputeAddressTakenInfo::OperandToVariableId::visit(SgDotExp* sgn)
{
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

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

// For example &(A[B[C[..]]])
// any pointer can that takes this address can modify
// contents of only A. The inner index expressions are r-values
// it is sufficient to add A to addressTakenSet
// 
void ComputeAddressTakenInfo::OperandToVariableId::visit(SgPntrArrRefExp* sgn)
{
  SgNode* arr_op = sgn->get_lhs_operand();
  SgVarRefExp* arr_name = isSgVarRefExp(arr_op); ROSE_ASSERT(arr_name != NULL);
  cati.addressTakenSet.insert(cati.vidm.variableId(arr_name));
}

void ComputeAddressTakenInfo::OperandToVariableId::visit(SgNode* sgn)
{
  std::cerr << "unhandled operand of SgAddressOfOp in AddressTakenAnalysis\n";
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
  try
  {
    if(!vidm.isUniqueVariableSymbolMapping())
      throw;
  }
  catch(...)
  {
    std::cerr << "Analysis Not initialized: Variable symbol mapping not computed\n";
  }
}

VariableIdSet CollectTypeInfo::getPointerTypeSet()
{
  return pointerTypeSet;
}

VariableIdSet CollectTypeInfo::getArrayTypeSet()
{
  return arrayTypeSet;
}


void CollectTypeInfo::collectTypes()
{
  throwIfUnInitException();
  VariableIdSet set = vidm.getVariableIdSet();
  for(VariableIdSet::iterator it = set.begin(); it != set.end(); ++it)
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
