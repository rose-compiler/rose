#include <iostream>
#include <fstream>
#include <algorithm>
#include "addressTakenAnalysis.h"

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
 ************** AddressTakenAnalysis  ************
 *************************************************/

VariableIdSet AddressTakenAnalysis::getAddressTakenSet()
{
  return addressTakenSet;
}

void AddressTakenAnalysis::throwIfUnInitException()
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

void AddressTakenAnalysis::OperandToVariableId::visit(SgVarRefExp *sgn)
{  
  ata.addressTakenSet.insert(ata.vidm.variableId(sgn));
}

void AddressTakenAnalysis::OperandToVariableId::visit(SgDotExp* sgn)
{
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

void AddressTakenAnalysis::OperandToVariableId::visit(SgArrowExp* sgn)
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
void AddressTakenAnalysis::OperandToVariableId::visit(SgPointerDerefExp* sgn)
{
  // we dont do anything here
}

// For example &(A[B[C[..]]])
// any pointer can that takes this address can modify
// contents of only A. The inner index expressions are r-values
// it is sufficient to add A to addressTakenSet
// 
void AddressTakenAnalysis::OperandToVariableId::visit(SgPntrArrRefExp* sgn)
{
  SgNode* arr_op = sgn->get_lhs_operand();
  SgVarRefExp* arr_name = isSgVarRefExp(arr_op); ROSE_ASSERT(arr_name != NULL);
  ata.addressTakenSet.insert(ata.vidm.variableId(arr_name));
}

void AddressTakenAnalysis::OperandToVariableId::visit(SgNode* sgn)
{
  std::cerr << "unhandled operand of SgAddressOfOp in AddressTakenAnalysis\n";
  ROSE_ASSERT(0);
}

void AddressTakenAnalysis::computeAddressTakenSet(SgNode* root)
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
void AddressTakenAnalysis::printAddressTakenSet()
{
  std::cout << "addressTakenSet: " << VariableIdSetPrettyPrint::str(addressTakenSet, vidm) << "\n";
}

/*************************************************
 **************** TypeAnalysis *******************
 *************************************************/

void TypeAnalysis::throwIfUnInitException()
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

VariableIdSet TypeAnalysis::getPointerTypeSet()
{
  return pointerTypeSet;
}

VariableIdSet TypeAnalysis::getArrayTypeSet()
{
  return arrayTypeSet;
}


void TypeAnalysis::collectTypes()
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

void TypeAnalysis::printPointerTypeSet()
{
  std::cout << "pointerTypeSet: " << VariableIdSetPrettyPrint::str(pointerTypeSet, vidm) << "\n";
}

void TypeAnalysis::printArrayTypeSet()
{
  std::cout << "arrayTypeSet: " << VariableIdSetPrettyPrint::str(arrayTypeSet, vidm) << "\n";
}

/*************************************************
 ******** FlowInsensitivePointerAnalysis  ********
 *************************************************/

void FlowInsensitivePointerAnalysis::runAnalysis()
{
  addrTakenAnalysis.computeAddressTakenSet(root);
  typeAnalysis.collectTypes();
}

void FlowInsensitivePointerAnalysis::printAnalysisSets()
{
  addrTakenAnalysis.printAddressTakenSet();
  typeAnalysis.printPointerTypeSet();
  typeAnalysis.printArrayTypeSet();
}

VariableIdMapping& FlowInsensitivePointerAnalysis::getVariableIdMapping()
{
  return vidm;
}

VariableIdSet FlowInsensitivePointerAnalysis::getMemModByPointer()
{
  VariableIdSet unionSet;
  const VariableIdSet addrTakenSet = addrTakenAnalysis.getAddressTakenSet();
  const VariableIdSet arrayTypeSet = typeAnalysis.getArrayTypeSet();
  
  // std::set_union(addrTakenSet.begin(), addrTakenSet.end(),
  //                       arrayTypeSet.begin(), arrayTypeSet.end(),
  //                       unionSet.begin());

  VariableIdSet::const_iterator it1 = addrTakenSet.begin();
  VariableIdSet::const_iterator it2 = arrayTypeSet.begin();
  VariableIdSet::iterator result = unionSet.begin();
  
  // re-implementation of set-union
  while(true)
  {
    if(it1 == addrTakenSet.end())
    {
      unionSet.insert(it2, arrayTypeSet.end());
      break;
    }
    if(it2 == arrayTypeSet.end())
    {
      unionSet.insert(it1, addrTakenSet.end());
      break;
    }
    
    if(*it1 < *it2)
    {
      unionSet.insert(result, *it1); ++it1; ++result;
    }
    else if(*it2 < *it1)
    {
      unionSet.insert(result, *it2); ++it2; ++result;
    }
    else
    {
      unionSet.insert(result, *it1); ++it1; ++it2; ++result;
    }
  }


  ROSE_ASSERT(unionSet.size() == arrayTypeSet.size() + addrTakenSet.size());

  return unionSet;
}
