#include "rose.h"

#include <boost/algorithm/string/predicate.hpp>

#include "RtedSymbols.h"
#include "rosez.hpp"

bool RTEDDEBUG() { return false; }

bool isRtedDecl(const std::string& name)
{
  return boost::starts_with(name, RtedSymbols::prefix);
}

static inline
SgFunctionSymbol*
find_sym(SgScopeStatement& n, const std::string& name, SgFunctionSymbol*)
{
  return n.lookup_function_symbol(name);
}

static inline
SgClassSymbol*
find_sym(SgScopeStatement& n, const std::string& name, SgClassSymbol*)
{
  return n.lookup_class_symbol(name);
}

template <class T>
static inline
void lookup(SgScopeStatement& n, const std::string& name, T*& res)
{
  std::string sym(RtedSymbols::prefix);

  sym.append(name);

  SgFunctionSymbol* func = find_sym(n, sym, res);
  if (func == NULL) return;

  res = func;
  if (RTEDDEBUG()) std::cerr << "Found MemberName : " << name << std::endl;
}

static inline
void initialize(SgScopeStatement& n, RtedSymbols& rtedsym)
{
  lookup(n, "CreateHeapArr",            rtedsym.roseCreateHeapArr);
  lookup(n, "CreateHeapPtr",            rtedsym.roseCreateHeapPtr);
  lookup(n, "AccessHeap",               rtedsym.roseAccessHeap);
  lookup(n, "Checkpoint",               rtedsym.roseCheckpoint);
  lookup(n, "FunctionCall",             rtedsym.roseFunctionCall);
  lookup(n, "AssertFunctionSignature",  rtedsym.roseAssertFunctionSignature);
  lookup(n, "ConfirmFunctionSignature", rtedsym.roseConfirmFunctionSignature);
  lookup(n, "ConvertIntToString",       rtedsym.roseConvertIntToString);
  lookup(n, "CreateVariable",           rtedsym.roseCreateVariable);
  lookup(n, "CreateObject",             rtedsym.roseCreateObject);
  lookup(n, "InitVariable",             rtedsym.roseInitVariable);
  lookup(n, "MovePointer",              rtedsym.roseMovePointer);
  lookup(n, "EnterScope",               rtedsym.roseEnterScope);
  lookup(n, "ExitScope",                rtedsym.roseExitScope);
  lookup(n, "AccessVariable",           rtedsym.roseAccessVariable);
  lookup(n, "IOFunctionCall",           rtedsym.roseIOFunctionCall);
  lookup(n, "RegisterTypeCall",         rtedsym.roseRegisterTypeCall);
  lookup(n, "FreeMemory",               rtedsym.roseFreeMemory);
  lookup(n, "ReallocateMemory",         rtedsym.roseReallocateMemory);
  lookup(n, "CheckIfThisNULL",          rtedsym.roseCheckIfThisNULL);
  lookup(n, "Addr",                     rtedsym.roseAddr);
  lookup(n, "AddrSh",                   rtedsym.roseAddrSh);
  lookup(n, "Close",                    rtedsym.roseClose);
}

struct RtedSymbolWrapper
{
  RtedSymbols& rs;

  explicit
  RtedSymbolWrapper(RtedSymbols& syms)
  : rs(syms)
  {}

  void handle_rtedTypes(const std::string& name, SgClassDeclaration& n)
  {
    if      (name == "rted_AddressDesc") rs.roseAddressDesc = SgClassType::createType(&n);
    else if (name == "rted_TypeDesc")    rs.roseTypeDesc    = SgClassType::createType(&n);
    else if (name == "rted_SourceInfo")  rs.roseSourceInfo  = SgClassType::createType(&n);
  }

  void handle(SgNode&)
  {}

  // \pp \todo This should probably be global scope
	void handle(SgGlobal& n)
  {
    initialize(n, rs);
  }

	void handle(SgTypedefDeclaration& n)
  {
    const std::string& name = n.get_name().str();

    if (name=="size_t")
    {
      rs.size_t_member = SgTypedefType::createType(&n);

      if (RTEDDEBUG())
        std::cerr << "Found Type : " << name << std::endl;
    }
  }

  void handle(SgClassDeclaration& n)
  {
    const std::string& name = n.get_name().str();

    if ( isRtedDecl(name) )
    {
      handle_rtedTypes(name, n);
    }
  }
};


void RtedSymbols::visit(SgNode* n)
{
  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************
  ez::visitSgNode(RtedSymbolWrapper(*this), n);
  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************
}

const std::string RtedSymbols::prefix("rted_");
