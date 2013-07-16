#ifndef _DEFUSEQUERY_H
#define _DEFUSEQUERY_H

#include "addressTakenAnalysis.h"

//typedef std::pair<VariableIdSet, bool> MemObj;

/*************************************************
 ***************** DefUseMemObj ******************
 *************************************************/

// determined completely based on syntactic information
// def_set consists of VariableIds which is written by the expression
// use_set consists of VariableIds which are read but not modified by this expression
// consider using references for efficiency
class DefUseMemObj
{
  VariableIdSet def_set;
  VariableIdSet use_set;
  bool ptr_modify;
  bool func_modify;

public:
  DefUseMemObj() : ptr_modify(false), func_modify(false) { }
  DefUseMemObj(const VariableIdSet& _def_set, const VariableIdSet& _use_set);
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
  DefUseMemObj operator+(const DefUseMemObj& dumo1);

  std::string str();
  // for more readability
  std::string str(VariableIdMapping& vidm);
};

// used by the getDefUseMemObj_rec to traverse the 
// structure of the expression and call the appropriate
// getDefUseLHS/RHS functions
class ExprVisitorPattern : public ROSE_VisitorPatternDefaultBase
{
  FlowInsensitivePointerAnalysis& fipa;
  DefUseMemObj dumo;

public:
  ExprVisitorPattern(FlowInsensitivePointerAnalysis& _fipa, DefUseMemObj _dumo);
  // lhs of assignment operator are always lvalues
  // process them 
  void visit(SgAssignOp* sgn);
  void visit(SgCompoundAssignOp* sgn);

  // recurse on sub-expressions
  void visit(SgBinaryOp* sgn);
  void visit(SgCastExp* sgn);
  
  // recursion undwinds on basic expressions
  // that represent memory
  //  
  void visit(SgVarRefExp* sgn);
  void visit(SgPntrArrRefExp* sgn);
  void visit(SgPointerDerefExp* sgn);
  void visit(SgDotExp* sgn);
  void visit(SgArrowExp* sgn);

  DefUseMemObj getDefUseMemObj();
};

class LvalueVisitorPattern : public ROSE_VisitorPatternDefaultBase
{
  FlowInsensitivePointerAnalysis& fipa;
  VariableIdMapping& vidm;
  DefUseMemObj dumo;
  bool isOnLHS;
public:
  LvalueVisitorPattern(FlowInsensitivePointerAnalysis& _fipa, VariableIdMapping& _vidm, DefUseMemObj _dumo, bool _isOnLHS);
  void visit(SgVarRefExp* sgn);
  void visit(SgPntrArrRefExp* sgn);
  void visit(SgPointerDerefExp* sgn);
  void visit(SgDotExp* sgn);
  void visit(SgArrowExp* sgn);

  DefUseMemObj getDefUseMemObj();
};


// interface function to
// identify modified and used locations based on syntax
//  a[i] = expr; a is modified and i, expr are used 
// *p = expr; since we dont know anything about p
// the locations that are modified involve all arrays and variables in addressTakenSet
// if the expression involve functions or function pointers it can 
// potentially modify everything
DefUseMemObj getDefUseMemObj(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa);

// breaks the expression based on '=' and calls the appropriate
// functions for LHS and RHS
DefUseMemObj getDefUseMemObj_rec(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa, DefUseMemObj dumo);

// used to process the lhs of assignment operator
// invokes a visitor pattern and adds the modified variables
// to def_set and used variables to use_set of dumo object
DefUseMemObj getDefUseMemObjLHS(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa, DefUseMemObj dumo);

// used to process the rhs of assignment operator
// invokes a visitor pattern that adds used variables
// to use_set and the modified variables such as sideffects
// through expressions to def_set of dumo object
DefUseMemObj getDefUseMemObjRHS(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa, DefUseMemObj dumo);


#endif

