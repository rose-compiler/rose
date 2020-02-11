#include "sage3basic.h"
#include "LoopInfo.h"
#include "SgNodeHelper.h"
#include "AstMatching.h"

#include <map>
#include <sstream>
#include <string>

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

LoopInfo::LoopInfo():
  forStmt(0),
  initStmt(0),
  condExpr(0),
  isOmpCanonical(false),
  iterationVarType(ITERVAR_UNKNOWN)
{
}

LoopInfo::~LoopInfo() {
  // nothing to do
}

std::string iterVarTypeToString(IterVarType iterVarType) {
  switch(iterVarType) {
  case ITERVAR_SEQ: return "sequential";
  case ITERVAR_PAR: return "parallel";
  case ITERVAR_UNKNOWN: return "unknown";
  default:
    stringstream ss;
    ss<<iterVarType;
    throw CodeThorn::Exception("undefined iterVarType (enum value "+ss.str()+")");
  }
}

std::string LoopInfo::toString() {
  string s="SgForStatement: ";
  s+=iterVarTypeToString(iterationVarType);
  return s;
}

VariableId LoopInfo::iterationVariableId(SgForStatement* forStmt, VariableIdMapping* variableIdMapping) {
  VariableId varId;
  AstMatching m;
  // operator '#' is used to ensure no nested loop is matched ('#' cuts off subtrees of 4th element (loop body)).
  string matchexpression="SgForStatement(_,_,SgAssignOp($ITERVAR=SgVarRefExp,_)|SgPlusPlusOp($ITERVAR=SgVarRefExp)|SgMinusMinusOp($ITERVAR=SgVarRefExp),..)";
    MatchResult r=m.performMatching(matchexpression,forStmt);
  if(r.size()>1) {
    //ROSE_ASSERT(r.size()==1);
    for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
      SgVarRefExp* node=isSgVarRefExp((*i)["$ITERVAR"]);
      varId=variableIdMapping->variableId(node);
      return varId;
    }
  } else {
    cout<<"WARNING: no match!"<<endl;
  }
  return varId;
}

void LoopInfo::computeOuterLoopsVarIds(VariableIdMapping* variableIdMapping) {
  ROSE_ASSERT(forStmt);
  // compute outer loops
  SgNode* node=forStmt;
  while(!isSgFunctionDefinition(node)) {
    node=node->get_parent();
    if(SgForStatement* outerForStmt=isSgForStatement(node)) {
      VariableId iterVarId=iterationVariableId(outerForStmt,variableIdMapping);
      if(iterVarId.isValid()) {
        outerLoopsVarIds.insert(iterVarId);
      } else {
        cout<<"WARNING: no iter variable detected."<<endl;
        cout<<forStmt->unparseToString()<<endl;
      }
    }
  }
}

void LoopInfo::computeLoopLabelSet(Labeler* labeler) {
  ROSE_ASSERT(forStmt);
  RoseAst ast(forStmt);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(labeler->numberOfAssociatedLabels(*i)) {
      // use getLabelSet to also include callreturn nodes
      loopLabelSet.insert(labeler->getLabel(*i));
    }
  }
}

bool LoopInfo::isInAssociatedLoop(const EState* estate) {
  Label lab=estate->label();
  ROSE_ASSERT(forStmt);
  return loopLabelSet.find(lab)!=loopLabelSet.end();
}
