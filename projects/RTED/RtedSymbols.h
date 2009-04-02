#ifndef RTEDSYM_H
#define RTEDSYM_H

/* -----------------------------------------------------------
 * tps : 6March 2009: This class adds transformations
 * so that runtime errors are caught at runtime before they happen
 * -----------------------------------------------------------*/
class RtedSymbols: public AstSimpleProcessing {
private:

public:
	SgMemberFunctionSymbol* roseCreateArray;
	SgMemberFunctionSymbol* roseArrayAccess;
	SgMemberFunctionSymbol* roseRtedClose;

	RtedSymbols() {
		roseCreateArray = NULL;
		roseArrayAccess = NULL;
		roseRtedClose = NULL;
	}
	;
	virtual ~RtedSymbols() {
	}

	// Traverse all nodes and check properties
	virtual void visit(SgNode* n) ;

};

#endif
