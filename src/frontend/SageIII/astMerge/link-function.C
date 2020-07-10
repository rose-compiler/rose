
#include "sage3basic.h"
#include "fixupTraversal.h"

namespace Rose { namespace AST {

#if defined(__cpp_range_based_for) && __cpp_range_based_for >= 200907
#define DEBUG_LinkFunctionAcrossFiles 0

// FIXME use std::tuple instead
struct FunctionDeclTriplet {
  SgFunctionDeclaration * first_nondef_decl;
  SgFunctionDeclaration * defn_decl;
  std::set<SgFunctionDeclaration *> decls;
};

static SgFunctionSymbol * select_shared_function_symbol(std::string const & name, std::map< SgFunctionSymbol *, FunctionDeclTriplet > const & sym_map) {
  if (sym_map.size() == 1) { // No duplication across TU, skip it if it is a static (file scope)
    SgFunctionDeclaration * decl = sym_map.begin()->second.first_nondef_decl;
    ROSE_ASSERT(decl != NULL);
    SgScopeStatement * scope = decl->get_scope();
    ROSE_ASSERT(scope != NULL);
    bool is_static_decl = decl->get_declarationModifier().get_storageModifier().isStatic();
    bool has_file_visibility = ( isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope) ) && is_static_decl;
    return has_file_visibility ? NULL : sym_map.begin()->first;
  } else { // Duplication: select the symbol from defining TU or any
    std::set<SgFunctionSymbol *> defn_syms;
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
      std::set<SgFunctionDeclaration *> decls;

      void visit(SgNode * node) {
        SgFunctionDeclaration * decl = (SgFunctionDeclaration *)node;
        decls.insert(decl);
      }
    } decls;

    struct SymbolCollection : public ROSE_VisitTraversal {
      std::set<SgFunctionSymbol *> symbols;

      void visit(SgNode * node) {
        SgFunctionSymbol * sym = (SgFunctionSymbol *)node;
        symbols.insert(sym);
      }
    } symbols;

    void build_name_symbol_decls_map(
      std::map< std::string, std::map< SgFunctionSymbol *, FunctionDeclTriplet > > & name_map
    ) const {

//    std::cout << "#  LinkFunctionAcrossFiles::build_name_symbol_decls_map" << std::endl;

      std::map<SgFunctionDeclaration *, FunctionDeclTriplet> decl_triplet_map;
      for (auto decl: decls.decls) {
//      std::cout << "#    decl = " << std::hex << decl << " ( " << decl->class_name() << " )" << std::endl;

        SgFunctionDeclaration * first_nondef = isSgFunctionDeclaration(decl->get_firstNondefiningDeclaration());
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

        SgFunctionDeclaration * decl = isSgFunctionDeclaration(sym->get_declaration());
        ROSE_ASSERT(decl != NULL);
        decl = isSgFunctionDeclaration(decl->get_firstNondefiningDeclaration());
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
      std::map< std::string, std::map< SgFunctionSymbol *, FunctionDeclTriplet > > name_map;
      build_name_symbol_decls_map(name_map);

//    std::cout << "#  LinkFunctionAcrossFiles::apply" << std::endl;

      for (auto p: name_map) {
        ROSE_ASSERT(p.second.size() > 0);

//      std::cout << "#    " << p.first << " -> " << p.second.size() << std::endl;

        SgFunctionSymbol * symbol = select_shared_function_symbol(p.first, p.second);
        if (symbol == NULL) continue; // Case of a declaration with file-scope

//      std::cout << "#      symbol = " << std::hex << symbol << " ( " << symbol->class_name() << " )" << std::endl;

        SgFunctionDeclaration * first_nondef_decl = p.second[symbol].first_nondef_decl;
        ROSE_ASSERT(first_nondef_decl != NULL);
        SgFunctionDeclaration * defn_decl = p.second[symbol].defn_decl;

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
          } else {
            symbol->set_declaration(first_nondef_decl);
          }
          for (auto r: q.second.decls) {
            r->set_firstNondefiningDeclaration(first_nondef_decl);
            // Deal with multiple definition: if not selected then it still see itself as the defining one
            SgFunctionDeclaration * self_defn_decl = isSgFunctionDeclaration(r->get_definingDeclaration());
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
#endif

void link_function(SgProject * project) {
#if defined(__cpp_range_based_for) && __cpp_range_based_for >= 200907
  LinkFunctionAcrossFiles lfaf;
  lfaf.apply(project);
#endif
}

} }

