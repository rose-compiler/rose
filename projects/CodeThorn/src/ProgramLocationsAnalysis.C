#include "sage3basic.h"
#include "ProgramLocationsAnalysis.h"

using namespace std;
using namespace CodeThorn;

std::int32_t CodeThorn::ProgramLocationsAnalysis::numPointerDerefOps(SgNode* node) {
  std::int32_t num=0;
  switch(node->variantT()) {
  case V_SgArrowExp:
  case V_SgPointerDerefExp:
  case V_SgArrowStarOp:
    num++;
    break;
  case V_SgPntrArrRefExp: {
    SgPntrArrRefExp* aref=isSgPntrArrRefExp(node);
    SgType* type=aref->get_type();
    if(isSgPointerType(type)) {
      num++;
    }
    break;
  }
  default:
    ; // nothing to do
  }
  return num;
}

std::int32_t CodeThorn::ProgramLocationsAnalysis::numLocalArrayAccessOps(SgNode* node) {
  std::int32_t num=0;
  switch(node->variantT()) {
  case V_SgPntrArrRefExp: {
    SgPntrArrRefExp* aref=isSgPntrArrRefExp(node);
    SgType* type=aref->get_type();
    // find all non-pointer array accesses
    if(!isSgPointerType(type)) {
      num++;
    }
    break;
  }
  default:
    ; // nothing to do
  }
  return num;
}

LabelSet CodeThorn::ProgramLocationsAnalysis::pointerDereferenceLocations(Labeler& labeler) {
  LabelSet labSet;
  for(auto lab : labeler) {
    SgNode* node=labeler.getNode(lab);
    if(numPointerDerefOps(node)>0) {
      labSet.insert(lab);
    }
  }
  return labSet;
}

LabelSet CodeThorn::ProgramLocationsAnalysis::arrayAccessLocations(Labeler& labeler) {
  LabelSet labSet;
  for(auto lab : labeler) {
    SgNode* node=labeler.getNode(lab);
    if(numPointerDerefOps(node)>0||numLocalArrayAccessOps(node)>0) {
      labSet.insert(lab);
    }
  }
  return labSet;
}

LabelSet CodeThorn::ProgramLocationsAnalysis::readAccessLocations(Labeler& labeler) {
  LabelSet labSet;
  std::int32_t num=0;
  for(auto lab : labeler) {
    SgNode* node=labeler.getNode(lab);
    RoseAst stmtAst(node);
    for(auto i=stmtAst.begin();i!=stmtAst.end();++i) {
      if(SgAssignOp* assignOp=isSgAssignOp(*i)) {
        SgExpression* rhs=assignOp->get_rhs_operand();
        RoseAst rhsAst(node);
        for(auto rhsNode : rhsAst) {
          if(isSgVarRefExp(rhsNode)) {
            labSet.insert(lab);
            break;
          }
        }
        i.skipChildrenOnForward();
      }
      // TODO: r-values on lhs, e.g.: a[x]=...;=>{x}
    }
  }
  return labSet;
}

