#ifndef SYMBOL_FINDER_H
#define SYMBOL_FINDER_H

#include <string>

class SymbolFinder
{
 public:
  static bool symbol_exists(std::string symbolName, SgScopeStatement* scope);
};
#endif
