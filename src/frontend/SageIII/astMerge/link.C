
#include "sage3basic.h"
#include "fixupTraversal.h"

namespace Rose {
namespace AST {
#if defined(__cpp_range_based_for) && __cpp_range_based_for >= 200907

/////////////////////////
//  Function's Linking //
/////////////////////////

#define DEBUG_LinkFunctionAcrossFiles 0

// FIXME use std::tuple instead
struct FunctionDeclTriplet {
  SgDeclarationStatement * first_nondef_decl;
  SgDeclarationStatement * defn_decl;
  std::set<SgDeclarationStatement *> decls;
};

static SgSymbol * select_shared_function_symbol(std::string const & name, std::map< SgSymbol *, FunctionDeclTriplet > const & sym_map) {
  if (sym_map.size() == 1) { // No duplication across TU, skip it if it is a static (file scope)
    SgDeclarationStatement * decl = sym_map.begin()->second.first_nondef_decl;
    ROSE_ASSERT(decl != NULL);
    SgScopeStatement * scope = decl->get_scope();
    ROSE_ASSERT(scope != NULL);
    bool is_static_decl = decl->get_declarationModifier().get_storageModifier().isStatic();
    bool has_file_visibility = ( isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope) ) && is_static_decl;
    return has_file_visibility ? NULL : sym_map.begin()->first;
  } else { // Duplication: select the symbol from defining TU or any
    std::set<SgSymbol *> defn_syms;
    std::set<SgSourceFile *> srcfiles;
    for (auto q: sym_map) {
      srcfiles.insert(SageInterface::getEnclosingSourceFile(q.first));
      if (q.second.defn_decl != NULL) {
        defn_syms.insert(q.first);
      }
    }

    if (srcfiles.size() < sym_map.size()) {
      std::cerr << "Found disconnected symbols that are not from distinct files..." << std::endl;
      std::cerr << " - name : " << name << std::endl;
      for (auto q: sym_map) {
        SgSourceFile * srcfile = SageInterface::getEnclosingSourceFile(q.first);
        std::cerr << " > symbol = " << std::hex << q.first << " (" << q.first->class_name() << ")" << std::endl;
        std::cerr << " > srcfile : " << srcfile->get_sourceFileNameWithPath() << " (" << std::hex << srcfile << ")" << std::endl;
      }
    }

    if (defn_syms.size() > 1) {
      std::cerr << "Found more than one definition for: " << name << std::endl;
      for (auto s: defn_syms) {
        SgSourceFile * srcfile = SageInterface::getEnclosingSourceFile(s);
        std::cerr << " > symbol = " << std::hex << s << " (" << s->class_name() << ")" << std::endl;
        std::cerr << " > srcfile : " << srcfile->get_sourceFileNameWithPath() << " (" << std::hex << srcfile << ")" << std::endl;
      }
    }

    return (defn_syms.size() > 0) ? *(defn_syms.begin()) : sym_map.begin()->first;
  }
}

class LinkFunctionAcrossFiles {
  private:
    struct DeclarationCollection : public ROSE_VisitTraversal {
      std::set<SgDeclarationStatement *> decls;

      void visit(SgNode * node) {
        SgDeclarationStatement * decl = (SgDeclarationStatement *)node;
        decls.insert(decl);
      }
    } decls;

    struct SymbolCollection : public ROSE_VisitTraversal {
      std::set<SgSymbol *> symbols;

      void visit(SgNode * node) {
        SgSymbol * sym = (SgSymbol *)node;
        symbols.insert(sym);
      }
    } symbols;

    void build_name_symbol_decls_map(
      std::map< std::string, std::map< SgSymbol *, FunctionDeclTriplet > > & name_map
    ) const {

//    std::cout << "#  LinkFunctionAcrossFiles::build_name_symbol_decls_map" << std::endl;

      std::map<SgDeclarationStatement *, FunctionDeclTriplet> decl_triplet_map;
      for (auto decl: decls.decls) {
//      std::cout << "#    decl = " << std::hex << decl << " ( " << decl->class_name() << " )" << std::endl;

        SgDeclarationStatement * first_nondef = decl->get_firstNondefiningDeclaration();
        ROSE_ASSERT(first_nondef != NULL);
        FunctionDeclTriplet & decl_triplet = decl_triplet_map[first_nondef];
        decl_triplet.decls.insert(decl);
        if (decl == first_nondef) {
          decl_triplet.first_nondef_decl = decl;
        } else if (decl == decl->get_definingDeclaration()) {
          decl_triplet.defn_decl = decl;
        }
      }

      for (auto sym: symbols.symbols) {
//      std::cout << "#    sym = " << std::hex << sym << " ( " << sym->class_name() << " )" << std::endl;

        SgDeclarationStatement * decl = isSgDeclarationStatement(sym->get_symbol_basis());
        ROSE_ASSERT(decl != NULL);
        if (decl_triplet_map.find(decl) == decl_triplet_map.end()) {
          std::cerr << "decl = " << std::hex << decl << " ( " << decl->class_name() << " )" << std::endl;
          std::cerr << "  decl->get_firstNondefiningDeclaration = " << std::hex << decl->get_firstNondefiningDeclaration() << std::endl;
          std::cerr << "  decl->get_definingDeclaration         = " << std::hex << decl->get_definingDeclaration()         << std::endl;
        }
        ROSE_ASSERT(decl_triplet_map.find(decl) != decl_triplet_map.end());
        name_map[sym->get_mangled_name().getString()][sym] = decl_triplet_map[decl];
      }
    }

  public:
    void apply(SgProject * project) {
      SgFunctionDeclaration::traverseMemoryPoolNodes(decls);
      SgTemplateInstantiationFunctionDecl::traverseMemoryPoolNodes(decls);
      SgFunctionSymbol::traverseMemoryPoolNodes(symbols);

      SgMemberFunctionDeclaration::traverseMemoryPoolNodes(decls);
      SgTemplateInstantiationMemberFunctionDecl::traverseMemoryPoolNodes(decls);
      SgMemberFunctionSymbol::traverseMemoryPoolNodes(symbols);

      std::map<SgNode *, SgNode *> sym_repl_map;
      std::set<SgSymbol *> sym_del_set;

      // Build map of mangled-name to map of symbol (per TU) to declarations (nondef/defn/set)
      std::map< std::string, std::map< SgSymbol *, FunctionDeclTriplet > > name_map;
      build_name_symbol_decls_map(name_map);

//    std::cout << "#  LinkFunctionAcrossFiles::apply" << std::endl;

      for (auto p: name_map) {
        ROSE_ASSERT(p.second.size() > 0);

//      std::cout << "#    " << p.first << " -> " << p.second.size() << std::endl;

        SgSymbol * symbol = select_shared_function_symbol(p.first, p.second);
        if (symbol == NULL) continue; // Case of a declaration with file-scope

//      std::cout << "#      symbol = " << std::hex << symbol << " ( " << symbol->class_name() << " )" << std::endl;

        SgDeclarationStatement * first_nondef_decl = p.second[symbol].first_nondef_decl;
        ROSE_ASSERT(first_nondef_decl != NULL);
        SgDeclarationStatement * defn_decl = p.second[symbol].defn_decl;

        SgScopeStatement * scope = first_nondef_decl->get_scope();
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
          }
          for (auto r: q.second.decls) {
            r->set_firstNondefiningDeclaration(first_nondef_decl);
            // Deal with multiple definition: if not selected then it still see itself as the defining one
            SgDeclarationStatement * self_defn_decl = r->get_definingDeclaration();
            r->set_definingDeclaration(self_defn_decl ? self_defn_decl : defn_decl);
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

/////////////////////////
//  Variable's Linking //
/////////////////////////

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
  } else { // Duplication: select any symbol. TODO: better choice
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
        first_to_rlist[rlist.back()] = rlist;
      }

      for (auto sym: symbols.symbols) {
        SgInitializedName * iname = isSgInitializedName(sym->get_symbol_basis());
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
            q.second.first->set_prev_decl_item(last_iname);
            last_iname = q.second.second;
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

/////////////////////////
//  Main Link Function //
/////////////////////////

void link(SgProject * project) {
#if defined(__cpp_range_based_for) && __cpp_range_based_for >= 200907

  // TODO namespace

  // TODO struct

  LinkFunctionAcrossFiles lfaf;
  lfaf.apply(project);
  
  LinkVariableAcrossFiles lvaf;
  lvaf.apply(project);
#endif
}

}
}

