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

namespace CodeThorn {

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

  // schroder3 (2016-07-29): Controls whether address-takings inside template declarations/
  //  definitions should be added to the address-taken set or not.
  bool addAddressTakingsInsideTemplateDecls;

  // schroder3 (2016-08-09): Specifies whether the ASTMatching should be used to determine
  //  all address-taken-relevant nodes. Currently slower than the for-loop with if-else
  //  constructs.
  bool useASTMatching;

  // result to be computed by this analysis
  // bool is set to true when operand of SgAddressOfExp is a complicated
  // expression for which VariableId cannot be determined
  // example: &(*p)
  VariableAddressTakenInfo variableAddressTakenInfo;
  // schroder3 (2016-07-19): second result of this analysis: set of function ids of address taken
  //  functions. The bool is currently not set.
  FunctionAddressTakenInfo functionAddressTakenInfo;

  // schroder3 (2016-07-19): Extended comment by function ids,
  //  reference creation and implicit address-taking of functions.
  //
  // address can be taken for any expression that is lvalue
  // The purpose of this class is to traverse arbitrary
  // expressions that are operands of SgAddressOfOp and find the
  // variables and functions whose address is actually taken. In
  // addition, SgVariableDeclaration and SgFunctionCallExp nodes are
  // searched for alias/ reference creation. Furthermore,
  // SgAssignOp and SgReturnStmt are considered too, because
  // they might contain a implicit address-taking of a function.
  //
  // For example in expression &(a.b->c),  'c' address is
  // actually taken. This class simply traverses the operand
  // of SgAddressOfOp to identify 
  // the variable or function whose address is taken
  // 
  // schroder3 (2016-07-19): TODO: Rename to reflect the current purpose
  //  (something like AddressTakingNodeToAddressTakenSet?)
  class OperandToVariableId : public ROSE_VisitorPatternDefaultBase
  {
    ComputeAddressTakenInfo& cati;
    int debuglevel;
   public:
    enum AssociationKind {
      AK_Assignment,
      AK_Initialization,
      AK_Cast
    };
    OperandToVariableId(ComputeAddressTakenInfo& _cati) : cati(_cati), debuglevel(0) { }
    void visit(SgVarRefExp*);
    void visit(SgVariableDeclaration*);
    void visit(SgDotExp*);
    void visit(SgArrowExp*);
    void visit(SgDotStarOp*);
    void visit(SgArrowStarOp*);
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
    void visit(SgReturnStmt* sgn);
    void visit(SgFunctionCallExp* sgn);
    void visit(SgThisExp* sgn);
    void visit(SgAddressOfOp* sgn);
    void visit(SgCtorInitializerList* sgn);
    void visit(SgConstructorInitializer* sgn);
    void visit(SgFunctionParameterList* sgn);
    void visit(SgLambdaExp* sgn);
    void visit(SgExpression* sgn);
    void visit(SgThrowOp* sgn);
    void visit(SgAggregateInitializer* sgn);
    void visit(SgCompoundInitializer* sgn);
    void visit(SgCompoundLiteralExp* sgn);
    void visit(SgDesignatedInitializer* sgn);
    void visit(SgLambdaCapture* sgn);
    void visit(SgNode* sgn);
    void insertVariableId(VariableId);
    void insertFunctionId(FunctionId);
    void insertAllVariableIds();
    void insertAllFunctionIds();
    // schroder3 (2016-07-20): Handles the arguments of a constructor or (member) function call regarding their "address-taken-ness".
    void handleCall(const SgTypePtrList& parameterTypes, const SgExpressionPtrList& argumentExpressions);
    // schroder3 (2016-07-20): Handles all kinds of associations (currently initializations and assignments) regarding their "address-taken-ness".
    void handleAssociation(const AssociationKind associationKind, const std::vector<VariableId>& possibleTargetEntities,
                           const SgType* targetEntityType, /*const*/ SgExpression* associatedExpression);
    void debugPrint(SgNode* sgn);
  };
public:
  ComputeAddressTakenInfo(VariableIdMapping& _vidm, FunctionIdMapping& _fidm)
    : vidm(_vidm), fidm(_fidm), addAddressTakingsInsideTemplateDecls(false), useASTMatching(false)
  {
    variableAddressTakenInfo.first = false;
    functionAddressTakenInfo.first = false;
  }
  void computeAddressTakenInfo(SgNode* root);
  // schroder3 (2016-08-09): Alternative to computeAddressTakenInfo(...) that uses the ASTMatching mechanism.
  //  This is currently slower than computeAddressTakenInfo(...).
  void computeAddressTakenInfoUsingASTMatching(SgNode* root);
  void printAddressTakenInfo();
  VariableAddressTakenInfo getVariableAddressTakenInfo();
  FunctionAddressTakenInfo getFunctionAddressTakenInfo();

  // schroder3 (2016-07-29): Added getter and setter:
  bool getAddAddressTakingsInsideTemplateDecls();
private:
  // schroder3 (2016-07-29): This is currently private because setting the member to true will
  //  make tests fail.
  void setAddAddressTakingsInsideTemplateDecls(bool);
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
  // schroder3: TODO: replace calls of getMemModByPointer by calls
  //  of getAddressTakenVariables
  VariableIdSet getMemModByPointer();
  VariableIdSet getAddressTakenVariables();
  FunctionIdSet getAddressTakenFunctions();
  VariableIdMapping& getVariableIdMapping();
};

} // end of namespace CodeThorn

#endif
