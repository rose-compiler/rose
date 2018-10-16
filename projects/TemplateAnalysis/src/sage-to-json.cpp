
#include "sage3basic.h"
#include "nlohmann/json.hpp"

namespace ROSE {
  using nlohmann::json;

  namespace Viz {

    typedef std::map<std::string, std::string> property_map_t;
    void to_json(json & j, const property_map_t & p);

    typedef std::vector<std::string> property_list_t;

    typedef SgNode * node_t;
    std::string to_string(const node_t & n);

    typedef std::pair<node_t, node_t> edge_t;
    std::string to_string(const edge_t & e);

    typedef std::vector<std::pair<std::string, node_t> > destinations_t;
    void to_json(json & j, const destinations_t & p);

    class SageVisitor : public ROSE_VisitTraversal {
      protected:
        std::map<node_t, property_map_t> p_nodes;

        std::map<node_t, node_t > p_parent;

        std::map<node_t, destinations_t > p_traversal;

        std::map<edge_t, property_list_t> p_relations;

      private:
        bool hasNode(node_t n) const;
        property_map_t & getNode(node_t n);
        const property_map_t & getNode(node_t n) const;

        void setParent(SgNode * n);
        const node_t & getParent(SgNode * n) const;

        destinations_t & getTraversal(SgNode * n);
        const destinations_t & getTraversal(SgNode * n) const;

        bool hasRelations(node_t s, node_t d) const;
        bool hasRelations(edge_t e) const;
        property_list_t & getRelations(node_t s, node_t d);
        property_list_t & getRelations(edge_t e);
        const property_list_t & getRelations(node_t s, node_t d) const;
        const property_list_t & getRelations(edge_t e) const;

      public:
        SageVisitor();
        virtual ~SageVisitor();

        void toJson(json & out) const;
        void toGraphViz(std::ostream & out, bool parent_edges = true, bool structure_edges = true, bool relation_edges = true) const;

      protected:
        bool ignore(SgNode * n) const;

      public:
        virtual void visit(SgNode * n);
    };
  }
}

namespace ROSE { namespace Viz {

// Ctor/Dtor

SageVisitor::SageVisitor() :
  p_nodes(), p_parent(), p_traversal(), p_relations()
{}

SageVisitor::~SageVisitor() {}

// Nodes

bool SageVisitor::hasNode(node_t n) const {
  return p_nodes.find(n) != p_nodes.end();
}

property_map_t & SageVisitor::getNode(node_t n) {
  auto it = p_nodes.find(n);
  if (it == p_nodes.end()) {
    auto res = p_nodes.insert(std::pair<node_t, property_map_t>(n, property_map_t()));
    ROSE_ASSERT(res.second);
    return res.first->second;
  } else {
    return it->second;
  }
}

const property_map_t & SageVisitor::getNode(node_t n) const {
  auto it = p_nodes.find(n);
  ROSE_ASSERT(it != p_nodes.end());
  return it->second;
}

// Parent

void SageVisitor::setParent(node_t n) {
  if (n->get_parent())
    p_parent[n] = n->get_parent();
}

const node_t & SageVisitor::getParent(node_t n) const {
  auto it = p_parent.find(n);
  if (it != p_parent.end()) {
    return it->second;
  } else {
    return NULL;
  }
}

// Traversal

destinations_t & SageVisitor::getTraversal(SgNode * n) {
  auto it = p_traversal.find(n);
  if (it == p_traversal.end()) {
    auto res = p_traversal.insert(std::pair<node_t, destinations_t>(n, destinations_t()));
    ROSE_ASSERT(res.second);
    return res.first->second;
  } else {
    return it->second;
  }
}

const destinations_t & SageVisitor::getTraversal(SgNode * n) const {
  auto it = p_traversal.find(n);
  ROSE_ASSERT(it != p_traversal.end());
  return it->second;
}

// Relations

bool SageVisitor::hasRelations(node_t s, node_t d) const {
  return hasRelations(edge_t{s,d});
}

bool SageVisitor::hasRelations(edge_t e) const {
  return p_relations.find(e) != p_relations.end();
}

property_list_t & SageVisitor::getRelations(node_t s, node_t d) {
  return getRelations(edge_t{s,d});
}

property_list_t & SageVisitor::getRelations(edge_t e) {
  auto it = p_relations.find(e);
  if (it == p_relations.end()) {
    auto res = p_relations.insert(std::pair<edge_t, property_list_t>(e, property_list_t()));
    ROSE_ASSERT(res.second);
    return res.first->second;
  } else {
    return it->second;
  }
}

const property_list_t & SageVisitor::getRelations(node_t s, node_t d) const {
  return getRelations(edge_t{s,d});
}

const property_list_t & SageVisitor::getRelations(edge_t e) const {
  auto it = p_relations.find(e);
  ROSE_ASSERT(it != p_relations.end());
  return it->second;
}

// JSON

void SageVisitor::toJson(json & out) const {
  ROSE_ASSERT(out.is_object());
  ROSE_ASSERT(out.empty());

  out["nodes"] = json::array();
  for (auto n = p_nodes.begin(); n != p_nodes.end(); n++) {
    json desc = json(n->second);
    desc["tag"] = to_string(n->first);
    out["nodes"].push_back(desc);
  }

  out["parent"] = json::object();
  for (auto e = p_parent.begin(); e != p_parent.end(); e++) {
    out["parent"][to_string(e->first)] = to_string(e->second);
  }

  out["traversal"] = json::object();
  for (auto s = p_traversal.begin(); s != p_traversal.end(); s++) {
    std::string s_ = to_string(s->first);
    out["traversal"][s_] = json::object();
    for (auto d = s->second.begin(); d != s->second.end(); d++) {
      out["traversal"][s_][d->first] = to_string(d->second);
    }
  }

  out["relations"] = json::array();
  for (auto r = p_relations.begin(); r != p_relations.end(); r++) {
    json o = json::object();

    o["src"] = to_string(r->first.first);
    o["dst"] = to_string(r->first.second);
    o["properties"] = r->second;

    out["relations"].push_back(o);
  }
}

void SageVisitor::toGraphViz(std::ostream & out, bool parent_edges, bool structure_edges, bool relation_edges) const {
  out << "digraph sage {" << std::endl;
  for (auto n = p_nodes.begin(); n != p_nodes.end(); n++) {
    std::ostringstream label;
    for (auto prop = n->second.begin(); prop != n->second.end(); prop++) {
      label << prop->first << " = " << prop->second;
      if (prop != n->second.end())
        label << "\\n";
    }
    out << to_string(n->first) << "[label=\"" << label.str() << "\"];" << std::endl;
  }

  if (parent_edges) {
    for (auto e = p_parent.begin(); e != p_parent.end(); e++) {
      out << to_string(e->second) << " -> " << to_string(e->first) << "[constraint=true];" << std::endl;
    }
  }

 std::string constraint = parent_edges ? std::string("false") : std::string("true");

  if (structure_edges) {
    for (auto s = p_traversal.begin(); s != p_traversal.end(); s++) {
      for (auto d = s->second.begin(); d != s->second.end(); d++) {
        out << to_string(s->first) << " -> " << to_string(d->second) << "[label=\"" << d->first << "\", constraint=" << constraint << "];" << std::endl;
      }
    }
  }

  if (relation_edges) {
    for (auto r = p_relations.begin(); r != p_relations.end(); r++) {
      std::ostringstream label;
      for (auto lbl = r->second.begin(); lbl != r->second.end(); lbl++) {
        label << *lbl;
        if (lbl != r->second.end())
          label << ",";
      }
      out << to_string(r->first.first) << " -> " << to_string(r->first.second) << "[label=\"" << label.str() << "\", constraint=" << constraint << "];" << std::endl;
    }
  }

  out << "}" << std::endl;
}

void to_json(json & j, const property_map_t & p) {
  j = json::object();
  for (auto it = p.begin(); it != p.end(); it++)
    j[it->first] = it->second;
}

void from_json(const json & j, property_map_t & p) {
  assert(!"NIY !!!!"); // TODO
}

std::string to_string(const node_t & p) {
  std::ostringstream oss;
  oss << "n_" << std::hex << p;
  return oss.str();
}

std::string to_string(json & j, const edge_t & p) {
  std::ostringstream oss;
  oss << "e_" << std::hex << p.first << "_" << std::hex << p.second;
  return oss.str();
}

// Visit

bool SageVisitor::ignore(SgNode * n) const {
  return isSg_File_Info(n);
}

void SageVisitor::visit(SgNode * n) {
  if (ignore(n)) return;

  ROSE_ASSERT(!hasNode(n));

#if 0
  {
    std::cerr << "--------------------------------------------" << std::endl;
    std::cerr << "n = " << std::hex << n << " (" << n->class_name() << ")" << std::endl;
  }
#endif

  SgType                 * type     = isSgType(n);
  SgNamedType            * ntype    = isSgNamedType(n);

  SgExpression           * expr     = isSgExpression(n);
  SgVarRefExp            * vref     = isSgVarRefExp(n);

  SgSymbol               * sym      = isSgSymbol(n);
  SgNamespaceSymbol      * nsym     = isSgNamespaceSymbol(n);
  SgVariableSymbol       * vsym     = isSgVariableSymbol(n);
  SgFunctionSymbol       * fsym     = isSgFunctionSymbol(n);
  SgFunctionTypeSymbol   * ftsym    = isSgFunctionTypeSymbol(n);
  SgClassSymbol          * csym     = isSgClassSymbol(n);
  SgEnumSymbol           * esym     = isSgEnumSymbol(n);
  SgEnumFieldSymbol      * efsym    = isSgEnumFieldSymbol(n);
  SgNonrealSymbol        * nrsym    = isSgNonrealSymbol(n);
  SgTemplateSymbol       * tsym     = isSgTemplateSymbol(n);
  SgTypedefSymbol        * tdsym    = isSgTypedefSymbol(n);
  SgLabelSymbol          * lsym     = isSgLabelSymbol(n);
  SgAliasSymbol          * asym     = isSgAliasSymbol(n);

  SgStatement            * stmt     = isSgStatement(n);
  SgDeclarationStatement * declstmt = isSgDeclarationStatement(n);
  SgVariableDeclaration  * vdecl    = isSgVariableDeclaration(n);
  SgFunctionDeclaration  * fdecl    = isSgFunctionDeclaration(n);
  SgClassDeclaration     * cdecl    = isSgClassDeclaration(n);
  SgEnumDeclaration      * edecl    = isSgEnumDeclaration(n);
  SgTypedefDeclaration   * tddecl   = isSgTypedefDeclaration(n);
  SgScopeStatement       * scope    = isSgScopeStatement(n);
  SgClassDefinition      * cdefn    = isSgClassDefinition(n);

  SgInitializedName      * iname    = isSgInitializedName(n);

  // Node
  {
    property_map_t & properties = getNode(n);

    properties["kind"] = n->class_name();

    // Unparse
    if (type || expr) {
      properties["unparsed"] = n->unparseToString();
    }

    // Name
    if (ntype) {
      properties["name"] = ntype->get_name();
    } else if (sym) {
      properties["name"] = sym->get_name();
    } else if (fdecl) {
      properties["name"] = fdecl->get_name();
    } else if (cdecl) {
      properties["name"] = cdecl->get_name();
    } else if (edecl) {
      properties["name"] = edecl->get_name();
    } else if (tddecl) {
      properties["name"] = tddecl->get_name();
    } else if (iname) {
      properties["name"] = iname->get_name();
    }

    // Position

    // TODO more properties [ depends on node kind ]
  }

  setParent(n);

  // Traversal
  if (!isSgBaseClass(n)) { // FIXME get_traversalSuccessorContainer not defined for SgBaseClass
    destinations_t & traversal = getTraversal(n);

    const std::vector<SgNode *> & succ_nodes = n->get_traversalSuccessorContainer();
    const std::vector<std::string> & succ_names = n->get_traversalSuccessorNamesContainer();

    if (succ_nodes.size() == succ_names.size()) {
      for (size_t i = 0; i < succ_nodes.size(); i++) {
        if (succ_nodes[i] == NULL) continue;
        
        traversal.push_back(std::pair<std::string, node_t>(succ_names[i], succ_nodes[i]));
      }
    }
  }

  // Relations
  {

    // Type
    SgType * reltype = NULL;
    if (expr) {
      reltype = expr->get_type();
    } else if (fdecl) {
      reltype = fdecl->get_type();
    } else if (cdecl) {
      reltype = cdecl->get_type();
    } else if (edecl) {
      reltype = edecl->get_type();
    } else if (tddecl) {
      reltype = tddecl->get_type();
    } else if (iname) {
      reltype = iname->get_type();
    }
    if (reltype != NULL) {
      getRelations(n, reltype).push_back("type");
    }

    // Declaration
    SgNode * reldecl = NULL;
    if (nsym) {
      reldecl = nsym->get_declaration();
    } else if (vsym) {
      reldecl = vsym->get_declaration();
    } else if (fsym) {
      reldecl = fsym->get_declaration();
    } else if (ftsym) {
//    reldecl = ftsym->get_declaration(); // FIXME does not exists!
    } else if (csym) {
      reldecl = csym->get_declaration();
    } else if (esym) {
      reldecl = esym->get_declaration();
    } else if (efsym) {
      reldecl = efsym->get_declaration();
    } else if (nrsym) {
      reldecl = nrsym->get_declaration();
    } else if (tsym) {
      reldecl = tsym->get_declaration();
    } else if (tdsym) {
      reldecl = tdsym->get_declaration();
    } else if (lsym) {
      reldecl = lsym->get_declaration();
    } else if (asym) {
      reldecl = asym->get_declaration();
    } else if (ntype) {
      reldecl = ntype->get_declaration();
    }
    if (reldecl != NULL) {
      getRelations(n, reldecl).push_back("declaration");
    }

    // Scope
    SgScopeStatement * relscope = NULL;
    if (stmt) {
      relscope = stmt->get_scope();
    } else if (nsym) {
      relscope = nsym->get_scope();
    } else if (vsym) {
      relscope = vsym->get_scope();
    } else if (fsym) {
      relscope = fsym->get_scope();
    } else if (ftsym) {
//    relscope = ftsym->get_scope(); // FIXME does not exists!
    } else if (csym) {
      relscope = csym->get_scope();
    } else if (esym) {
      relscope = esym->get_scope();
    } else if (efsym) {
      relscope = efsym->get_scope();
    } else if (nrsym) {
      relscope = nrsym->get_scope();
    } else if (tsym) {
      relscope = tsym->get_scope();
    } else if (tdsym) {
      relscope = tdsym->get_scope();
    } else if (lsym) {
      relscope = lsym->get_scope();
    } else if (asym) {
      relscope = asym->get_scope();
    } else if (iname) {
      relscope = iname->get_scope();
#if 0
    } else if (vdecl) {
      relscope = vdecl->get_scope();
    } else if (fdecl) {
      relscope = fdecl->get_scope();
    } else if (cdecl) {
      relscope = cdecl->get_scope();
    } else if (edecl) {
      relscope = edecl->get_scope();
    } else if (tddecl) {
      relscope = tddecl->get_scope();
#endif
    }
    if (relscope != NULL) {
      getRelations(n, relscope).push_back("scope");
    }

    // Reference
    SgSymbol * relsym = NULL;
    if (vref) {
      relsym = vref->get_symbol();
    }
    if (relsym) {
      getRelations(n, relsym).push_back("reference");
    }

    // First & defining declarations
    if (declstmt) {
      if (declstmt->get_firstNondefiningDeclaration()) {
        getRelations(n, declstmt->get_firstNondefiningDeclaration()).push_back("first_nondefining_declaration");
      }
      if (declstmt->get_definingDeclaration()) {
        getRelations(n, declstmt->get_definingDeclaration()).push_back("defining_declaration");
      }
    }

    // Base classes
    if (cdefn) {
      SgBaseClassPtrList & bclass = cdefn->get_inheritances();
      for (auto it = bclass.begin(); it != bclass.end(); it++) {
        getRelations(n, *it).push_back("inherit");
      }
    }
  }
}

}}

// MAIN

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);
  SgProject * project = new SgProject(args);

  auto files = project->get_files();
  ROSE_ASSERT(files.size() == 1);

  std::string filename = files[0]->get_sourceFileNameWithoutPath();

  ROSE::Viz::SageVisitor sage_visitor;
  sage_visitor.traverseMemoryPool();

  ROSE::json out = ROSE::json::object();
  sage_visitor.toJson(out);

  std::ofstream jsonfile(filename + ".json");
  jsonfile << out.dump(4) << std::endl;
  jsonfile.close();

  std::ofstream dotfile_tree(filename + ".parent.dot");
  sage_visitor.toGraphViz(dotfile_tree, true, false, false);
  dotfile_tree.close();

  std::ofstream dotfile_traversal(filename + ".traversal.dot");
  sage_visitor.toGraphViz(dotfile_traversal, false, true, false);
  dotfile_traversal.close();

  std::ofstream dotfile_graph(filename + ".relation.dot");
  sage_visitor.toGraphViz(dotfile_graph, false, false, true);
  dotfile_graph.close();

  std::ofstream dotfile_full(filename + ".full.dot");
  sage_visitor.toGraphViz(dotfile_full, true, true, true);
  dotfile_full.close();

  return 0;
}

