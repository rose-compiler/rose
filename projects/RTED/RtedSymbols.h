#ifndef RTEDSYM_H
#define RTEDSYM_H

#include "rose.h"

struct RtedSymbols
{
  SgFunctionSymbol*   roseCreateArray;
  SgFunctionSymbol*   roseAllocMem;
  SgFunctionSymbol*   roseAccessArray;
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
  SgFunctionSymbol*   roseUpcExitWorkzone;
  SgFunctionSymbol*   roseUpcEnterWorkzone;
  SgFunctionSymbol*   roseUpcAllInitialize;
  SgFunctionSymbol*   roseUpcBeginExclusive;
  SgFunctionSymbol*   roseUpcEndExclusive;
  SgFunctionSymbol*   roseUpcEnterSharedPtr;
  SgFunctionSymbol*   roseUpcExitSharedPtr;

  SgEnumDeclaration*  roseAllocKind;

  SgClassType*        roseTypeDesc;
  SgClassType*        roseAddressDesc;
  SgClassType*        roseSourceInfo;
  SgTypedefType*      size_t_member;

  RtedSymbols()
  : roseCreateArray(NULL),
    roseAllocMem(NULL),
    roseAccessArray(NULL),
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

    roseUpcExitWorkzone(NULL),
    roseUpcEnterWorkzone(NULL),
    roseUpcAllInitialize(NULL),
    roseUpcBeginExclusive(NULL),
    roseUpcEndExclusive(NULL),
    roseUpcEnterSharedPtr(NULL),
    roseUpcExitSharedPtr(NULL),

    roseAllocKind(NULL),

    roseTypeDesc(NULL),
    roseAddressDesc(NULL),
    roseSourceInfo(NULL),
    size_t_member(NULL)
  {}

  void initialize(SgGlobal& n);

  static const std::string prefix;
};

#endif
