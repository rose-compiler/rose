#ifndef CONT_NODE_ATTRIBUTE_H
#define CONT_NODE_ATTRIBUTE_H

#include"sage3basic.h"
#include<AstAttributeMechanism.h>
#include"CTAnalysis.h"
#include"Flow.h"

enum BranchReturns {NO, MAYBE, YES};

class ContNodeAttribute: public AstAttribute {
public: 
  CodeThorn::Label contLabel;		 
  BranchReturns trueBranchReturns, falseBranchReturns;
  ContNodeAttribute(CodeThorn::Label contLabel, BranchReturns trueBranchReturns, BranchReturns falseBranchReturns): contLabel(contLabel), trueBranchReturns(trueBranchReturns), falseBranchReturns(falseBranchReturns){} 
  virtual std::string toString();
};

#endif
