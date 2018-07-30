#ifndef SYMBOL_TABLE_PRINTER
#define SYMBOL_TABLE_PRINTER

#include "rose.h"

struct SymbolTablePrinter : AstSimpleProcessing
{
  void visit(SgNode* node);
};

#endif
