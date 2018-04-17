#ifndef CONT_NODE_ATTRIBUTE_H
#define CONT_NODE_ATTRIBUTE_H

#include"sage3basic.h"
#include<AstAttributeMechanism.h>
#include"Analyzer.h"
#include"Flow.h"

using namespace std;

enum BranchReturns {NO, MAYBE, YES};

class ContNodeAttribute: public AstAttribute
{
	public: 
	Label contLabel;		 
	BranchReturns trueBranchReturns, falseBranchReturns;
	ContNodeAttribute(Label contLabel, BranchReturns trueBranchReturns, BranchReturns falseBranchReturns): contLabel(contLabel), trueBranchReturns(trueBranchReturns), falseBranchReturns(falseBranchReturns){} 
	virtual string toString();
	

};




#endif
