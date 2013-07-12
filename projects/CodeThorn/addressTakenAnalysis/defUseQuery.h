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
class DefUseMemObj
{
  VariableIdSet def_set;
  VariableIdSet use_set;
  bool ptrAcess;
  bool fptrPresent;
public:
  DefUseMemObj() : ptrAcess(false), fptrPresent(false) { }
};

// class to identify sets of memory involved in 
class ProcessOperands : public ROSE_VisitorPatternDefaultBase
{
  VariableIdSet set;
  FlowInsensitivePointerAnalysis& fipa;
  VariableIdMapping& vidm;
  bool isModByPointer;
public:
  ProcessOperands(FlowInsensitivePointerAnalysis& _fipa) : fipa(_fipa), vidm(_fipa.getVariableIdMapping()) { }
  void visit(SgVarRefExp* sgn);
  void visit(SgPntrArrRefExp* sgn);
  void visit(SgPointerDerefExp* sgn);
};


DefUseMemObj getDefUseMemObj(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa);
DefUseMemObj getDefUseMemObjLHS(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa);
DefUseMemObj getDefUseMemObjRHS(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa);


#endif

