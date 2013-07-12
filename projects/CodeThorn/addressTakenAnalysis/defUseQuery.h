#ifndef _DEFUSEQUERY_H
#define _DEFUSEQUERY_H

#include "addressTakenAnalysis.h"

typedef std::pair<VariableIdSet, bool> MemObj;

/*************************************************
 ******************** MemObj *********************
 *************************************************/
// class MemObj
// {
//   VariableIdSet set;
//   bool pointerAccessed;
// public:
//   MemObj() : pointerAccessed(false) { }
//   bool isAccessedByPointer();
//   void insert(VariableId _vid);
//   void setVariableIdSet(VariableIdSet that);
// };

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


MemObj getDefMemObj(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa);
MemObj getUseMemObj(SgNode* sgn, FlowInsensitivePointerAnalysis& fipa);


#endif
