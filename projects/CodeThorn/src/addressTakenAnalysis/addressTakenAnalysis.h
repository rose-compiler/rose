#ifndef _ADDRESSTAKENANALYSIS_H
#define _ADDRESSTAKENANALYSIS_H

/*************************************************************
 * Copyright: (C) 2013 by Sriram Aananthakrishnan            *
 * Author   : Sriram Aananthakrishnan                        *
 * email    : aananthakris1@llnl.gov                         *
 *************************************************************/

#include "AstTerm.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"
#include "Miscellaneous.h"
#include <set>
#include "VariableIdUtils.h"
#include "AstVariableIdInterface.h"

using namespace CodeThorn;

// AST Query Processor
// common functor to process any query and build match result

/*************************************************
 ***************** ProcessQuery  *****************
 *************************************************/
class ProcessQuery
{
  // the root node on which the AST matching needs to be performed
  MatchResult match_result;

public:
  ProcessQuery() { }
  // functor to operate on the given query
  MatchResult operator()(std::string query, SgNode* root);
  MatchResult getMatchResult();
  void printMatchResult();
  void clearMatchResult();
};

/*************************************************
 ************* ComputeAddressTakenInfo  **********
 *************************************************/
class ComputeAddressTakenInfo
{
  VariableIdMapping& vidm;
  // result to be computed by this analysis
  VariableIdSet addressTakenSet;

  // address can be taken for any expression that is lvalue
  // The purpose of this class is to traverse arbitrary
  // expressions that are operands of SgAddressOfOp and find the
  // variable whose address is actually taken.
  // For example in expression &(a.b->c),  'c' address is
  // actually taken. This class simply traverses the operand
  // of SgAddressOfOp to identify 
  // the variable whose address is taken
  // 
  class OperandToVariableId : public ROSE_VisitorPatternDefaultBase
  {
    ComputeAddressTakenInfo& cati;
  public:
    OperandToVariableId(ComputeAddressTakenInfo& _cati) : cati(_cati) { }
    void visit(SgVarRefExp*);
    void visit(SgDotExp*);
    void visit(SgArrowExp*);
    void visit(SgPointerDerefExp*);
    void visit(SgPntrArrRefExp*);
    void visit(SgAssignOp* sgn);
    void visit(SgCommaOpExp* sgn);
    void visit(SgConditionalExp* sgn);
    void visit(SgFunctionRefExp* sgn);
    void visit(SgFunctionCallExp* sgn);
    void visit(SgNode* sgn);    
  };
public:
  ComputeAddressTakenInfo(VariableIdMapping& _vidm) : vidm(_vidm) {}
  void throwIfUnInitException();
  void computeAddressTakenSet(SgNode* root);
  void printAddressTakenSet();
  VariableIdSet getAddressTakenSet();
};

/*************************************************
 *************** CollectTypeInfo *****************
 *************************************************/
class CollectTypeInfo
{
  VariableIdMapping& vidm;
  VariableIdSet pointerTypeSet;
  VariableIdSet arrayTypeSet;
  VariableIdSet referenceTypeSet;
  bool initialized;

  // collect type info for only these if set by constructor
  VariableIdSet varsUsed;

public:
  CollectTypeInfo(VariableIdMapping& _vidm) : vidm(_vidm) { }
  CollectTypeInfo(VariableIdMapping& _vidm,
                  VariableIdSet _usedVarsInFunctions) 
    : vidm(_vidm),
    varsUsed(_usedVarsInFunctions) { }

  void collectTypes();
  void printPointerTypeSet();
  void printArrayTypeSet();
  void printReferenceTypeSet();
  void throwIfUnInitException();
  VariableIdSet getPointerTypeSet();
  VariableIdSet getArrayTypeSet();
  VariableIdSet getReferenceTypeSet();
};

/*************************************************
 ********** FlowInsensitivePointerInfo  **********
 *************************************************/
class FlowInsensitivePointerInfo
{
  SgNode* root;
  VariableIdMapping& vidm;
  ComputeAddressTakenInfo compAddrTakenInfo;
  CollectTypeInfo collTypeInfo;

public:
  FlowInsensitivePointerInfo(SgProject* project, VariableIdMapping& _vidm) : root(project), 
    vidm(_vidm),
    compAddrTakenInfo(_vidm),
    collTypeInfo(_vidm)
  { 
  }

  FlowInsensitivePointerInfo(SgProject* project, 
                             VariableIdMapping& _vidm, 
                             VariableIdSet usedVarsInProgram) : root(project),
    vidm(_vidm),
    compAddrTakenInfo(_vidm),
    collTypeInfo(_vidm, usedVarsInProgram)
    {
    }
  void collectInfo();
  void printInfoSets();
  VariableIdSet getMemModByPointer();
  VariableIdMapping& getVariableIdMapping();
};

#endif
