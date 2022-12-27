
#include "Rose/CodeGen/SymbolScanner.h"

namespace Rose { namespace CodeGen {

SymbolScanner<void>::SymbolScanner() : SymbolScannerBase<SymbolScanner<void>>(), api() {
  SgNamespaceSymbol::traverseMemoryPoolNodes(*this);

  SgClassSymbol::traverseMemoryPoolNodes(*this);
  SgTemplateClassSymbol::traverseMemoryPoolNodes(*this);

  SgTypedefSymbol::traverseMemoryPoolNodes(*this);
  SgTemplateTypedefSymbol::traverseMemoryPoolNodes(*this);

  SgVariableSymbol::traverseMemoryPoolNodes(*this);
  SgTemplateVariableSymbol::traverseMemoryPoolNodes(*this);

  SgFunctionSymbol::traverseMemoryPoolNodes(*this);
  SgMemberFunctionSymbol::traverseMemoryPoolNodes(*this);
  SgTemplateFunctionSymbol::traverseMemoryPoolNodes(*this);
  SgTemplateMemberFunctionSymbol::traverseMemoryPoolNodes(*this);
}

} }

