#include "sage3basic.h"
#include "ReachabilityAnalysis.h"

// returns the error_XX label number or -1
//  error_0 is a valid label number (therefore -1 is returned if no label is found)
// this function only matches the RERS pattern of guarded assertions.
// The more general version will operate on the set of reachable program labels.
int  ReachabilityAnalysis::isIfWithLabeledAssert(SgNode* node) {
  if(isSgIfStmt(node)) {
    node=SgNodeHelper::getTrueBranch(node);
    RoseAst block(node);
    for(RoseAst::iterator i=block.begin();i!=block.end();++i) {
      SgNode* node2=*i;
      if(SgExprStatement* exp=isSgExprStatement(node2))
        node2=SgNodeHelper::getExprStmtChild(exp);
      if(isSgLabelStatement(node2)) {
        RoseAst::iterator next=i;
        next++;
        if(SgNodeHelper::Pattern::matchAssertExpr(*next)) {
          //cout<<"DEBUG: ASSERT with Label found:"<<endl;
          SgLabelStatement* labStmt=isSgLabelStatement(*i);
          assert(labStmt);
          string name=SgNodeHelper::getLabelName(labStmt);
          if(name=="globalError")
            name="error_60";
          name=name.substr(6,name.size()-6);
          std::istringstream ss(name);
          int num;
          ss>>num;
          //cout<<"DEBUG: label nr: "<<num<<endl;
          return num;
        }
      }
    }
  }
  return -1;
}

int  ReachabilityAnalysis::isConditionOfIfWithLabeledAssert(SgNode* node) {
  if(SgNodeHelper::isCond(node)) {
    node=node->get_parent();
    if(isSgExprStatement(node))
      node=node->get_parent();
    ROSE_ASSERT(isSgIfStmt(node)||isSgWhileStmt(node)||isSgDoWhileStmt(node));
    return isIfWithLabeledAssert(node);
  } else {
    return -1;
  }
}

// will be adapted once a more general version of determining reachable program labels exists
PropertyValueTable ReachabilityAnalysis::fiReachabilityAnalysis(Labeler& labeler, FIConstAnalysis& fiConstAnalysis) {
  PropertyValueTable reachabilityResults(100); // TODO: determine number of error labels
  LabelSet trueConditionLabelSet=fiConstAnalysis.getTrueConditions();
  LabelSet falseConditionLabelSet=fiConstAnalysis.getFalseConditions();
  /*
  for(LabelSet::iterator i=trueConditionLabelSet.begin();i!=trueConditionLabelSet.end();++i) {
    SgNode* cond=labeler.getNode(*i);
    int assertCode=isConditionOfIfWithLabeledAssert(cond);
    if(assertCode>=0)
      reachabilityResults.reachable(assertCode);
  }
  */
  for(LabelSet::iterator i=falseConditionLabelSet.begin();i!=falseConditionLabelSet.end();++i) {
    SgNode* cond=labeler.getNode(*i);
    int assertCode=isConditionOfIfWithLabeledAssert(cond);
    if(assertCode>=0)
      reachabilityResults.nonReachable(assertCode);
  }
  return reachabilityResults;
}

#if 0
  int fiReachabilityAnalysis() {
    // determine expressions of blocks/ifstatements
    SgExpression* exp=0;
    if(isSgIfStmt(*i)||isSgWhileStmt(*i)||isSgDoWhileStmt(*i)) {
      SgNode* node=SgNodeHelper::getCond(*i);
      if(isSgExprStatement(node)) {
        node=SgNodeHelper::getExprStmtChild(node);
      }
      //cout<<node->class_name()<<";";
      exp=isSgExpression(node);
      if(exp) {
        AbstractValue res=fiConstAnalysis.eval(exp);
        int assertCode=isIfWithLabeledAssert(*i);
        if(assertCode>=0) {
          if(res.isTrue()) {
            reachabilityResults.reachable(assertCode);
          }
          if(res.isFalse()) {
            reachabilityResults.nonReachable(assertCode);
          }
        }
        //cout<<"\nELIM:"<<res.toString()<<":"<<exp->unparseToString()<<endl;
      }
    }
  }
#endif
