
#include "sage3basic.h"
#include "fixupTraversal.h"

namespace Rose { namespace AST {

#if defined(__cpp_range_based_for) && __cpp_range_based_for >= 200907
#define DEBUG_LinkVariableAcrossFiles 0

static SgSymbol * select_shared_variable_symbol(
  std::string const & name,
  std::map< SgSymbol *, std::pair<SgInitializedName *, SgInitializedName *> > const & sym_map
) {
  if (sym_map.size() == 1) { // No duplication across TU, skip it if it is a static (file scope)
    SgInitializedName * iname = sym_map.begin()->second.first;
    ROSE_ASSERT(iname != NULL);
    SgScopeStatement * scope = iname->get_scope();
    ROSE_ASSERT(scope != NULL);
    bool is_static_decl = iname->get_storageModifier().isStatic();
    bool has_file_visibility = ( isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope) ) && is_static_decl;
    return has_file_visibility ? NULL : sym_map.begin()->first;
  } else {
    // Duplication:
    //  - select first symbol whose symbol basis is the first iname in the chain of decls
    // FIXME: better choice ??

    for (auto p: sym_map) {
      SgInitializedName * iname = isSgInitializedName(p.first->get_symbol_basis());
      ROSE_ASSERT(iname != NULL);
      if (iname->get_prev_decl_item() == NULL) {
        return p.first;
      }
    }
    return sym_map.begin()->first;
  }
}

class LinkVariableAcrossFiles {
  private:
    struct : public ROSE_VisitTraversal {
      std::set<SgInitializedName *> inames;

      void visit(SgNode * node) {
        SgInitializedName * iname = (SgInitializedName *)node;

        SgScopeStatement * scope = iname->get_scope();
        ROSE_ASSERT(scope != NULL);

        bool is_fparam = isSgFunctionParameterList(iname->get_parent());
        bool is_valid_scope = !is_fparam && (isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope));
        bool is_variable = isSgVariableDeclaration(iname->get_parent());
        if ( is_valid_scope && is_variable ) {
          inames.insert(iname);
        }
      }
    } inames;

    struct : public ROSE_VisitTraversal {
      std::set<SgSymbol *> symbols;

      void visit(SgNode * node) {
        SgSymbol * sym = (SgSymbol *)node;

        SgScopeStatement * scope = sym->get_scope();
        ROSE_ASSERT(scope != NULL);

        bool is_valid_scope = isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope);
        bool is_variable = isSgVariableDeclaration(sym->get_symbol_basis()->get_parent());
        if ( is_valid_scope && is_variable ) {
          symbols.insert(sym);
        }
      }
    } symbols;
    
    void build_name_symbol_inames_map(
      std::map< std::string, std::map< SgSymbol *, std::pair<SgInitializedName *, SgInitializedName *> > > & name_map
    ) {
      std::set<SgInitializedName *> seens;
      std::map<SgInitializedName *, std::vector<SgInitializedName *> > first_to_rlist;

      // Following map is used when symbol does not point to the first declaration on the chain
      // This happen when a variable is declared in two headers (extern) and each header include the other header.
      // Then, when the AST from two files that include one of the header are merged, the sharing will make the symbol point to a iname that is not the first in the chain.
      // FIXME if performance issue it might be faster to preserve the other iname during sharing
      std::map<SgInitializedName *, SgInitializedName *> any_to_first;
      for (auto iname: inames.inames) {
        // skip if already seen in any list
        if (!seens.insert(iname).second) continue;

        // build longuest list so far (else it would have been seen) (could build partial list but premature optimization)
        std::vector<SgInitializedName *> rlist;
        rlist.push_back(iname);
        while (true) {
          SgInitializedName * prev = rlist.back()->get_prev_decl_item();
          if (prev == NULL) break;
          seens.insert(prev);
          rlist.push_back(prev);
        }
        SgInitializedName * first_iname = rlist.back();
        first_to_rlist[first_iname] = rlist;

        for (auto i: rlist) {
          any_to_first[i] = first_iname;
        }
      }

      for (auto sym: symbols.symbols) {
        SgInitializedName * iname = isSgInitializedName(sym->get_symbol_basis());
        ROSE_ASSERT(iname != NULL);

        iname = any_to_first[iname];
        ROSE_ASSERT(iname != NULL);

        std::vector<SgInitializedName *> & rlist = first_to_rlist[iname];
        ROSE_ASSERT(rlist.size() > 0);
        std::pair<SgInitializedName *, SgInitializedName *> p(rlist.back(), rlist[0]);
        name_map[sym->get_mangled_name().getString()][sym] = p;
      }
    }

  public:
    void apply(SgProject * project) {
      SgInitializedName::traverseMemoryPoolNodes(inames);
      SgVariableSymbol::traverseMemoryPoolNodes(symbols);

      std::map<SgNode *, SgNode *> sym_repl_map;
      std::set<SgSymbol *> sym_del_set;

      // Build map of mangled-name to map of symbol (per TU) to initialized names (first and last of the chain, chain is reversed: following `get_prev_decl_item` point lead to first from last)
      std::map< std::string, std::map< SgSymbol *, std::pair<SgInitializedName *, SgInitializedName *> > > name_map;
      build_name_symbol_inames_map(name_map);

      for (auto p: name_map) {
        ROSE_ASSERT(p.second.size() > 0);

        SgSymbol * symbol = select_shared_variable_symbol(p.first, p.second);
        if (symbol == NULL) continue; // Case of a declaration with file-scope

        SgInitializedName * first_iname = p.second[symbol].first;
        ROSE_ASSERT(first_iname != NULL);

        SgInitializedName * last_iname = p.second[symbol].second;
        ROSE_ASSERT(last_iname != NULL);

        SgScopeStatement * scope = first_iname->get_scope();
        ROSE_ASSERT(scope != NULL);

        // Alias symbol if it has project visibility
        if (isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope)) {
          SgAliasSymbol * asym = new SgAliasSymbol(symbol);
          if (isSgGlobal(scope)) {
            SgGlobal * gsaf = project->get_globalScopeAcrossFiles();
            ROSE_ASSERT(gsaf != NULL);
            gsaf->get_symbol_table()->insert(symbol->get_name(), asym);
          } else if (isSgNamespaceDefinitionStatement(scope)) {
            SgNamespaceDefinitionStatement * gnd = ((SgNamespaceDefinitionStatement *)scope)->get_global_definition();
            ROSE_ASSERT(gnd != NULL);
            gnd->get_symbol_table()->insert(symbol->get_name(), asym);
          }
        }

        // Register symbols for deletion and substitution. Patches nondef/defn declarations.
        for (auto q: p.second) {
          if (q.first != symbol) {
            sym_repl_map[q.first] = symbol;
            sym_del_set.insert(q.first);

            // We only concatenate the chain of inames associated with this symbol if the
            // symbol's basis is actually the first item of that chain. Other cases are due
            // to multiple extern declaration being seen from headers (which are read in
            // various orders) and eliminated by sharing. The result is a single chain with
            // symbols pointing to various point of that chain.
            SgInitializedName * base_iname = isSgInitializedName(q.first->get_symbol_basis());
            ROSE_ASSERT(base_iname != NULL);
            if (base_iname == q.second.first && base_iname->get_prev_decl_item() == NULL) {
              q.second.first->set_prev_decl_item(last_iname);
              last_iname = q.second.second;
            }
          }
        }
      }

      for (auto s: sym_del_set) {
        SgSymbolTable * table = isSgSymbolTable(s->get_parent());
        ROSE_ASSERT(table != NULL);
        table->remove(s);
        delete s;
      }
      fixupTraversal(sym_repl_map);
    }
};
#endif

void link_variable(SgProject * project) {
#if defined(__cpp_range_based_for) && __cpp_range_based_for >= 200907
  LinkVariableAcrossFiles lvaf;
  lvaf.apply(project);
#endif
}

} }

