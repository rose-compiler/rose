#ifndef ADDRESSTAKENANALYSIS_H
#define ADDRESSTAKENANALYSIS_H

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

#include "FunctionIdMapping.h"
#include "VariableIdUtils.h"

// AST Query Processor
// common functor to process any query and build match result

/*************************************************
 ***************** ProcessQuery  *****************
 *************************************************/

namespace SPRAY {

class ProcessQuery
{
  // the root node on which the AST matching needs to be performed
  MatchResult match_result;

public:
  ProcessQuery() { }
  // functor to operate on the given query
  MatchResult& operator()(std::string query, SgNode* root);
  MatchResult& getMatchResult();
  void printMatchResult();
  void clearMatchResult();
};

/*************************************************
 ************* ComputeAddressTakenInfo  **********
 *************************************************/
class ComputeAddressTakenInfo
{
  typedef std::pair<bool, VariableIdSet> VariableAddressTakenInfo;
  typedef std::pair<bool, FunctionIdSet> FunctionAddressTakenInfo;
  VariableIdMapping& vidm;
  FunctionIdMapping& fidm;
  // result to be computed by this analysis
  // bool is set to true when operand of SgAddressOfExp is a complicated
  // expression for which VariableId cannot be determined
  // example: &(*p)
  VariableAddressTakenInfo variableAddressTakenInfo; // variables of which the address was taken
  FunctionAddressTakenInfo functionAddressTakenInfo; // functions of which the address was taken

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
    int debuglevel;
  public:
  OperandToVariableId(ComputeAddressTakenInfo& _cati) : cati(_cati), debuglevel(0) { }
    void visit(SgVarRefExp*);
    void visit(SgVariableDeclaration*);
    void visit(SgDotExp*);
    void visit(SgArrowExp*);
    void visit(SgPointerDerefExp*);
    void visit(SgPntrArrRefExp*);
    void visit(SgAssignOp* sgn);
    void visit(SgPlusPlusOp* sgn);
    void visit(SgMinusMinusOp* sgn);
    void visit(SgCommaOpExp* sgn);
    void visit(SgConditionalExp* sgn);
    void visit(SgCastExp* sgn);
    // The following SgXXXFunctionRefExp types only have the
    //  base class SgExpression in common
    void visit(SgFunctionRefExp* sgn);
    void visit(SgMemberFunctionRefExp* sgn);
    void visit(SgTemplateFunctionRefExp* sgn);
    void visit(SgTemplateMemberFunctionRefExp* sgn);
    void visit(SgFunctionCallExp* sgn);
    void visit(SgThisExp* sgn);
    void visit(SgNode* sgn);
    void insertFunctionId(FunctionId);
    void debugPrint(SgNode* sgn);
  };
public:
  ComputeAddressTakenInfo(VariableIdMapping& _vidm, FunctionIdMapping& _fidm) : vidm(_vidm), fidm(_fidm)
  {
    variableAddressTakenInfo.first = false;
    functionAddressTakenInfo.first = false;
  }
  void computeAddressTakenInfo(SgNode* root);
  void printAddressTakenInfo();
  VariableAddressTakenInfo getVariableAddressTakenInfo();
  FunctionAddressTakenInfo getFunctionAddressTakenInfo();
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
  FunctionIdMapping& fidm;
  ComputeAddressTakenInfo compAddrTakenInfo;
  CollectTypeInfo collTypeInfo;

public:
  FlowInsensitivePointerInfo(SgProject* project, VariableIdMapping& _vidm, FunctionIdMapping& _fidm) : root(project),
    vidm(_vidm),
    fidm(_fidm),
    compAddrTakenInfo(_vidm, _fidm),
    collTypeInfo(_vidm)
  { 
  }

  FlowInsensitivePointerInfo(SgProject* project, 
                             VariableIdMapping& _vidm, 
                             FunctionIdMapping& _fidm,
                             VariableIdSet usedVarsInProgram) : root(project),
    vidm(_vidm),
    fidm(_fidm),
    compAddrTakenInfo(_vidm, _fidm),
    collTypeInfo(_vidm, usedVarsInProgram)
    {
    }
  void collectInfo();
  void printInfoSets();
  VariableIdSet getMemModByPointer();
  VariableIdMapping& getVariableIdMapping();
};

} // end of namespace SPRAY

#endif
