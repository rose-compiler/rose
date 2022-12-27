
#ifndef ROSE_CodeGen_SymbolScanner_TXX
#define ROSE_CodeGen_SymbolScanner_TXX

#include "sage3basic.h"

#include <iostream>

namespace Rose { namespace CodeGen {

template <typename CRT>
void SymbolScannerBase<CRT>::visit(SgNode * node) {
  switch (node->variantT()) {
    case V_SgNamespaceSymbol:              crt()->template visit<SgNamespaceSymbol> ( (SgNamespaceSymbol*)node, crt()->api.namespaces); break;

    case V_SgClassSymbol:                  crt()->template visit<SgClassSymbol>     ( (SgClassSymbol*)node,     crt()->api.classes);    break;
    case V_SgTemplateClassSymbol:          crt()->template visit<SgClassSymbol>     ( (SgClassSymbol*)node,     crt()->api.classes);    break;

    case V_SgTypedefSymbol:                crt()->template visit<SgTypedefSymbol>   ( (SgTypedefSymbol*)node,   crt()->api.typedefs);   break;
    case V_SgTemplateTypedefSymbol:        crt()->template visit<SgTypedefSymbol>   ( (SgTypedefSymbol*)node,   crt()->api.typedefs);   break;

    case V_SgVariableSymbol:               crt()->template visit<SgVariableSymbol>  ( (SgVariableSymbol*)node,  crt()->api.variables);  break;
    case V_SgTemplateVariableSymbol:       crt()->template visit<SgVariableSymbol>  ( (SgVariableSymbol*)node,  crt()->api.variables);  break;

    case V_SgFunctionSymbol:               crt()->template visit<SgFunctionSymbol>  ( (SgFunctionSymbol*)node,  crt()->api.functions);  break;
    case V_SgMemberFunctionSymbol:         crt()->template visit<SgFunctionSymbol>  ( (SgFunctionSymbol*)node,  crt()->api.functions);  break;
    case V_SgTemplateFunctionSymbol:       crt()->template visit<SgFunctionSymbol>  ( (SgFunctionSymbol*)node,  crt()->api.functions);  break;
    case V_SgTemplateMemberFunctionSymbol: crt()->template visit<SgFunctionSymbol>  ( (SgFunctionSymbol*)node,  crt()->api.functions);  break;

    default:
      ROSE_ABORT();
  }
}

template <typename API>
template <typename SymT>
void SymbolScanner<API>::visit(SymT * sym, std::map<std::string, SymT * API::* > const & objmap) {
  auto str = sym->get_declaration()->get_qualified_name().getString();
  auto it = objmap.find(str);
  if (it != objmap.end()) {
    api.*(it->second) = sym;
  }
}

#define DEBUG_Rose_CodeGen_SymbolScanner_void 0

template <typename SymT>
void SymbolScanner<void>::visit(SymT * sym, std::map<std::string, std::set<SymT *> > & objmap) {
#if DEBUG_Rose_CodeGen_SymbolScanner_void
  std::cout << "SymbolScanner::visit( sym = " << sym << " : " << sym->class_name() << " )" << std::endl;
  std::cout << "  sym->get_declaration() = " << sym->get_declaration() << " : " << sym->get_declaration()->class_name() << ")" << std::endl;
  std::cout << "  sym->get_scope() = " << sym->get_scope() << " : " << sym->get_scope()->class_name() << ")" << std::endl;
  if (sym->get_scope()) {
    std::cout << "  sym->get_scope()->get_parent() = " << sym->get_scope()->get_parent() << " : " << sym->get_scope()->get_parent()->class_name() << ")" << std::endl;
  }
#endif
  auto str = sym->get_declaration()->get_qualified_name().getString();
#if DEBUG_Rose_CodeGen_SymbolScanner_void
  std::cout << "  str = " << str << std::endl;
#endif
  objmap[str].insert(sym);
}

} }

#endif /* ROSE_CodeGen_SymbolScanner_TXX */

