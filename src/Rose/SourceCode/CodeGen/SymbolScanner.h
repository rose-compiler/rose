#ifndef ROSE_SourceCode_CodeGen_SymbolScanner_H
#define ROSE_SourceCode_CodeGen_SymbolScanner_H

#include <Rose/SourceCode/CodeGen/Object.h>

#include <vector>
#include <map>
#include <string>

namespace Rose { namespace SourceCode { namespace CodeGen {

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

} } }

#include "Rose/SourceCode/CodeGen/SymbolScanner.txx"

#endif /* ROSE_SourceCode_CodeGen_SymbolScanner_H */
