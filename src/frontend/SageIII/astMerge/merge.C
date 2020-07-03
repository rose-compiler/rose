
#include "sage3basic.h"

#include "link.h"
#include "delete.h"
#include "share.h"

#define ENABLE_plot_links 0

using namespace std;

namespace Rose {
namespace AST {

#if ENABLE_plot_links

static std::string declaration_position(SgLocatedNode * const lnode) {
  Sg_File_Info* fileInfo = lnode->get_file_info();
  ROSE_ASSERT(fileInfo != NULL);

  return "_F" + StringUtility::numberToString(fileInfo->get_file_id()) + \
         "_L" + StringUtility::numberToString(fileInfo->get_line())    + \
         "_C" + StringUtility::numberToString(fileInfo->get_col())     ;
}

template <typename NodeT>
static void node_to_graphviz(std::ostream & os, std::set<SgNode *> & seens, NodeT * node) {
  seens.insert(node);
}

template <>
void node_to_graphviz<SgDeclarationStatement>(std::ostream & os, std::set<SgNode *> & seens, SgDeclarationStatement * node) {
  if (!seens.insert(node).second) return;

  SgDeclarationStatement * defn = node->get_definingDeclaration();
  if (defn != NULL) {
    node_to_graphviz(os, seens, defn);
    os << "  n_" << std::hex << node << " -> n_" << std::hex << defn  << " [constraint=false, label=\"defn\", color=black];" << std::endl;
  }

  SgDeclarationStatement * nondef = node->get_firstNondefiningDeclaration();
  if (nondef != NULL) {
    node_to_graphviz(os, seens, nondef);
    os << "  n_" << std::hex << node << " -> n_" << std::hex << nondef << " [constraint=false, label=\"nondef\", color=black];" << std::endl;
  }

  SgClassDeclaration * xdecl = isSgClassDeclaration(node);
  if (xdecl != NULL && xdecl->get_definition() != NULL) {
    node_to_graphviz(os, seens, xdecl->get_definition());
    os << "  n_" << std::hex << node << " -> n_" << std::hex << xdecl->get_definition() << " [constraint=false, label=\"defintion\", color=black];" << std::endl;
  }
}

template <>
void node_to_graphviz<SgInitializedName>(std::ostream & os, std::set<SgNode *> & seens, SgInitializedName * node) {
  if (!seens.insert(node).second) return;

  SgInitializedName * prev_iname = node->get_prev_decl_item();
  if (prev_iname != NULL) {
    node_to_graphviz(os, seens, prev_iname);
    os << "  n_" << std::hex << node << " -> n_" << std::hex << prev_iname  << " [constraint=false, label=\"previous\", color=black];" << std::endl;
  }
}

template <typename NodeT>
static bool is_frontend_variable(NodeT * n) {
  return n->get_mangled_name().getString().find("frontend_specific_variable_to_provide_header_file_path") != std::string::npos;
}

template <typename NodeT>
static void nodes_and_syms_to_graphviz_tree(
  std::ostream & os,
  std::set<NodeT *> const & nodes,
  std::set<SgSymbol *> const & symbols
) {
  std::set<SgNode *> seens;

  os << "digraph G {" << std::endl;

  for (auto node: nodes) {
    SgDeclarationStatement * decl = isSgDeclarationStatement(node);
    SgInitializedName * iname = isSgInitializedName(node);
    if (decl != NULL) {
      if (!is_frontend_variable(decl)) node_to_graphviz(os, seens, decl);
    } else if (iname != NULL) {
      if (!is_frontend_variable(iname)) node_to_graphviz(os, seens, iname);
    } else {
      seens.insert(node);
    }
  }

  for (auto sym: symbols) {
    if (is_frontend_variable(sym))
      continue;

    seens.insert(sym);

    SgScopeStatement * scope = sym->get_scope();
    ROSE_ASSERT(scope != NULL);
    os << "  n_" << std::hex << sym << " -> n_" << std::hex << scope << " [constraint=false, label=\"scope\", color=green];" << std::endl;

    SgNode * basis = sym->get_symbol_basis();
    ROSE_ASSERT(basis != NULL);
    os << "  n_" << std::hex << sym << " -> n_" << std::hex << basis << " [constraint=true, label=\"basis\", color=blue];" << std::endl;      
  }

  std::vector<SgNode *> parents;
  for (auto n: seens) {
    SgLocatedNode * lnode = isSgLocatedNode(n);
    if (lnode != NULL) {
      parents.push_back(lnode);
    }
  }

  std::set<SgNode *> parent_seens;
  while (!parents.empty()) {
    SgNode * p = parents.back();
    parents.pop_back();

    if (parent_seens.find(p) != parent_seens.end()) continue;
    parent_seens.insert(p);

    SgDeclarationStatement * declstmt_parent = isSgDeclarationStatement(p);
    if (declstmt_parent != NULL && seens.find(declstmt_parent) != seens.end()) {
      node_to_graphviz(os, seens, declstmt_parent);
    }

    SgNode * pp = p->get_parent();
    if (pp != NULL && !isSgSourceFile(pp)) {
      parents.push_back(pp);
      os << "  n_" << std::hex << pp << " -> n_" << std::hex << p << " [constraint=true, color=red];" << std::endl;
    }
  }
  seens.insert(parent_seens.begin(), parent_seens.end());

  std::map<SgGlobal *, std::set<SgNode *> > enclosure_map;
  for (auto n: seens) {
    SgGlobal * g = isSgGlobal(n) ? (SgGlobal*)n : SageInterface::getEnclosingNode<SgGlobal>(n);
    enclosure_map[g].insert(n);
  }

  for (auto p: enclosure_map) {
#if 0
    if (p.first) {
      os << "  subgraph cluster_" << std::hex << p.first << " {" << std::endl;
      SgSourceFile * srcfile = isSgSourceFile(p.first->get_parent());
      std::string filename = srcfile ? srcfile->getFileName() : "shared";
      os << "    label = \"" << filename << "\";" << std::endl;
    }
#endif
    for (auto n: p.second) {
      SgDeclarationStatement * decl = isSgDeclarationStatement(n);
      SgInitializedName * iname = isSgInitializedName(n);
      SgSymbol * sym = isSgSymbol(n);
      os << "    n_" << std::hex << n << " [label=\"" << std::hex << n << "\\n" << n->class_name();
      if (decl) {
        os << "\\n" << decl->get_mangled_name().getString();
        os << "\\n" << declaration_position(decl);
      } else if (iname) {
        os << "\\n" << iname->get_mangled_name().getString();
      } else if (sym) {
        os << "\\n" << sym->get_mangled_name().getString();
      }
      os << "\"];" << std::endl;
    }
#if 0
    if (p.first) {
      os << "  }" << std::endl;
    }
#endif
  }
  os << "}" << std::endl;
}

static void plot_links(std::ostream & os) {
  struct : public ROSE_VisitTraversal {
    std::set<SgNode *> decls;

    void visit(SgNode * node) {
      SgDeclarationStatement * declstmt = isSgDeclarationStatement(node);
      SgInitializedName * iname = isSgInitializedName(node);
      if ( declstmt && !is_frontend_variable(declstmt) ) decls.insert(declstmt);
      if ( iname    && !is_frontend_variable(iname)    ) decls.insert(iname);
    }
  } decls;

  struct : public ROSE_VisitTraversal {
    std::set<SgSymbol *> symbols;

    void visit(SgNode * node) {
      SgSymbol * sym = (SgSymbol *)node;
      if (!is_frontend_variable(sym))
        symbols.insert(sym);
    }
  } symbols;

  SgClassDeclaration::traverseMemoryPoolNodes(decls);
  SgTemplateInstantiationDecl::traverseMemoryPoolNodes(decls);
  SgClassSymbol::traverseMemoryPoolNodes(symbols);

  SgFunctionDeclaration::traverseMemoryPoolNodes(decls);
  SgTemplateInstantiationFunctionDecl::traverseMemoryPoolNodes(decls);
  SgFunctionSymbol::traverseMemoryPoolNodes(symbols);

  SgMemberFunctionDeclaration::traverseMemoryPoolNodes(decls);
  SgMemberFunctionSymbol::traverseMemoryPoolNodes(symbols);

  SgVariableDeclaration::traverseMemoryPoolNodes(decls);
  SgInitializedName::traverseMemoryPoolNodes(decls);
  SgVariableSymbol::traverseMemoryPoolNodes(symbols);

  nodes_and_syms_to_graphviz_tree(os, decls.decls, symbols.symbols);
}
#endif /* ENABLE_plot_links */

////////////////////////
// AST merge function //
////////////////////////

#define DEBUG__ROSE_AST_MERGE 0

void merge(SgProject * project) {
#if DEBUG__ROSE_AST_MERGE
  printf("Rose::AST::merge: project = %x\n", project);
#endif
  TimingPerformance timer ("AST merge:");

  int nodes_start = numberOfNodes();
#if ENABLE_plot_links
  std::ofstream ofs_in("merging-before.dot");
  plot_links(ofs_in);
#endif

  shareRedundantNodes(project);
  deleteIslands(project);

#if ENABLE_plot_links
  std::ofstream ofs_mid("merging-middle.dot");
  plot_links(ofs_mid);
#endif

  link(project);

#if ENABLE_plot_links
  std::ofstream ofs_out("merging-after.dot");
  plot_links(ofs_out);
#endif

#if !DEBUG__ROSE_AST_MERGE
  if (SgProject::get_verbose() > 0) {
#endif
    int nodes_end = numberOfNodes();

    // Based on: http://en.wikipedia.org/wiki/Data_compression_ratio
    double percentageCompression  = ( nodes_end * 100.0 ) / nodes_start;
    double percentageSpaceSavings = 100.0 - percentageCompression;
    double mergeFactor            = ( ((double) nodes_start) / ((double) nodes_end) );

    double numberOfFiles  = project->numberOfFiles();
    double mergeEfficency = 0.0;
    if (numberOfFiles == 1) {
      mergeEfficency = 1.0 + percentageSpaceSavings;
    } else {
      double fileNumberMultiplier = numberOfFiles / (numberOfFiles - 1);
      mergeEfficency = percentageSpaceSavings * fileNumberMultiplier;
    }

    printf ("   Removed %d nodes from %d to %d:\n", nodes_start - nodes_end, nodes_start, nodes_end);
    printf ("      %2.4lf percent compression\n", percentageCompression);
    printf ("      %2.4lf percent space savings\n", percentageSpaceSavings);
    printf ("      mergeEfficency = %2.4lf\n", mergeEfficency);
    printf ("      mergeFactor = %2.4lf\n", mergeFactor);
#if !DEBUG__ROSE_AST_MERGE
  }
#endif
}

}
}

