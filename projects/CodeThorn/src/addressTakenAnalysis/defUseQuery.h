#ifndef _DEFUSEQUERY_H
#define _DEFUSEQUERY_H

/*************************************************************
 * Copyright: (C) 2013 by Sriram Aananthakrishnan            *
 * Author   : Sriram Aananthakrishnan                        *
 * email    : aananthakris1@llnl.gov                         *
 *************************************************************/


#include "addressTakenAnalysis.h"

/*************************************************
 *************** DefUseMemObjInfo ****************
 *************************************************/

// determined completely based on syntactic information
// def_set consists of VariableIds which is written by the expression
// use_set consists of VariableIds which are read but not modified by this expression
// consider using references for efficiency
class DefUseMemObjInfo
{
  VariableIdSet def_set;
  VariableIdSet use_set;
  bool ptr_modify;
  bool func_modify;

public:
  DefUseMemObjInfo() : ptr_modify(false), func_modify(false) { }
  DefUseMemObjInfo(const VariableIdSet& _def_set, const VariableIdSet& _use_set);
  VariableIdSet getDefSet();
  VariableIdSet getUseSet();
  
  VariableIdSet& getDefSetRefMod(); 
  VariableIdSet& getUseSetRefMod();

  const VariableIdSet& getDefSetRef() const;
  const VariableIdSet& getUseSetRef() const;
  
  bool isModByPointer();
  bool isModByFunction();

  bool isDefSetEmpty();
  bool isUseSetEmpty();

  void copyDefToUseSet();
  void copyUseToDefSet();
  DefUseMemObjInfo operator+(const DefUseMemObjInfo& dumo1);

  std::string str();
  // for more readability
  std::string str(VariableIdMapping& vidm);
};

// used by the getDefUseMemObjInfo_rec to traverse the 
// structure of the expression and call the appropriate
// getDefUseLHS/RHS functions
class ExprWalker : public ROSE_VisitorPatternDefaultBase
{
  VariableIdMapping& vidm;
  // if set then we are processing an expression that modifies memory
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
  
  // recursion undwinds on basic expressions
  // that represent memory
  //  
  void visit(SgVarRefExp* sgn);
  void visit(SgPntrArrRefExp* sgn);
  void visit(SgPointerDerefExp* sgn);
  void visit(SgDotExp* sgn);
  void visit(SgArrowExp* sgn);

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

