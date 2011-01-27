#ifndef RTEDSYM_H
#define RTEDSYM_H

#include "rose.h"

bool RTEDDEBUG();

/* -----------------------------------------------------------
 * tps : 6March 2009: This class adds transformations
 * so that runtime errors are caught at runtime before they happen
 * -----------------------------------------------------------*/
struct RtedSymbols : AstSimpleProcessing {
	SgFunctionSymbol*   roseCreateHeapArr;
	SgFunctionSymbol*   roseCreateHeapPtr;
	SgFunctionSymbol*   roseAccessHeap;
	SgFunctionSymbol*   roseCheckpoint;
	SgFunctionSymbol*   roseFunctionCall;
	SgFunctionSymbol*   roseAssertFunctionSignature;
	SgFunctionSymbol*   roseConfirmFunctionSignature;
	SgFunctionSymbol*   roseFreeMemory;
	SgFunctionSymbol*   roseReallocateMemory;
	SgFunctionSymbol*   roseConvertIntToString;
	SgFunctionSymbol*   roseCreateVariable;
	SgFunctionSymbol*   roseCreateObject;
	SgFunctionSymbol*   roseInitVariable;
	SgFunctionSymbol*   roseMovePointer;
	SgFunctionSymbol*   roseAccessVariable;
	SgFunctionSymbol*   roseEnterScope;
	SgFunctionSymbol*   roseExitScope;
	SgFunctionSymbol*   roseIOFunctionCall;
	SgFunctionSymbol*   roseRegisterTypeCall;
	SgFunctionSymbol*   roseCheckIfThisNULL;
	SgFunctionSymbol*   roseAddr;
	SgFunctionSymbol*   roseAddrSh;
	SgFunctionSymbol*   roseClose;

	// symbols for UPC
	SgFunctionSymbol*   roseProcessMsg;

	SgEnumDeclaration*  roseAllocKind;

	SgType*             roseTypeDesc;
	SgType*             roseAddressDesc;
	SgType*             roseSourceInfo;
	SgTypedefType*      size_t_member;

	RtedSymbols()
  : roseCreateHeapArr(NULL),
	  roseCreateHeapPtr(NULL),
    roseAccessHeap(NULL),
    roseCheckpoint(NULL),
    roseFunctionCall(NULL),
    roseAssertFunctionSignature(NULL),
    roseConfirmFunctionSignature(NULL),
    roseFreeMemory(NULL),
    roseReallocateMemory(NULL),
    roseConvertIntToString(NULL),
    roseCreateVariable(NULL),
    roseCreateObject(NULL),
    roseInitVariable(NULL),
		roseMovePointer(NULL),
    roseAccessVariable(NULL),
    roseEnterScope(NULL),
    roseExitScope(NULL),
    roseIOFunctionCall(NULL),
    roseRegisterTypeCall(NULL),
    roseCheckIfThisNULL(NULL),
		roseAddr(NULL),
		roseAddrSh(NULL),
		roseClose(NULL),

		roseProcessMsg(NULL),

		roseAllocKind(NULL),

		roseTypeDesc(NULL),
		roseAddressDesc(NULL),
		roseSourceInfo(NULL),
    size_t_member(NULL)
	{}

	// Traverse all nodes and check properties
	virtual void visit(SgNode* n) ;

	static const std::string prefix;
};

#endif
