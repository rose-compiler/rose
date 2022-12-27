#ifndef ROSE_CodeGen_SymbolScanner_H
#define ROSE_CodeGen_SymbolScanner_H

#include <Rose/CodeGen/Object.h>

#include <vector>
#include <map>
#include <string>

namespace Rose { namespace CodeGen {

#define DEBUG_Rose_CodeGen_SymbolScanner_void 0

template <typename CRT>
struct SymbolScannerBase : public ROSE_VisitTraversal {

  constexpr inline CRT * crt() { return static_cast<CRT*>(this); }
  constexpr inline CRT const * crt() const { return static_cast<CRT const *>(this); }

  void visit(SgNode * node);
};

template <typename API>
struct SymbolScanner : public SymbolScannerBase<SymbolScanner<API>> {
  API & api;

  SymbolScanner(API & api_) : api(api_) {}

  template <typename SymT>
  void visit(SymT * sym, std::map<std::string, SymT * API::* > const & objmap);
};

#define DEBUG_Rose_CodeGen_SymbolScanner_void 0

template <>
struct SymbolScanner<void> : public SymbolScannerBase<SymbolScanner<void>> {
  struct api_t {
    std::map<std::string, std::set<SgNamespaceSymbol *> > namespaces;
    std::map<std::string, std::set<SgClassSymbol *> > classes;
    std::map<std::string, std::set<SgTypedefSymbol *> > typedefs;
    std::map<std::string, std::set<SgVariableSymbol *> > variables;
    std::map<std::string, std::set<SgFunctionSymbol *> > functions;
  } api;

  SymbolScanner();

  template <typename SymT>
  void visit(SymT * sym, std::map<std::string, std::set<SymT *> > & objmap);
};

} }

#include "Rose/CodeGen/SymbolScanner.txx"

#endif /* ROSE_CodeGen_SymbolScanner_H */
