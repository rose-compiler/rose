#ifndef _DEFUSEQUERY_H
#define _DEFUSEQUERY_H

/*************************************************************
 * Copyright: (C) 2013 by Sriram Aananthakrishnan            *
 * Author   : Sriram Aananthakrishnan                        *
 * email    : aananthakris1@llnl.gov                         *
 *************************************************************/


#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"
#include "Miscellaneous.h"
#include <set>
#include <utility>

#include "VariableIdUtils.h"

using namespace CodeThorn;

// VariableId:
// numbers all variable symbols in the program 
// with a unique number. It is not used to
// represent a location within an array or heap
// region. With VariableId, we can only reason
// about entire region (for example arr in arr[i])
// and not individual locations in the region

// typedef std::set<VariableId> VariableIdSet;

// MemObjInfo is used to represent the result
// of def/use query on expressions. The result
// represents a set of locations that are
// modified/used by any given expression.
// MemObjInfo is a pair <VariableIdSet, bool>
// @VariableIdSet: set of memory locations 
// represented by their VariableId. 
// @bool: flag is set if the memory locations
// that may be used/modified this expression
// are different from those appearing on the
// expression itself (example: *p or array + 5)
// flag is an indicator for requiring more
// sophisticated pointer analysis to answer the
// def/use query on this expression
typedef std::pair<VariableIdSet, bool> MemObjInfo;

typedef std::set<SgFunctionCallExp*> FunctionCallExpSet;
// it is expensive to answer def/use queries on demand
// for expressions that involve function calls
// as it would mean walking all the expressions
// in the function and the functions that it may call
// @FunctionCallExpSet: consitsts of all the functions
// that this expression is calling
// @bool: flag is an indicator that the 
// expression alone is not sufficient to
// answer the def/use query
typedef std::pair<FunctionCallExpSet, bool> FunctionCallExpInfo;

/*************************************************
 *************** DefUseMemObjInfo ****************
 *************************************************/

// determined completely based on syntactic information
// def_set consists of VariableIds which is written by the expression
// use_set consists of VariableIds which are read but not modified by this expression
// func_set consists of all SgFunctionCallExps that are invoked by this expression
class DefUseMemObjInfo
{
  MemObjInfo def_set;
  MemObjInfo use_set;
  FunctionCallExpInfo func_set;
   
public:
  DefUseMemObjInfo() { }
  DefUseMemObjInfo(const MemObjInfo& _dset, const MemObjInfo& _uset, const FunctionCallExpInfo& _fset);
  
  // returns the corresponding info about the memory locations
  MemObjInfo getDefMemObjInfo();
  MemObjInfo getUseMemObjInfo();
  MemObjInfo& getDefMemObjInfoMod(); 
  MemObjInfo& getUseMemObjInfoMod();
  const MemObjInfo& getDefMemObjInfoRef() const;
  const MemObjInfo& getUseMemObjInfoRef() const;

  FunctionCallExpInfo getFunctionCallExpInfo();
  FunctionCallExpInfo& getFunctionCallExpInfoMod();
  const FunctionCallExpInfo& getFunctionCallExpInfoRef() const;
  
  // copy sets to handle side-effects
  void copyDefToUse();
  void copyUseToDef();
  
  bool isDefSetEmpty();
  bool isUseSetEmpty();
  bool isFunctionCallExpInfoEmpty();

  // returns the flag func_modify
  bool isModByFunction();

  // returns the flag of MemObjInfo
  bool isDefSetModByPointer();
  bool isUseSetModByPointer();

  DefUseMemObjInfo operator+(const DefUseMemObjInfo& dumo1);

  std::string str();
  // for more readability
  std::string str(VariableIdMapping& vidm);
  std::string funcCallExpSetPrettyPrint();
};

// used by the getDefUseMemObjInfo_rec to traverse the 
// structure of the expression and call the appropriate
// getDefUseLHS/RHS functions
class ExprWalker : public ROSE_VisitorPatternDefaultBase
{
  VariableIdMapping& vidm;
  // if set then we are processing an expression on lhs of assignment
  bool isModExpr;
  DefUseMemObjInfo dumo;

public:
  ExprWalker(VariableIdMapping& _vidm, bool isModExpr);
  // lhs of assignment operator are always lvalues
  // process them 
  void visit(SgAssignOp* sgn);
  void visit(SgCompoundAssignOp* sgn);

  // recurse on sub-expressions
  void visit(SgBinaryOp* sgn);

  void visit(SgCastExp* sgn);
  void visit(SgAddressOfOp* sgn);
  void visit(SgMinusMinusOp* sgn);
  void visit(SgMinusOp* sgn);
  void visit(SgUnaryAddOp* sgn);
  void visit(SgNotOp* sgn);
  void visit(SgPlusPlusOp* sgn);
  void visit(SgSizeOfOp* sgn);

  void visit(SgFunctionCallExp* sgn);
  void visit(SgExprListExp* sgn);
  void visit(SgConditionalExp* sgn);

  
  // recursion undwinds on basic expressions
  // that represent memory
  //  
  void visit(SgVarRefExp* sgn);
  void visit(SgPntrArrRefExp* sgn);
  void visit(SgPointerDerefExp* sgn);
  void visit(SgDotExp* sgn);
  void visit(SgArrowExp* sgn);

  void visit(SgInitializedName* sgn);
  void visit(SgAssignInitializer* sgn);
  void visit(SgConstructorInitializer* sgn);

  void visit(SgValueExp* sgn);
  void visit(SgFunctionRefExp* sgn);
  void visit(SgMemberFunctionRefExp* sgn);

  void visit(SgExpression* sgn);

  DefUseMemObjInfo getDefUseMemObjInfo();
};

class LvalueExprWalker : public ROSE_VisitorPatternDefaultBase
{
  VariableIdMapping& vidm;
  bool isModExpr;
  DefUseMemObjInfo dumo;
public:
  LvalueExprWalker(VariableIdMapping& _vidm, bool _isModExpr);
  void visit(SgVarRefExp* sgn);
  void visit(SgPntrArrRefExp* sgn);
  void visit(SgPointerDerefExp* sgn);
  void visit(SgDotExp* sgn);
  void visit(SgArrowExp* sgn);
  void visit(SgInitializedName* sgn);

  DefUseMemObjInfo getDefUseMemObjInfo();
};


// interface function to
// identify modified and used locations based on syntax
//  a[i] = expr; a is modified and i, expr are used 
// *p = expr; since we dont know anything about p
// the locations that are modified involve all arrays and variables in addressTakenSet
// if the expression involve functions or function pointers it can 
// potentially modify everything
DefUseMemObjInfo getDefUseMemObjInfo(SgNode* sgn, VariableIdMapping& vidm);

// internal implementation
// @sgn: root node
// @fipi: required to answer dereferencing queries
// @dumo: the defuse object that will be built
// @isModExpr: set to true if the expression is modifying a memory location
DefUseMemObjInfo getDefUseMemObjInfo_rec(SgNode* sgn, VariableIdMapping& vidm, bool isModExpr);

// used to process the lhs of assignment operator
// invokes a visitor pattern and adds the modified variables
// to def_set and used variables to use_set of dumo object
DefUseMemObjInfo getDefUseMemObjInfoLvalue(SgNode* sgn, VariableIdMapping& vidm, bool isModExpr);

#endif

