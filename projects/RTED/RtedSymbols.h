#ifndef RTEDSYM_H
#define RTEDSYM_H

/* -----------------------------------------------------------
 * tps : 6March 2009: This class adds transformations
 * so that runtime errors are caught at runtime before they happen
 * -----------------------------------------------------------*/
class RtedSymbols: public AstSimpleProcessing {
private:

public:
	SgFunctionSymbol* roseCreateArray;
	SgFunctionSymbol* roseArrayAccess;
	SgFunctionSymbol* roseRtedClose;
	SgFunctionSymbol* roseFunctionCall;
	SgFunctionSymbol* roseAssertFunctionSignature;
	SgFunctionSymbol* roseConfirmFunctionSignature;
	SgFunctionSymbol* roseFreeMemory;
	SgFunctionSymbol* roseReallocateMemory;
	SgFunctionSymbol* roseConvertIntToString;
	SgFunctionSymbol* roseCreateVariable;
	SgFunctionSymbol* roseInitVariable;
	SgFunctionSymbol* roseMovePointer;
	SgFunctionSymbol* roseAccessVariable;
	SgFunctionSymbol* roseEnterScope;
	SgFunctionSymbol* roseExitScope;
	SgFunctionSymbol* roseIOFunctionCall;
	SgFunctionSymbol* roseRegisterTypeCall;
	SgType* size_t_member;

	RtedSymbols() {
		roseCreateArray = NULL;
		roseArrayAccess = NULL;
		roseRtedClose = NULL;
        roseFunctionCall = NULL;
		roseAssertFunctionSignature = NULL;
		roseConfirmFunctionSignature = NULL;
        roseFreeMemory = NULL;
        roseReallocateMemory = NULL;
		roseConvertIntToString=NULL;
		roseCreateVariable = NULL;
		roseInitVariable = NULL;
		roseAccessVariable = NULL;
		roseEnterScope = NULL;
		roseExitScope = NULL;
		roseIOFunctionCall=NULL;
		roseRegisterTypeCall=NULL;
		size_t_member=NULL;
	};
	
	virtual ~RtedSymbols() {
	}

	// Traverse all nodes and check properties
	virtual void visit(SgNode* n) ;

};

#endif
