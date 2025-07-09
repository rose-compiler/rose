#include "sage3basic.h"
#include "Rose/AST/Utility.h"

#define DEBUG_LinkAcrossFiles 0

namespace Rose { namespace SourceCode { namespace AST { namespace IO {

///////// Utility code

template <typename NodeT>
struct Collector : public ROSE_VisitTraversal {
  std::set<NodeT *> nodes;
  void visit(SgNode * node) { nodes.insert((NodeT *)node); }
};

template <typename DeclT>
bool has_file_visibility( DeclT * decl) {
  SgScopeStatement * scope = decl->get_scope();
  ROSE_ASSERT(scope != NULL);
  bool is_static_decl = decl->get_declarationModifier().get_storageModifier().isStatic();
  return ( isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope) ) && is_static_decl;
}

template <>
bool has_file_visibility<SgInitializedName>( SgInitializedName * iname) {
  SgScopeStatement * scope = iname->get_scope();
  ROSE_ASSERT(scope != NULL);
  if ( isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope) ) {
    SgVariableDeclaration * vdecl = isSgVariableDeclaration(iname->get_parent());
    if (vdecl) {
      return vdecl->get_declarationModifier().get_storageModifier().isStatic();
    }
  }

  return false;
}

template <typename DeclT>
std::string decl_to_string(DeclT * decl) {
  std::ostringstream oss;
  oss << decl->class_name() << " ( " << std::hex << decl << " ) @ " << decl->get_file_info()->get_filenameString() << " +" << StringUtility::numberToString(decl->get_file_info()->get_line());
  return oss.str();
}


template <typename LinkT>
static void registerSymbolsForSubstitution(
  typename LinkT::sym_t * refsym,
  typename LinkT::desc_t const & refdesc,
  typename LinkT::descmap_t const & descmap,
  std::set<typename LinkT::sym_t *> const & oldsyms,
  std::map<SgNode *, SgNode *> & node_repl_map,
  std::set<SgSymbol *> & sym_del_set,
  std::set<SgType *> & /*type_del_set*/
) {
  for (auto oldsym: oldsyms) {
    ROSE_ASSERT(oldsym != refsym);

    node_repl_map[oldsym] = refsym;
    sym_del_set.insert(oldsym);

    for (auto r: descmap.find(oldsym)->second.decls) {
      r->set_firstNondefiningDeclaration(refdesc.first_nondef_decl);
      // Deal with multiple definition: if not selected then it still see itself as the defining one
      typename LinkT::decl_t * self_defn_decl = (typename LinkT::decl_t *)(r->get_definingDeclaration());
      r->set_definingDeclaration(self_defn_decl ? self_defn_decl : refdesc.defn_decl);
    }
  }

  for (auto r: descmap.find(refsym)->second.decls) {
    r->set_firstNondefiningDeclaration(refdesc.first_nondef_decl);
    // Deal with multiple definition: if not selected then it still see itself as the defining one
    typename LinkT::decl_t * self_defn_decl = (typename LinkT::decl_t *)(r->get_definingDeclaration());
    r->set_definingDeclaration(self_defn_decl ? self_defn_decl : refdesc.defn_decl);
  }
}

void createSymbolAlias(
  SgGlobal * gsaf,
  SgSymbol * refsym,
  SgScopeStatement * scope
) {
  ROSE_ASSERT(scope != NULL);
  if (isSgGlobal(scope)) {
    gsaf->get_symbol_table()->insert(refsym->get_name(), new SgAliasSymbol(refsym));
  } else if (isSgNamespaceDefinitionStatement(scope)) {
    SgNamespaceDefinitionStatement * gnd = ((SgNamespaceDefinitionStatement *)scope)->get_global_definition();
    ROSE_ASSERT(gnd != NULL);
    gnd->get_symbol_table()->insert(refsym->get_name(), new SgAliasSymbol(refsym));
  }
}

///////// LinkAcrossFiles: main template, does linking with handling of ODR Violation

template <typename ObjectLinkerT>
class LinkAcrossFiles {
  public:
    using decl_t = typename ObjectLinkerT::decl_t;
    using sym_t  = typename ObjectLinkerT::sym_t;
    using type_t = typename ObjectLinkerT::type_t;
    using desc_t = typename ObjectLinkerT::desc_t;
    using descmap_t = typename ObjectLinkerT::descmap_t;
    using cluster_t = typename ObjectLinkerT::cluster_t;

  private:
    Collector<decl_t> decls;
    Collector<sym_t> syms;
    Collector<type_t> types;

    static void print_declaration_cluster(
      sym_t * sym,
      desc_t const & desc,
      std::string const prefix,
      std::ostream & out
    ) {
      out << prefix << sym->class_name() << " (" << std::hex << sym << ") : " << sym->get_name() << std::endl;
      desc.print(prefix+"  ", out);
    }

    static void log_defn_vs_nodefn(
      std::string const & /*name*/,
      descmap_t const & descmap,
      std::vector<sym_t *> syms_nodefn,
      std::vector<sym_t *> syms_defns,
      std::ostream & out
    ) {
      if (syms_nodefn.size() > 0 && syms_defns.size() > 1) {
        out << "    At least one declaration cluster without definition AND more than one declaration cluster with definitions. ";
        out << "ODR Violations can only be handled in the absence of non-defining clusters as these lack the definition used to disambiguate the violation. ";
        out << "To solve this issue, group translation-units appropriately, merge the AST of each group, then merge the groups' ASTs. ";
        out << "The non-defining and defining clusters are listed bellow. ";
        out << "The goal is to group each defining cluster with some of the non-defining clusters ; such that each non-defining cluster is grouped with a single defining cluster. ";
        out << "The paths are a good start to group the translation-units." << std::endl << std::endl;
        out << "  !!! AST is INCOMPLETE: symbols not associated with a definition are not linked !!!" << std::endl << std::endl;
      }

      if (syms_nodefn.size() > 0) {
        out << "    Non-defining symbols:" << std::endl;
        for (auto s: syms_nodefn) print_declaration_cluster(s, descmap.find(s)->second, "    > ", out);
      }
      if (syms_defns.size() > 0) {
        out << "    Defining symbols:" << std::endl;
        for (auto s: syms_defns ) print_declaration_cluster(s, descmap.find(s)->second, "    > ", out);
      }
    }

    static void log_cluster(
      std::string const & /*name*/,
      descmap_t const & descmap,
      cluster_t & clusters,
      std::ostream & out
    ) {
      for (auto c: clusters) {
        out << "    Selected symbol (and declaration):" << std::endl;
        print_declaration_cluster(c.first, descmap.find(c.first)->second, "    > ", out);
        if (c.second.size() > 0) {
          out << "    Redundant symbols (declarations will be attached to selected symbol):" << std::endl;
          for (auto s: c.second) {
            print_declaration_cluster(s, descmap.find(s)->second, "    > > ", out);
          }
        }
      }
    }

    static void symbol_clustering(
      std::string const & name,
      descmap_t const & descmap,
      cluster_t & clusters,
      std::ostream & odr_log
    ) {
      std::vector<sym_t *> syms_nodefn;
      std::vector<sym_t *> syms_defns;

#if DEBUG_LinkAcrossFiles
      std::cerr << "  Linking " << name << std::endl;
#endif

      for (auto p: descmap) {
        ObjectLinkerT::clusteringSymbolTriage(p.first, p.second, syms_nodefn, syms_defns);
      }

      bool ODR_violation = true; // (syms_defns.size() > 1);

#if DEBUG_LinkAcrossFiles
      log_defn_vs_nodefn(name, descmap, syms_nodefn, syms_defns, std::cerr);
#endif
      if (ODR_violation) {
        odr_log << "Linking " << ObjectLinkerT::name << " " << name << std::endl;
        log_defn_vs_nodefn(name, descmap, syms_nodefn, syms_defns, odr_log);
      }

      if (syms_defns.size() > 1) {
        while (syms_defns.size() > 0) {
          sym_t * refsym = syms_defns.back();
#if DEBUG_LinkAcrossFiles
          std::cerr << "refsym = " << std::hex << refsym << std::endl;
#endif
          syms_defns.pop_back();

          auto const & refdesc = descmap.find(refsym)->second;

          std::set<sym_t *> & cluster = clusters[refsym];

          auto curr = syms_defns.begin();
          while (curr != syms_defns.end()) {
            sym_t * currsym = *curr;
#if DEBUG_LinkAcrossFiles
            std::cerr << "currsym = " << std::hex << currsym << std::endl;
#endif
            auto const & currdesc = descmap.find(currsym)->second;

            if (refdesc.equivalent(currdesc)) {
#if DEBUG_LinkAcrossFiles
              std::cerr << "equivalent = true" << std::endl;
#endif
              cluster.insert(currsym);
              curr = syms_defns.erase(curr);
            } else {
#if DEBUG_LinkAcrossFiles
              std::cerr << "equivalent = false" << std::endl;
#endif
              curr++;
            }
          }
        }
#if DEBUG_LinkAcrossFiles
        std::cerr << "clusters.size() = " << clusters.size() << std::endl;
#endif

      } else if (syms_defns.size() == 1) {
        sym_t * refsym = syms_defns.back();
        syms_defns.pop_back();

        std::set<sym_t *> & cluster = clusters[refsym];
        for (auto s: syms_nodefn) {
          cluster.insert(s);
        }
      } else if (syms_nodefn.size() > 0) {

        sym_t * refsym = syms_nodefn.back();
        syms_nodefn.pop_back();

        std::set<sym_t *> & cluster = clusters[refsym];

        for (auto currsym: syms_nodefn) {
          cluster.insert(currsym);
        }
      }

      ROSE_ASSERT(syms_defns.size() == 0);

#if DEBUG_LinkAcrossFiles
      log_cluster(name, descmap, clusters, std::cerr);
#endif
      if (ODR_violation) {
        odr_log << "  Resolution " << clusters.size() << " cluster of symbols:" << std::endl;
        log_cluster(name, descmap, clusters, odr_log);
        odr_log << "===================" << std::endl;
      }
    }

  public:
    void apply(SgProject * project) {
#if DEBUG_LinkAcrossFiles
      std::cout << "LinkAcrossFiles<" << ObjectLinkerT::name << ">::apply() " << std::endl;
#endif

      ObjectLinkerT::collection(decls, syms, types);

      std::map< std::string, descmap_t > name_map;
      ObjectLinkerT::buildNameMap(decls.nodes, syms.nodes, name_map);

      std::map<SgNode *, SgNode *> node_repl_map;
      std::set<SgSymbol *> sym_del_set;
      std::set<SgType *>   type_del_set;

      std::map<std::string, std::string> odrs;
      for (auto p: name_map) {
        ROSE_ASSERT(p.second.size() > 0);

        cluster_t clusters;

        std::ostringstream odr_log;
        symbol_clustering(p.first, p.second, clusters, odr_log);
        auto odr = odr_log.str();
        if (odr.size() > 0) {
          odrs[p.first] = odr;
        }
        if (clusters.empty()) continue; // Case of declarations with file-scope

        ObjectLinkerT::clusterProcessing(project->get_globalScopeAcrossFiles(), p.first, p.second, clusters, node_repl_map, sym_del_set, type_del_set);
      }

      if (odrs.size() > 0) {
        if (SgProject::get_verbose() > 0) {
          std::cout << odrs.size() << " potential ODR Violation on " << ObjectLinkerT::name << " detected. See rose_odr_" << ObjectLinkerT::name << ".log for more information." << std::endl;
        }
        std::ostringstream fn; fn << "rose_odr_" << ObjectLinkerT::name << ".log";
        std::ofstream f(fn.str());
        for (auto odr : odrs) {
          f << odr.second;
        }
      }

      ObjectLinkerT::typeProcessing(types.nodes, node_repl_map, sym_del_set, type_del_set);

      // Delete superfluous symbols
      for (auto s: sym_del_set) {
        SgSymbolTable * table = isSgSymbolTable(s->get_parent());
        ROSE_ASSERT(table != NULL);
        table->remove(s);
        delete s;
      }

      // Delete superfluous types
      for (auto t: type_del_set) {
        delete t;
      }

      Rose::AST::Utility::edgePointerReplacement(node_repl_map); // FIXME before or after delete?
    }
};

// Linkers for Function, Variable, Class, and Namespace

struct FunctionLinker {
  using decl_t = SgFunctionDeclaration;
  using sym_t  = SgFunctionSymbol;
  using type_t = void;

  static constexpr const char * name = "function";
  
  struct desc_t {
    decl_t * first_nondef_decl;
    decl_t * defn_decl;
    std::set<decl_t *> decls;

    void print(std::string const prefix, std::ostream & out) const {
      out << prefix << "First non-defining: " << decl_to_string(first_nondef_decl) << std::endl;
      if (defn_decl)
        out << prefix << "Defining:           " << decl_to_string(defn_decl) << std::endl;
      for (auto d: decls)
        if (d != first_nondef_decl && d != defn_decl)
          out << prefix << "Extra non-defining: " << decl_to_string(d) << std::endl;
    }

    bool equivalent(desc_t const &) const {
      return true;
    }
  };

  using descmap_t = std::map< sym_t *, desc_t >;
  using cluster_t = std::map< sym_t *, std::set<sym_t *> >;

  static void collection(
    Collector<decl_t> & decls,
    Collector<sym_t> & syms,
    Collector<type_t> & /*types*/
  ) {
    SgFunctionDeclaration::traverseMemoryPoolNodes(decls);
    SgTemplateInstantiationFunctionDecl::traverseMemoryPoolNodes(decls);
    SgFunctionSymbol::traverseMemoryPoolNodes(syms);

    SgMemberFunctionDeclaration::traverseMemoryPoolNodes(decls);
    SgTemplateInstantiationMemberFunctionDecl::traverseMemoryPoolNodes(decls);
    SgMemberFunctionSymbol::traverseMemoryPoolNodes(syms);
  }

  static void buildNameMap(
    std::set<decl_t *> const & decls,
    std::set<sym_t  *> const & syms,
    std::map< std::string, descmap_t > & name_map
  ) {
    std::map<decl_t *, desc_t> decl_triplet_map;

    for (auto decl: decls) {
      decl_t * first_nondef = isSgFunctionDeclaration(decl->get_firstNondefiningDeclaration());
      ROSE_ASSERT(first_nondef != NULL);
      desc_t & decl_triplet = decl_triplet_map[first_nondef];
      decl_triplet.decls.insert(decl);
      if (decl == first_nondef) {
        decl_triplet.first_nondef_decl = decl;
      } else if (decl == decl->get_definingDeclaration()) {
        decl_triplet.defn_decl = decl;
      }
    }

    for (auto sym: syms) {
      decl_t * decl = isSgFunctionDeclaration(sym->get_declaration());
      ROSE_ASSERT(decl != NULL);
      decl = isSgFunctionDeclaration(decl->get_firstNondefiningDeclaration());
      ROSE_ASSERT(decl != NULL);
      if (decl_triplet_map.find(decl) == decl_triplet_map.end()) {
        std::cerr << "decl = " << std::hex << decl << " ( " << decl->class_name() << " )" << std::endl;
        std::cerr << "  decl->get_firstNondefiningDeclaration = " << std::hex << decl->get_firstNondefiningDeclaration() << std::endl;
        std::cerr << "  decl->get_definingDeclaration         = " << std::hex << decl->get_definingDeclaration()         << std::endl;
      }
      ROSE_ASSERT(decl_triplet_map.find(decl) != decl_triplet_map.end());

      SgFunctionType * ftype = isSgFunctionType(decl->get_type());
      ROSE_ASSERT(ftype != nullptr);

      std::string name = decl->get_qualified_name().getString();
      for (auto t: ftype->get_arguments()) {
        name += "__" + t->get_mangled().getString();
      }

      name_map[name][sym] = decl_triplet_map[decl];
    }
  }

  static void clusteringSymbolTriage(
    sym_t * symbol,
    desc_t const & desc,
    std::vector<sym_t *> & syms_nodefn,
    std::vector<sym_t *> & syms_defns
  ) {
    if (!has_file_visibility(desc.first_nondef_decl)) {
      if (desc.defn_decl) {
        syms_defns.push_back(symbol);
      } else {
        syms_nodefn.push_back(symbol);
      }
    }
  }

  static void clusterProcessing(
    SgGlobal * gsaf,
    std::string const & /*name*/,
    descmap_t const & descmap,
    cluster_t const & clusters,
    std::map<SgNode *, SgNode *> & node_repl_map,
    std::set<SgSymbol *> & sym_del_set,
    std::set<SgType *> & type_del_set
  ) {
    for (auto c: clusters) {
      sym_t * refsym = c.first;
      auto const & refdesc = descmap.find(refsym)->second;

      ROSE_ASSERT(refdesc.first_nondef_decl != NULL);
      refsym->set_declaration(refdesc.first_nondef_decl);

      createSymbolAlias(gsaf, refsym, refdesc.first_nondef_decl->get_scope());

      registerSymbolsForSubstitution<FunctionLinker>(refsym, refdesc, descmap, c.second, node_repl_map, sym_del_set, type_del_set);
    }
  }

  static void typeProcessing(std::set<type_t*> const &, std::map<SgNode*,SgNode*> &,
                             std::set<SgSymbol*> &, std::set<SgType *> &) {
  }
};

struct VariableLinker {
  using decl_t = SgInitializedName;
  using sym_t  = SgVariableSymbol;
  using type_t = void;

  static constexpr const char * name = "variable";
  
  struct desc_t { // first and last of the chain, chain is reversed: following `get_prev_decl_item` point lead to first from last
    decl_t * first;
    decl_t * last;
    
    void print(std::string const prefix, std::ostream & out) const {
      out << prefix << "First: " << decl_to_string(first) << std::endl;
      out << prefix << "Last : " << decl_to_string(last) << std::endl;
      decl_t * d = last->get_prev_decl_item();
      while (d != nullptr && d != first) {
        out << prefix << "Other: " << decl_to_string(d) << std::endl;
      }
    }

    bool equivalent(desc_t const &) const {
      return true;
    }
  };

  using descmap_t = std::map< sym_t *, desc_t >;
  using cluster_t = std::map< sym_t *, std::set<sym_t *> >;

  static void collection(
    Collector<decl_t> & decls,
    Collector<sym_t> & syms,
    Collector<type_t> & /*types*/
  ) {
    SgInitializedName::traverseMemoryPoolNodes(decls);
    SgVariableSymbol::traverseMemoryPoolNodes(syms);
  }

  static void buildNameMap(
    std::set<decl_t *> const & decls,
    std::set<sym_t  *> const & syms,
    std::map< std::string, descmap_t > & name_map
  ) {
    std::set<SgInitializedName *> seens;
    std::map<SgInitializedName *, std::vector<SgInitializedName *> > first_to_rlist;

    // Following map is used when symbol does not point to the first declaration on the chain
    // This happen when a variable is declared in two headers (extern) and each header include the other header.
    // Then, when the AST from two files that include one of the header are merged, the sharing will make the symbol point to a iname that is not the first in the chain.
    // FIXME if performance issue it might be faster to preserve the other iname during sharing
    std::map<SgInitializedName *, SgInitializedName *> any_to_first;
    for (auto iname: decls) {
      // skip if already seen in any list
      if (!seens.insert(iname).second) continue;

      // build longuest list so far (else it would have been seen) (could build partial list but premature optimization)
      std::vector<SgInitializedName *> rlist;
      rlist.push_back(iname);
      while (true) {
        SgInitializedName * prev = rlist.back()->get_prev_decl_item();
        ROSE_ASSERT(prev != rlist.back());
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

    for (auto sym: syms) {
      SgInitializedName * iname = isSgInitializedName(sym->get_symbol_basis());
      ROSE_ASSERT(iname != NULL);

      iname = any_to_first[iname];
      ROSE_ASSERT(iname != NULL);

      std::vector<SgInitializedName *> & rlist = first_to_rlist[iname];
      ROSE_ASSERT(rlist.size() > 0);
      
      name_map[sym->get_mangled_name().getString()][sym].first = rlist.back();
      name_map[sym->get_mangled_name().getString()][sym].last  = rlist[0];
    }
  }

  static void clusteringSymbolTriage(
    sym_t * symbol,
    desc_t const & desc,
    std::vector<sym_t *> & /*syms_nodefn*/,
    std::vector<sym_t *> & syms_defns
  ) {
    if (!has_file_visibility(desc.first)) {
      syms_defns.push_back(symbol);
    }
  }

  static void clusterProcessing(
    SgGlobal * gsaf,
    std::string const & /*name*/,
    descmap_t const & descmap,
    cluster_t const & clusters,
    std::map<SgNode *, SgNode *> & node_repl_map,
    std::set<SgSymbol *> & sym_del_set,
    std::set<SgType *> & /*type_del_set*/
  ) {
    for (auto c: clusters) {
      sym_t * refsym = c.first;
      auto const & refdesc = descmap.find(refsym)->second;

      createSymbolAlias(gsaf, refsym, refdesc.first->get_scope());

      // Register symbols for deletion and substitution. Patches nondef/defn declarations.
      decl_t * last = refdesc.last;
      for (auto oldsym: c.second) {
        ROSE_ASSERT(oldsym != refsym);

        auto const & olddesc = descmap.find(oldsym)->second;

        node_repl_map[oldsym] = refsym;
        sym_del_set.insert(oldsym);

        // We only concatenate the chain of inames associated with this symbol if the
        // symbol's basis is actually the first item of that chain. Other cases are due
        // to multiple extern declaration being seen from headers (which are read in
        // various orders) and eliminated by sharing. The result should be a single chain with
        // symbols pointing to various point of that chain.
        // Potential for 'last' to point to some iname which creates cycles: setting prev_decl_item to nulltptr
        SgInitializedName * base_iname = isSgInitializedName(oldsym->get_symbol_basis());
        ROSE_ASSERT(base_iname != NULL);
        if (base_iname == olddesc.first && base_iname->get_prev_decl_item() == NULL) {
          olddesc.first->set_prev_decl_item(last);
          last = olddesc.last;
          last->set_prev_decl_item(nullptr);
        }
      }
    }
  }

  static void typeProcessing(std::set<type_t*> const &, std::map<SgNode*,SgNode*> &,
                             std::set<SgSymbol*> &, std::set<SgType*> &) {
  }
};

struct ClassLinker {
  using decl_t = SgClassDeclaration;
  using sym_t  = SgClassSymbol;
  using type_t = SgClassType;

  static constexpr const char * name = "class";
  
  struct desc_t {
    decl_t * first_nondef_decl;
    decl_t * defn_decl;
    std::set<decl_t *> decls;

    void print(std::string const prefix, std::ostream & out) const {
      out << prefix << "First non-defining: " << decl_to_string(first_nondef_decl) << std::endl;
      if (defn_decl)
        out << prefix << "Defining:           " << decl_to_string(defn_decl) << std::endl;
      for (auto d: decls)
        if (d != first_nondef_decl && d != defn_decl)
          out << prefix << "Extra non-defining: " << decl_to_string(d) << std::endl;
    }

    bool equivalent(desc_t const & desc) const {
      ROSE_ASSERT(defn_decl);
      ROSE_ASSERT(desc.defn_decl);

      SgClassDefinition * defn0 = defn_decl->get_definition();
      ROSE_ASSERT(defn0);
      SgClassDefinition * defn1 = desc.defn_decl->get_definition();
      ROSE_ASSERT(defn1);

      // We handle ODR Violations *only* for C-style POD. In all other cases, classes are marked equivalent.
      // Heavy-weight ODR Violation analysis needed to ensure those are actually the same.
      for (auto m: defn0->get_members()) if (m->variantT() != V_SgVariableDeclaration) return true;
      for (auto m: defn1->get_members()) if (m->variantT() != V_SgVariableDeclaration) return true;

      if (defn0->get_members().size() != defn1->get_members().size()) return false;
      
      for (size_t i = 0; i < defn0->get_members().size(); i++) {
        auto m0 = defn0->get_members().at(i);
        auto m1 = defn1->get_members().at(i);

        SgVariableDeclaration * f0 = (SgVariableDeclaration*)(m0);
        SgVariableDeclaration * f1 = (SgVariableDeclaration*)(m1);

        SgInitializedName * i0 = f0->get_variables()[0];
        SgInitializedName * i1 = f1->get_variables()[0];

        if (i0->get_type() != i1->get_type()) return false;
        if (i0->get_name() != i1->get_name()) return false;
      }

      return true;
    }
  };
  using descmap_t = std::map< sym_t *, desc_t >;
  using cluster_t = std::map< sym_t *, std::set<sym_t *> >;

  static void collection(
    Collector<decl_t> & decls,
    Collector<sym_t> & syms,
    Collector<type_t> & types
  ) {
    SgClassDeclaration::traverseMemoryPoolNodes(decls);
    SgTemplateInstantiationDecl::traverseMemoryPoolNodes(decls);
    SgClassSymbol::traverseMemoryPoolNodes(syms);
    SgClassType::traverseMemoryPoolNodes(types);
  }

  static void buildNameMap(
    std::set<decl_t *> const & decls,
    std::set<sym_t  *> const & syms,
    std::map< std::string, descmap_t > & name_map
  ) {
    std::map<decl_t *, desc_t> decl_triplet_map;

    for (auto decl: decls) {
      decl_t * first_nondef = isSgClassDeclaration(decl->get_firstNondefiningDeclaration());
      ROSE_ASSERT(first_nondef != NULL);
      desc_t & decl_triplet = decl_triplet_map[first_nondef];
      decl_triplet.decls.insert(decl);
      if (decl == first_nondef) {
        decl_triplet.first_nondef_decl = decl;
      } else if (decl == decl->get_definingDeclaration()) {
        decl_triplet.defn_decl = decl;
      }
    }

    for (auto sym: syms) {
      decl_t * decl = isSgClassDeclaration(sym->get_declaration());
      ROSE_ASSERT(decl != NULL);
      decl = isSgClassDeclaration(decl->get_firstNondefiningDeclaration());
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

  static void clusteringSymbolTriage(
    sym_t * symbol,
    desc_t const & desc,
    std::vector<sym_t *> & syms_nodefn,
    std::vector<sym_t *> & syms_defns
  ) {
    if (!has_file_visibility(desc.first_nondef_decl)) {
      if (desc.defn_decl) {
        syms_defns.push_back(symbol);
      } else {
        syms_nodefn.push_back(symbol);
      }
    }
  }

  static void clusterProcessing(
    SgGlobal * gsaf,
    std::string const & /*name*/,
    descmap_t const & descmap,
    cluster_t const & clusters,
    std::map<SgNode *, SgNode *> & node_repl_map,
    std::set<SgSymbol *> & sym_del_set,
    std::set<SgType *> & type_del_set
  ) {
    for (auto c: clusters) {
      sym_t * refsym = c.first;
      auto const & refdesc = descmap.find(refsym)->second;

      ROSE_ASSERT(refdesc.first_nondef_decl != NULL);
      refsym->set_declaration(refdesc.first_nondef_decl);

      createSymbolAlias(gsaf, refsym, refdesc.first_nondef_decl->get_scope());

      registerSymbolsForSubstitution<ClassLinker>(refsym, refdesc, descmap, c.second, node_repl_map, sym_del_set, type_del_set);
    }
  }

  static void typeProcessing(
    std::set<type_t *> const & types,
    std::map<SgNode *, SgNode *> & node_repl_map,
    std::set<SgSymbol *> & /*sym_del_set*/,
    std::set<SgType *> & type_del_set
  ) {
    // Build map from first decl to map of type to assoc. decl
    std::map<SgClassDeclaration *, std::map<SgClassType *, SgClassDeclaration *> > xtype_map;
    for (auto t: types) {
      SgClassDeclaration * xdecl = isSgClassDeclaration(t->get_declaration());
      ROSE_ASSERT(xdecl != NULL);
      SgClassDeclaration * first_xdecl = isSgClassDeclaration(xdecl->get_firstNondefiningDeclaration());
      ROSE_ASSERT(first_xdecl != NULL);
      xtype_map[first_xdecl][t] = xdecl;
    }

    // Find all class-types pointing to wrong declaration (must be first non-defining)
    for (auto p: xtype_map) {
      SgClassType * xtype = NULL;
      std::vector<SgClassType *> xtypes;
      for (auto q: p.second) {
        if (q.second == p.first) {
          xtype = q.first; // valid type if assoc. decl. is the first non-defining declaration
        } else {
          xtypes.push_back(q.first);
        }
      }
      ROSE_ASSERT(xtype != NULL); // Should have found at least one valid type
      for (auto xtype_: xtypes) {
        type_del_set.insert(xtype_);
        node_repl_map[xtype_] = xtype;
      }
    }
  }
};
#if 0
struct NamespaceLinker {
  using decl_t = SgNamespaceDeclaration;
  using sym_t  = SgNamespaceSymbol;
  using type_t = void;

  static constexpr char * name = "namespace";
  
  struct desc_t {
    decl_t * first_nondef_decl;
    decl_t * defn_decl;
    std::set<decl_t *> decls;
    
    void print(std::string const prefix, std::ostream & out) const {
      // TODO    
    }

    bool equivalent(desc_t const & desc) const {
      return true;
    }
  };
  using descmap_t = std::map< sym_t *, desc_t >;
  using cluster_t = std::map< sym_t *, std::set<sym_t *> >;

  static void collection(
    Collector<decl_t> & decls,
    Collector<sym_t> & syms,
    Collector<type_t> & types
  ) {
    SgNamespaceDeclaration::traverseMemoryPoolNodes(decls);
    SgNamespaceSymbol::traverseMemoryPoolNodes(syms);
  }

  static void buildNameMap(
    std::set<decl_t *> const & decls,
    std::set<sym_t  *> const & syms,
    std::map< std::string, descmap_t > & name_map
  ) {
    // TODO
  }

  static void clusteringSymbolTriage(
    sym_t * symbol,
    desc_t const & desc,
    std::vector<sym_t *> & syms_nodefn,
    std::vector<sym_t *> & syms_defns
  ) {
    syms_defns.push_back(symbol);
  }

  static void clusterProcessing(
    SgGlobal * gsaf,
    std::string const & name,
    descmap_t const & descmap,
    cluster_t const & clusters,
    std::map<SgNode *, SgNode *> & node_repl_map,
    std::set<SgSymbol *> & sym_del_set,
    std::set<SgType *> & type_del_set
  ) {
    // TODO
  }

  static void typeProcessing(
    std::set<type_t *> const & types,
    std::map<SgNode *, SgNode *> & node_repl_map,
    std::set<SgSymbol *> & sym_del_set,
    std::set<SgType *> & type_del_set
  ) {}
};
#endif
void link(SgProject * project) {
//LinkAcrossFiles<NamespaceLinker> lnaf;
//lnaf.apply(project);
  LinkAcrossFiles<ClassLinker> lcaf;
  lcaf.apply(project);
  LinkAcrossFiles<FunctionLinker> lfaf;
  lfaf.apply(project);
  LinkAcrossFiles<VariableLinker> lvaf;
  lvaf.apply(project);
}

} } } }

