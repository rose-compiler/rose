#ifndef CHECKBASE_H
#define CHECKBASE_H
#include <cstddef>
#include <cstdlib>
#include <stdio.h>

#include "rose.h"
#include <vector>
#include "RTC.h"

class CheckBase;
class TraversalBase;


typedef std::vector<CheckBase*> CheckContainer;
typedef std::vector<SgBinaryOp*> BopContainer;

typedef Rose_STL_Container<SgNode*> NodeContainer;
//extern CheckBase* checkbaseobject;

void insertTimingSupport(SgProject *project);

#if 0
void processBinaryOpChecks(SgBinaryOp*);
void processMultipleBinaryOpChecks(BopContainer&);
#endif

class CheckBase {
	
	private:
	std::vector<SgNode*> SelectedNodes;

	

	public:

	static CheckContainer BinOpChecks;
	static CheckContainer UnaryOpChecks;

    static SgProject* CBProject;
	static TraversalBase *TBObj;

	static void RegisterBinaryOpCheck(CheckBase* CB) {
		BinOpChecks.push_back(CB);
	}

	static void RegisterUnaryOpCheck(CheckBase* CB) {
		UnaryOpChecks.push_back(CB);
	}

	#if 1
	static void processBinaryOpChecks(SgBinaryOp* BinOp) {
		
		for(CheckContainer::iterator objit = BinOpChecks.begin(); objit != BinOpChecks.end();
					objit++) {

			CheckBase* CB = *objit;
			//printf("%lu\n", &(*CB));
			CB->process(BinOp);
		//	checkbaseobject->process(BinOp);
		}
	}

	static void processMultipleBinaryOpChecks(BopContainer& BinOps) {
		
		// Optionally, you could call the process(BopContainer&) from here itself
		#if 1		
		for(CheckContainer::iterator objit = BinOpChecks.begin(); objit != BinOpChecks.end();
					objit++) {

			CheckBase* CB = *objit;
			CB->process(BinOps);
		}

		#else
		// This uses processBinaryOpChecks to spoon feed each one to the corresponding
		// check (eg. ArithCheck)
		for(BopContainer::iterator bpit = BinOps.begin(); bpit != BinOps.end();
			bpit++) {

			CheckBase::processBinaryOpChecks(*bpit);
		}
		#endif
	}
	#endif


	virtual void registerCheck() { printf("CheckBase: registerCheck\n"); }

	virtual void process(SgNode*);

	virtual void process(SgBinaryOp*) {};

	virtual void process(BopContainer&) {};

	virtual void process(SgAddOp*) {};

	virtual void process(SgSubtractOp*) {};

	virtual void process(SgMultiplyOp*) {};

	virtual void process(SgDivideOp*) {};


	static void naiveInstrumentation() {
		
		for(CheckContainer::iterator objit = BinOpChecks.begin(); objit != BinOpChecks.end();
					objit++) {

			CheckBase* CB = *objit;
			CB->instrument();
		}
	}

    virtual void instrument(SgNode*);

	virtual void instrument();

	virtual void instrument(SgBinaryOp*) {};

	virtual void instrument(SgAddOp*) {};

	virtual void instrument(SgSubtractOp*) {};

	virtual void instrument(SgMultiplyOp*) {};

	virtual void instrument(SgDivideOp*) {};


	virtual ~CheckBase() {};	

};

class TraversalBase {

	public:
	static NodeContainer VarNames;
	static NodeContainer MallocRefs;
	static NodeContainer VarRefs;

	static CheckBase *CBObj;

	virtual void nodeCollection(SgProject*) = 0;

	virtual void nodeInstrumentation(SgProject*) = 0;

	virtual ~TraversalBase() {};

	static void processBinaryOps(BopContainer& BinOps) {
		CheckBase::processMultipleBinaryOpChecks(BinOps);
	}

	static void naiveInstrumentation() {
		CheckBase::naiveInstrumentation();
	}
};

#endif
