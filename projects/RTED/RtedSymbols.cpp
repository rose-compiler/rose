#include "rose.h"
#include "RtedSymbols.h"

bool RTEDDEBUG() { return false; }

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
  static const std::string prefix("rted_");

  std::string sym(prefix);

  sym.append(name);

  SgFunctionSymbol* func = find_sym(n, sym, res);
  if (func == NULL) return;

  res = func;
  if (RTEDDEBUG()) std::cerr << "Found MemberName : " << name << std::endl;
}

static inline
void initialize(SgScopeStatement* n, RtedSymbols& rtedsym)
{
  ROSE_ASSERT(n != NULL);

  lookup(*n, "CreateHeapArr",            rtedsym.roseCreateHeapArr);
  lookup(*n, "CreateHeapPtr",            rtedsym.roseCreateHeapPtr);
  lookup(*n, "AccessHeap",               rtedsym.roseAccessHeap);
  lookup(*n, "Checkpoint",               rtedsym.roseCheckpoint);
  lookup(*n, "FunctionCall",             rtedsym.roseFunctionCall);
  lookup(*n, "AssertFunctionSignature",  rtedsym.roseAssertFunctionSignature);
  lookup(*n, "ConfirmFunctionSignature", rtedsym.roseConfirmFunctionSignature);
  lookup(*n, "ConvertIntToString",       rtedsym.roseConvertIntToString);
  lookup(*n, "CreateVariable",           rtedsym.roseCreateVariable);
  lookup(*n, "CreateObject",             rtedsym.roseCreateObject);
  lookup(*n, "InitVariable",             rtedsym.roseInitVariable);
  lookup(*n, "MovePointer",              rtedsym.roseMovePointer);
  lookup(*n, "EnterScope",               rtedsym.roseEnterScope);
  lookup(*n, "ExitScope",                rtedsym.roseExitScope);
  lookup(*n, "AccessVariable",           rtedsym.roseAccessVariable);
  lookup(*n, "IOFunctionCall",           rtedsym.roseIOFunctionCall);
  lookup(*n, "RegisterTypeCall",         rtedsym.roseRegisterTypeCall);
  lookup(*n, "FreeMemory",               rtedsym.roseFreeMemory);
  lookup(*n, "ReallocateMemory",         rtedsym.roseReallocateMemory);
  lookup(*n, "CheckIfThisNULL",          rtedsym.roseCheckIfThisNULL);
}


void RtedSymbols::visit(SgNode* n)
{
  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

  SgScopeStatement*     n_scope = isSgScopeStatement(n);

  if (n_scope) {
    initialize(n_scope, *this);
    return;
  }

  SgTypedefDeclaration* n_typedef = isSgTypedefDeclaration(n);

  if (n_typedef) {
		std::string name = n_typedef->get_name().str();

    if (name=="size_t") {
			SgType* baseType = n_typedef->get_base_type();

      size_t_member = baseType;
			if (RTEDDEBUG())
			  std::cerr << "Found Type : " << name << std::endl;
		}
  }

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************
}
