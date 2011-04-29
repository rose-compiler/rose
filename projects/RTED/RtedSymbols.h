#ifndef RTEDSYM_H
#define RTEDSYM_H

bool RTEDDEBUG();

/* -----------------------------------------------------------
 * tps : 6March 2009: This class adds transformations
 * so that runtime errors are caught at runtime before they happen
 * -----------------------------------------------------------*/
class RtedSymbols: public AstSimpleProcessing {
private:

public:
	SgFunctionSymbol* roseCreateHeap;
	SgFunctionSymbol* roseAccessHeap;
	SgFunctionSymbol* roseCheckpoint;
	SgFunctionSymbol* roseFunctionCall;
	SgFunctionSymbol* roseAssertFunctionSignature;
	SgFunctionSymbol* roseConfirmFunctionSignature;
	SgFunctionSymbol* roseFreeMemory;
	SgFunctionSymbol* roseReallocateMemory;
	SgFunctionSymbol* roseConvertIntToString;
	SgFunctionSymbol* roseCreateVariable;
#if NOT_YET_IMPLEMENTED
	SgFunctionSymbol* roseCreateSharedVariable; // for upc
#endif /* NOT_YET_IMPLEMENTED */
	SgFunctionSymbol* roseCreateObject;
	SgFunctionSymbol* roseInitVariable;
	SgFunctionSymbol* roseMovePointer;
	SgFunctionSymbol* roseAccessVariable;
	SgFunctionSymbol* roseEnterScope;
	SgFunctionSymbol* roseExitScope;
	SgFunctionSymbol* roseIOFunctionCall;
	SgFunctionSymbol* roseRegisterTypeCall;
	SgFunctionSymbol* roseCheckIfThisNULL;
	SgType* size_t_member;

	RtedSymbols()
  : roseCreateHeap(NULL),
    roseAccessHeap(NULL),
    roseCheckpoint(NULL),
    roseFunctionCall(NULL),
    roseAssertFunctionSignature(NULL),
    roseConfirmFunctionSignature(NULL),
    roseFreeMemory(NULL),
    roseReallocateMemory(NULL),
    roseConvertIntToString(NULL),
    roseCreateVariable(NULL),
#if NOT_YET_IMPLEMENTED
    roseCreateSharedVariable(NULL),
#endif /* NOT_YET_IMPLEMENTED */
    roseCreateObject(NULL),
    roseInitVariable(NULL),
    roseAccessVariable(NULL),
    roseEnterScope(NULL),
    roseExitScope(NULL),
    roseIOFunctionCall(NULL),
    roseRegisterTypeCall(NULL),
    roseCheckIfThisNULL(NULL),
    size_t_member(NULL)
	{}

	virtual ~RtedSymbols() {
	}

	// Traverse all nodes and check properties
	virtual void visit(SgNode* n) ;

};

#endif
