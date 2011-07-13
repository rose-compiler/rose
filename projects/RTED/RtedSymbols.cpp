#include <iostream>

#include "sageGeneric.hpp"

#include "RtedSymbols.h"

static
void presence_test(const std::string& name, void *const symptr)
{
  if (symptr) return;

  std::cerr << "#Error: Unable to locate symbol for '" << name << "'.\n"
            << "        Did you include 'RTED/RuntimeSystem.h' ?"
            << std::endl;

  exit(1);
}

static inline
SgClassSymbol*
find_sym(SgScopeStatement& n, const std::string& name, SgClassType*)
{
  SgClassSymbol* const res = n.lookup_class_symbol(name);

  presence_test(name, res);
  return res;
}

static inline
SgTypedefSymbol*
find_sym(SgScopeStatement& n, const std::string& name, SgTypedefType*)
{
  SgTypedefSymbol* const res = n.lookup_typedef_symbol(name);

  presence_test(name, res);
  return res;
}

template <class SageType>
static inline
SageType*
create_type(SgScopeStatement& n, const std::string& name, SageType* tag)
{
  return SageType::createType(find_sym(n, name, tag)->get_declaration());
}

std::string rtedIdentifier(const std::string& name)
{
  std::string res;

  res.reserve(name.size() + RtedSymbols::prefix.size());
  res.append(RtedSymbols::prefix);
  res.append(name);

  return res;
}

static
void set_typedef_type(SgScopeStatement& n, const std::string& name, SgTypedefType*& res)
{
  res = create_type(n, name, res);
  ROSE_ASSERT(res);
}

static
void set_rtedtype(SgScopeStatement& n, const std::string& name, SgClassType*& res)
{
  res = create_type(n, rtedIdentifier(name), res);
  ROSE_ASSERT(res);
}

static
void set_rtedenum(SgScopeStatement& n, const std::string& name, SgEnumDeclaration*& res)
{
  res = n.lookup_enum_symbol(rtedIdentifier(name))->get_declaration();
  ROSE_ASSERT(res);
}

static
void set_rtedfunc(SgGlobal& n, const std::string& name, SgFunctionSymbol*& res)
{
  res = n.lookup_function_symbol(rtedIdentifier(name));
  // cannot assert for upc functions :(   was: ROSE_ASSERT(res);
}

void RtedSymbols::initialize(SgGlobal& n)
{
  set_rtedfunc(n, "CreateArray",              roseCreateArray);
  set_rtedfunc(n, "AllocMem",                 roseAllocMem);
  set_rtedfunc(n, "AccessArray",              roseAccessArray);
  set_rtedfunc(n, "Checkpoint",               roseCheckpoint);
  set_rtedfunc(n, "FunctionCall",             roseFunctionCall);
  set_rtedfunc(n, "AssertFunctionSignature",  roseAssertFunctionSignature);
  set_rtedfunc(n, "ConfirmFunctionSignature", roseConfirmFunctionSignature);
  set_rtedfunc(n, "ConvertIntToString",       roseConvertIntToString);
  set_rtedfunc(n, "CreateVariable",           roseCreateVariable);
  set_rtedfunc(n, "CreateObject",             roseCreateObject);
  set_rtedfunc(n, "InitVariable",             roseInitVariable);
  set_rtedfunc(n, "MovePointer",              roseMovePointer);
  set_rtedfunc(n, "EnterScope",               roseEnterScope);
  set_rtedfunc(n, "ExitScope",                roseExitScope);
  set_rtedfunc(n, "AccessVariable",           roseAccessVariable);
  set_rtedfunc(n, "IOFunctionCall",           roseIOFunctionCall);
  set_rtedfunc(n, "RegisterTypeCall",         roseRegisterTypeCall);
  set_rtedfunc(n, "FreeMemory",               roseFreeMemory);
  set_rtedfunc(n, "ReallocateMemory",         roseReallocateMemory);
  set_rtedfunc(n, "CheckIfThisNULL",          roseCheckIfThisNULL);
  set_rtedfunc(n, "Addr",                     roseAddr);
  set_rtedfunc(n, "AddrSh",                   roseAddrSh);
  set_rtedfunc(n, "Close",                    roseClose);
  set_rtedfunc(n, "UpcExitWorkzone",          roseUpcExitWorkzone);
  set_rtedfunc(n, "UpcEnterWorkzone",         roseUpcEnterWorkzone);
  set_rtedfunc(n, "UpcAllInitialize",         roseUpcAllInitialize);
  set_rtedfunc(n, "UpcBeginExclusive",        roseUpcBeginExclusive);
  set_rtedfunc(n, "UpcEndExclusive",          roseUpcEndExclusive);

  set_rtedtype(n, "AddressDesc",              roseAddressDesc);
  set_rtedtype(n, "TypeDesc",                 roseTypeDesc);
  set_rtedtype(n, "SourceInfo",               roseSourceInfo);

  set_rtedenum(n, "AllocKind",                roseAllocKind);

  set_typedef_type(n, "size_t",               size_t_member);
}


const std::string RtedSymbols::prefix("rted_");
