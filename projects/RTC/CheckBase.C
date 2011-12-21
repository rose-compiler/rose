#ifndef CHECKBASE_C
#define CHECKBASE_C
#include "CheckBase.h"

CheckContainer CheckBase::BinOpChecks;
CheckContainer CheckBase::UnaryOpChecks;
TraversalBase* CheckBase::TBObj;
CheckBase* 	   TraversalBase::CBObj;
SgProject*	   CheckBase::CBProject;

#if 0
void processBinaryOpChecks(SgBinaryOp* BinOp) {
	
	for(CheckContainer::iterator objit = CheckBase::BinOpChecks.begin(); objit != CheckBase::BinOpChecks.end();
				objit++) {

		CheckBase* CB = *objit;
		printf("%lu\n", &(*CB));
		CB->process(BinOp);
		checkbaseobject->process(BinOp);
	}
}

void processMultipleBinaryOpChecks(BopContainer& BinOps) {
		
	for(BopContainer::iterator bpit = BinOps.begin(); bpit != BinOps.end();
		bpit++) {

		processBinaryOpChecks(*bpit);
	}
}
#endif


#if 1
void CheckBase::process(SgNode*) {
	printf("CheckBase::process\n");
}

void CheckBase::instrument(SgNode*) {
	printf("CheckBase::instrument SgNode\n");
}

void CheckBase::instrument() {
    printf("CheckBase::instrument\n");
}
#endif

#endif
