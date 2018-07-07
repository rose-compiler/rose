#include "SymbolTablePrinter.h"

void SymbolTablePrinter::visit(SgNode* node)
{
  if (isSgBasicBlock(node))
  {
    SageInterface::outputLocalSymbolTables(node);
  }
}
