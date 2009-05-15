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
	SgFunctionSymbol* roseConvertIntToString;
	SgFunctionSymbol* roseCallStack;

	RtedSymbols() {
		roseCreateArray = NULL;
		roseArrayAccess = NULL;
		roseRtedClose = NULL;
		roseConvertIntToString=NULL;
		roseCallStack = NULL;
	};
	
	virtual ~RtedSymbols() {
	}

	// Traverse all nodes and check properties
	virtual void visit(SgNode* n) ;

};

#endif
