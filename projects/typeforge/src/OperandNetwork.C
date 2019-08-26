
#include "sage3basic.h"

#include "OperandData.h"

// #include "sage3basic.h"

namespace Typeforge {

extern SgProject * project;

#ifndef DEBUG__Typeforge
#  define DEBUG__Typeforge 0
#endif
#ifndef DEBUG__Typeforge__OperandNetwork
#  define DEBUG__Typeforge__OperandNetwork DEBUG__Typeforge
#endif
#ifndef DEBUG__Typeforge__OperandNetwork__initialize
#  define DEBUG__Typeforge__OperandNetwork__initialize DEBUG__Typeforge__OperandNetwork
#endif
#ifndef DEBUG__Typeforge__OperandNetwork__addNode
#  define DEBUG__Typeforge__OperandNetwork__addNode DEBUG__Typeforge__OperandNetwork
#endif
#ifndef DEBUG__Typeforge__OperandNetwork__toDot
#  define DEBUG__Typeforge__OperandNetwork__toDot DEBUG__Typeforge__OperandNetwork
#endif
#ifndef DEBUG__Typeforge__ExpressionTraversal
#  define DEBUG__Typeforge__ExpressionTraversal DEBUG__Typeforge
#endif
#ifndef DEBUG__Typeforge__stripType
#  define DEBUG__Typeforge__stripType DEBUG__Typeforge
#endif

EdgeKind operator| (const EdgeKind & a, const EdgeKind & b) {
  assert(a != EdgeKind::unknown);
  assert(b != EdgeKind::unknown);

  EdgeKind r = EdgeKind::unknown;

  if (a == b) r = a;
  else if (a == EdgeKind::traversal || a == EdgeKind::value) r = b;
  else if (b == EdgeKind::traversal || b == EdgeKind::value) r = a;
  else if (a == EdgeKind::deref && b == EdgeKind::address) r = EdgeKind::value;
  else if (a == EdgeKind::address && b == EdgeKind::deref) r = EdgeKind::value;

  return r;
}

EdgeKind operator& (const EdgeKind & a, const EdgeKind & b) {
  assert(a != EdgeKind::unknown);
  assert(b != EdgeKind::unknown);

  // incompatible
  assert(a != EdgeKind::deref || b != EdgeKind::address);
  assert(a != EdgeKind::address || b != EdgeKind::deref);

  EdgeKind r = EdgeKind::unknown;

  if (a == b) r = a;
  else if (a == EdgeKind::traversal || a == EdgeKind::value) r = b;
  else if (b == EdgeKind::traversal || b == EdgeKind::value) r = a;

  return r;
}

OperandData<OperandKind::base> * OperandNetwork::addNode(SgLocatedNode * lnode) {
  assert(lnode != nullptr);

#if DEBUG__Typeforge__OperandNetwork__addNode
  std::cout << "ENTER Typeforge::OperandNetwork::addNode" << std::endl;
  std::cout << "  lnode = " << lnode << " ( " << ( lnode ? lnode->class_name() : "" ) << " )" << std::endl;
#endif

  OperandData<OperandKind::base> * od = nullptr;
  auto idata = node_to_data.find(lnode); 
  if (idata == node_to_data.end()) {
    od = OperandData<OperandKind::base>::build(lnode);
    assert(od != nullptr);

    node_to_data[lnode] = od;
    handle_to_data[od->handle] = od;
    kind_to_nodes[od->opkind].insert(lnode);
  } else {
    od = idata->second;
    assert(od != nullptr);
  }
  return od;

#if DEBUG__Typeforge__OperandNetwork__addNode
  std::cout << "LEAVE Typeforge::OperandNetwork::addNode" << std::endl;
#endif
}

void OperandNetwork::rmNode(SgLocatedNode * const node) {
  auto it = node_to_data.find(node);
  assert(it != node_to_data.end());

  kind_to_nodes[it->second->opkind].erase(node);

  handle_to_data.erase(it->second->handle);

  node_to_data.erase(it);

  assert(predeccessors[node].size() == 0);
  predeccessors.erase(node);

  assert(successors[node].size() == 0);
  successors.erase(node);
}

void OperandNetwork::addEdge(SgLocatedNode * const node_source, SgLocatedNode * const node_target, EdgeKind const ek) {
  assert(node_source != node_target);

  auto & pe = predeccessors[node_target][node_source];
  auto & se = successors[node_source][node_target];

  pe = pe & ek;
  se = se & ek;
}

void OperandNetwork::rmEdge(SgLocatedNode * const node_source, SgLocatedNode * const node_target) {
  auto it_source = node_to_data.find(node_source);
  assert(it_source != node_to_data.end());

  OperandData<OperandKind::base> * source_data = it_source->second;
  assert(source_data != nullptr);

  auto it_target = node_to_data.find(node_target);
  assert(it_target != node_to_data.end());

  OperandData<OperandKind::base> * target_data = it_target->second;
  assert(target_data != nullptr);

  successors[node_source].erase(node_target);
  predeccessors[node_target].erase(node_source);
}

void OperandNetwork::updateLabels(SgLocatedNode * pred_node, SgLocatedNode * old_node, SgLocatedNode * new_node) {
  auto it_pred = node_to_data.find(pred_node);
  assert(it_pred != node_to_data.end());

  OperandData<OperandKind::base> * pred_data = it_pred->second;
  assert(pred_data != nullptr);

  std::set<std::string> rm_labels;
  for (auto & p : pred_data->edge_labels) {
    if (p.second == old_node) {
      if (new_node != nullptr) {
        p.second = new_node;
      } else {
        rm_labels.insert(p.first);
      }
    }
  }
  for (auto s : rm_labels) {
    pred_data->edge_labels.erase(s);
  }
}

struct OperandCollector : public ROSE_VisitTraversal {
  std::set<SgExpression *> exproots;

  std::set<SgLocatedNode *> decls;

  void visit(SgNode * node) {
    assert(node != nullptr);

    SgLocatedNode * lnode = isSgLocatedNode(node);
    if (lnode != nullptr) {
      SgExpression * expr = isSgExpression(lnode);

      SgVariableDeclaration * vdecl = isSgVariableDeclaration(lnode);
      if (isSgTemplateVariableDeclaration(vdecl)) {
        vdecl = nullptr;
      }

      SgFunctionDeclaration * fdecl = isSgFunctionDeclaration(lnode);
      if (isSgTemplateFunctionDeclaration(fdecl) || isSgTemplateMemberFunctionDeclaration(fdecl)) {
        fdecl = nullptr;
      } else if (fdecl != nullptr && fdecl->get_firstNondefiningDeclaration() != fdecl) {
        fdecl = nullptr;
      }

      SgInitializedName * iname = isSgInitializedName(lnode);

      SgNode * lnode_parent = lnode->get_parent();

      if (iname && (
            lnode_parent == nullptr               ||
            isSgCtorInitializerList(lnode_parent) ||
            isSgEnumDeclaration(lnode_parent)     ||
            isSgTemplateParameter(lnode_parent)   ||
            isSgDeclarationScope(lnode_parent)
      )) {
        iname = nullptr;
      } else if (iname) {
        lnode_parent = lnode_parent->get_parent();
        SgDeclarationStatement * iname_fdecl = isSgFunctionDeclaration(lnode_parent);
        if (iname_fdecl != nullptr) {
          if (iname_fdecl->get_definingDeclaration() != nullptr) {
            iname_fdecl = iname_fdecl->get_definingDeclaration();
          } else {
            iname_fdecl = iname_fdecl->get_firstNondefiningDeclaration();
          }
          if (iname_fdecl != lnode_parent) {
            iname = nullptr;
          }
        }
      }

      if (expr && !isSgExpression(lnode->get_parent())) {
        exproots.insert(expr);
      } else if (iname && !isSgVariableDeclaration(lnode->get_parent())) {
        decls.insert(lnode);
      } else if (vdecl || fdecl) {
        decls.insert(lnode);
      }
    }
  }
};

struct ExprPathDesc {
  SgLocatedNode * root;
  SgLocatedNode * last;

  bool skip;

  std::vector<SgType *> casts;

  ExprPathDesc(SgLocatedNode * __root) :
    root(__root),
    last(nullptr),
    skip(false),
    casts()
  {}

  ExprPathDesc(ExprPathDesc const & v) :
    root(v.root),
    last(v.last),
    skip(v.skip),
    casts(v.casts.begin(), v.casts.end())
  {}
};

struct ExpressionTraversal : public AstTopDownProcessing<ExprPathDesc> {
  std::vector<ExprPathDesc> paths;

  virtual ExprPathDesc evaluateInheritedAttribute(SgNode * n, ExprPathDesc attr) {
    SgLocatedNode * lnode = isSgLocatedNode(n);
    assert(lnode != nullptr);

    if (attr.skip) return attr;

#if DEBUG__Typeforge__ExpressionTraversal
    std::cout << "ExpressionTraversal::evaluateInheritedAttribute" << std::endl;
    std::cout << "  n = " << n << " ( " << n->class_name() << " )" << std::endl;
    std::cout << "    parent = " << n->get_parent() << " ( " << ( n->get_parent() ? n->get_parent()->class_name() : "" ) << " )" << std::endl;
    std::cout << "  attr.root = " << attr.root << " ( " << ( attr.root ? attr.root->class_name() : "" ) << " )" << std::endl;
#endif

    if (attr.root == nullptr) {
      attr.root = lnode;
      return attr;   
    } else {
      assert(attr.last == nullptr);

      if (SgCastExp * cexp = isSgCastExp(n)) {
        attr.casts.push_back(cexp->get_type());
        return attr;
      } else if (isSgExprListExp(n)) {
        return attr;
      } else {
        attr.last = lnode;
        paths.push_back(attr);

#if DEBUG__Typeforge__ExpressionTraversal
        std::cout << "  attr.last = " << attr.last << " ( " << ( attr.last ? attr.last->class_name() : "" ) << " )" << std::endl;
#endif

        return ExprPathDesc(lnode);
      }
    }
  }
};

void OperandNetwork::initialize(SgProject * p) {
#if DEBUG__Typeforge__OperandNetwork__initialize
  std::cout << "ENTER Typeforge::OperandNetwork::initialize" << std::endl;
  std::cout << "  p = " << p << std::endl;
  std::cout << "  # nodes = " << node_to_data.size() << std::endl;
#endif

  if (p != nullptr) {
    assert(::Typeforge::project == nullptr || ::Typeforge::project == p);
    ::Typeforge::project = p;
  }
  assert(::Typeforge::project != nullptr);

  OperandCollector operand_collector;
  ExpressionTraversal expression_traversal;

  operand_collector.traverseMemoryPool();

#if DEBUG__Typeforge__OperandNetwork__initialize
  std::cout << " -> Declarations" << std::endl;
#endif
  for (auto decl : operand_collector.decls) {
#if DEBUG__Typeforge__OperandNetwork__initialize
    std::cout << "    decl = " << decl << " ( " << decl->class_name() << " )" << std::endl;
#endif

    addNode(decl);
  }

#if DEBUG__Typeforge__OperandNetwork__initialize
  std::cout << " -> Expression Roots" << std::endl;
#endif
  for (auto exproot : operand_collector.exproots) {

    SgNode * parent = exproot->get_parent();

    if (
         parent == nullptr                             ||
         isSgTemplateArgument(parent)                  ||
         isSgStaticAssertionDeclaration(parent)        ||
         isSgCaseOptionStmt(parent)                    || // < "case XXX:"
         isSgArrayType(parent)                         || // < "int A[XXX]"
         isSgVariableDeclaration(parent)               || // < "int :XXX" (bitfield)
         isSgTypeOfType(parent)                        || // < "typeof(XXX)"
         isSgDeclType(parent)                          || // < "decltype(XXX)"
         parent->variantT() == V_SgLocatedNode            // Weird case where the parent of an expression is a SgLocatedNode (shuold not happend as it is not a terminal node)
    ) {
      continue;
    }

#if DEBUG__Typeforge__OperandNetwork__initialize
    std::cout << "    exproot = " << exproot << " ( " << exproot->class_name() << " )" << std::endl;
    std::cout << "    parent  = " << parent  << " ( " << parent->class_name()  << " )" << std::endl;
#endif

    SgLocatedNode * root = nullptr;
    if (isSgReturnStmt(parent)) {
      SgFunctionDeclaration * fdecl = SageInterface::getEnclosingFunctionDeclaration(parent);
      assert(fdecl != nullptr);
      fdecl = isSgFunctionDeclaration(fdecl->get_firstNondefiningDeclaration());
      assert(fdecl != nullptr);

      if (!isSgTemplateFunctionDeclaration(fdecl) && !isSgTemplateMemberFunctionDeclaration(fdecl)) {
        root = fdecl;
      }
    } else if (
        SgInitializedName * iname = isSgInitializedName(parent)
    ) {
      if (SgVariableDeclaration * vdecl = isSgVariableDeclaration(iname->get_parent())) {
        root = vdecl;
      } else {
        root = iname;
      }
    } else if (
        isSgForStatement(parent)  || // > case of the update expression
        isSgExprStatement(parent)    // > normal expression statement
    ) {
      // NOP
    } else {
      std::cerr << "IN Typeforge::OperandNetwork::initialize" << std::endl;
      std::cerr << "    exproot = " << exproot << " ( " << exproot->class_name() << " )" << std::endl;
      std::cerr << "      ->get_parent() = " << parent << " ( " << ( parent ? parent->class_name() : "" ) << " )" << std::endl;
      assert(false);
    }

#if DEBUG__Typeforge__OperandNetwork__initialize
    std::cout << "    root = " << root << " ( " << ( root ? root->class_name() : "" ) << " )" << std::endl;
#endif
    ExprPathDesc attr(root);
    expression_traversal.traverse(exproot, attr);
  }

#if DEBUG__Typeforge__OperandNetwork__initialize
  std::cout << " -> Paths" << std::endl;
#endif
  for (auto path : expression_traversal.paths) {
#if DEBUG__Typeforge__OperandNetwork__initialize
    std::cout << "    path.root = " << path.root << " ( " << ( path.root ? path.root->class_name() : "" ) << " )" << std::endl;
    std::cout << "    path.last = " << path.last << " ( " << ( path.last ? path.last->class_name() : "" ) << " )" << std::endl;
#endif

    assert(path.last);

    OperandData<OperandKind::base> * target = addNode(path.last);
    assert(target != nullptr);

    target->casts.insert(target->casts.end(), path.casts.begin(), path.casts.end());

    if (path.root) {
      OperandData<OperandKind::base> * source = addNode(path.root);
      assert(source != nullptr);
      addEdge(path.root, path.last, EdgeKind::traversal);
    }
  }

#if DEBUG__Typeforge__OperandNetwork__initialize
  std::cout << "LEAVE Typeforge::OperandNetwork::initialize" << std::endl;
  std::cout << "  # nodes = " << node_to_data.size() << std::endl;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

void OperandNetwork::toDot(std::string const & fname) const {
  std::fstream out;
  out.open(fname, std::ios::out | std::ios::trunc);
  toDot(out);
  out.close();
}

std::string getColor(OperandData<OperandKind::base> const * data) {
  if (!data->can_be_changed) {
    return "grey";
  }

  switch (data->opkind) {
    case OperandKind::variable:
    case OperandKind::function:
    case OperandKind::parameter: {
      return "blue";
    }

    case OperandKind::varref:
    case OperandKind::fref:
    case OperandKind::thisref:
    case OperandKind::value: {
      return "green";
    }

    case OperandKind::assign:
    case OperandKind::member_access: {
      return "purple";
    }

    case OperandKind::unary_arithmetic:
    case OperandKind::binary_arithmetic: {
      return "chocolate";
    }

    case OperandKind::call:
    case OperandKind::array_access:
    case OperandKind::address_of:
    case OperandKind::dereference: {
      return "orange";
    }

    case OperandKind::unknown: {
      return "red";
    }

    case OperandKind::base:
    default: {
      std::abort();
    }
  }
}

std::map<OperandKind, std::string> opkind_strings{
  { OperandKind::base              , "BASE"          },
  { OperandKind::variable          , "VARIABLE"      },
  { OperandKind::function          , "FUNCTION"      },
  { OperandKind::parameter         , "PARAMETER"     },
  { OperandKind::varref            , "VARREF"        },
  { OperandKind::fref              , "FREF"          },
  { OperandKind::thisref           , "THISREF"       },
  { OperandKind::value             , "VALUE"         },
  { OperandKind::assign            , "ASSIGN"        },
  { OperandKind::unary_arithmetic  , "UNARY_ARITH"   },
  { OperandKind::binary_arithmetic , "BINARY_ARITH"  },
  { OperandKind::call              , "CALL"          },
  { OperandKind::array_access      , "ARRAY_ACCESS"  },
  { OperandKind::address_of        , "ADDRESS_OF"    },
  { OperandKind::dereference       , "DEREFERENCE"   },
  { OperandKind::member_access     , "MEMBER_ACCESS" },
  { OperandKind::unknown           , "UNKNOWN"       },
};

static std::string encode_html(std::string const & data) {
  std::string buffer;
  buffer.reserve(data.size());
  for (size_t pos = 0; pos != data.size(); ++pos) {
    switch (data[pos]) {
      case '&':  buffer.append("&amp;");       break;
      case '\"': buffer.append("&quot;");      break;
      case '\'': buffer.append("&apos;");      break;
      case '<':  buffer.append("&lt;");        break;
      case '>':  buffer.append("&gt;");        break;
      default:   buffer.append(&data[pos], 1); break;
    }
  }
  return buffer;
}

static void toDot_node(std::ostream & out, SgLocatedNode * node, OperandData<OperandKind::base> const * data, SgUnparse_Info * uinfo) {
  out << "  n_" << node << " [shape=plaintext, label=<" << std::endl;
  out << "  <table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"5\">" << std::endl;
  out << "    <tr>" << std::endl;
  out << "      <td colspan=\"1\">" << opkind_strings[data->opkind] << "</td>" << std::endl;
  out << "      <td colspan=\"9\">";
  switch (data->opkind) {
    case OperandKind::variable:
    case OperandKind::function:
    case OperandKind::parameter: {
      out << encode_html(data->handle);
      break;
    }

    case OperandKind::varref:
    case OperandKind::fref:
    case OperandKind::thisref:
    case OperandKind::value: {
      out << encode_html(node->unparseToString());
      break;
    }

    case OperandKind::assign:
    case OperandKind::unary_arithmetic:
    case OperandKind::binary_arithmetic:
    case OperandKind::call:
    case OperandKind::array_access:
    case OperandKind::address_of:
    case OperandKind::dereference:
    case OperandKind::member_access:
    case OperandKind::unknown: {
      out << node->class_name();
      break;
    }

    case OperandKind::base:
    default: {
      std::abort();
    }
  }
  out << "      </td>" << std::endl;
  out << "    </tr>" << std::endl;

  out << "    <tr>" << std::endl;
  out << "      <td colspan=\"1\">Type</td>" << std::endl;
  if (isSgClassType(data->original_type->stripType())) {
    uinfo->set_isTypeFirstPart();
  } else {
    uinfo->unset_isTypeFirstPart();
  }
  out << "      <td colspan=\"9\">" << encode_html(globalUnparseToString(data->original_type, uinfo)) << "</td>" << std::endl;
  out << "    </tr>" << std::endl;

  out << "    <tr>" << std::endl;
  out << "      <td colspan=\"1\">Position</td>" << std::endl;
  out << "      <td colspan=\"5\">" << encode_html(data->filename) << "</td>" << std::endl;
  out << "      <td colspan=\"1\">" << data->line_start   << "</td>" << std::endl;
  out << "      <td colspan=\"1\">" << data->column_start << "</td>" << std::endl;
  out << "      <td colspan=\"1\">" << data->line_start   << "</td>" << std::endl;
  out << "      <td colspan=\"1\">" << data->column_start << "</td>" << std::endl;
  out << "    </tr>" << std::endl;

  size_t i = 0;
  for (auto cast : data->casts) {
    out << "    <tr>" << std::endl;
    out << "      <td colspan=\"1\">Cast #" << i++ << "</td>" << std::endl;
    if (isSgClassType(cast->stripType())) {
      uinfo->set_isTypeFirstPart();
    } else {
      uinfo->unset_isTypeFirstPart();
    }
    out << "      <td colspan=\"9\">" << encode_html(globalUnparseToString(cast, uinfo)) << "</td>" << std::endl;
    out << "    </tr>" << std::endl;
  }

  out << "  </table>>";
  out << ", color=\"" << getColor(data) << "\"];" << std::endl;
}

struct edge_descriptor_t {
  EdgeKind ekind;
  std::string label;
  bool constraint;

  edge_descriptor_t() :
    ekind(EdgeKind::unknown),
    label(),
    constraint(true)
  {}

  edge_descriptor_t(
    EdgeKind ekind__,
    std::string label__,
    bool constraint__
  ) :
    ekind(ekind__),
    label(label__),
    constraint(constraint__)
  {}
};

typedef std::map<std::pair<SgLocatedNode *, SgLocatedNode *>, edge_descriptor_t > edges_map;
typedef std::pair<std::pair<SgLocatedNode *, SgLocatedNode *>, edge_descriptor_t > edges_entry;

static edge_descriptor_t & getEdge(
  SgLocatedNode * s, SgLocatedNode * t,
  edges_map & edges
) {
  std::pair<SgLocatedNode *, SgLocatedNode *> e(s,t); 
  auto it = edges.find(e);
  if (it == edges.end()) {
    auto r = edges.insert(edges_entry(e, edge_descriptor_t()));
    assert(r.second);
    return r.first->second;
  } else {
    return it->second;
  }
}

static void toDot_edge(std::ostream & out, edges_entry const & edge) {
  out << "  n_" << edge.first.first << " -> n_" << edge.first.second << " [";
  if (edge.second.label.size() > 0) {
    out << "label=\"" << encode_html(edge.second.label) << "\", ";
  }
  switch (edge.second.ekind) {
    case EdgeKind::value:     out << "color=\"blue\", ";   break;
    case EdgeKind::address:   out << "color=\"red\", ";    break;
    case EdgeKind::deref:     out << "color=\"green\", ";  break;
    case EdgeKind::traversal: out << "color=\"purple\", "; break;
    case EdgeKind::unknown:   out << "color=\"black\", ";
  }
  if (edge.second.constraint) {
    out << "constraint=true, ";
  } else {
    out << "constraint=false, ";
  }
  out << "];" << std::endl;
}

void OperandNetwork::toDot(std::ostream & out) const {
#if DEBUG__Typeforge__OperandNetwork__toDot
  std::cout << "ENTER Typeforge::OperandNetwork::toDot" << std::endl;
#endif

  out << "digraph {" << std::endl;

  std::set<SgLocatedNode *> nodes;
  edges_map edges;

  for (auto ndi : node_to_data) {
    OperandData<OperandKind::base> const * data = ndi.second;

    if (data->from_system_files) continue;

#if DEBUG__Typeforge__OperandNetwork__toDot
    std::cout << "  ndi.first = " << ndi.first << " ( " << ndi.first->class_name() << " )" << std::endl;
#endif

    nodes.insert(ndi.first);

#if DEBUG__Typeforge__OperandNetwork__toDot
    std::cout << "  # predeccessors = " << predeccessors[ndi.first].size() << std::endl;
    size_t cnt = 0;
#endif
    auto it_preds = predeccessors.find(ndi.first);
    if (it_preds != predeccessors.end()) {
      for (auto n : it_preds->second) {
#if DEBUG__Typeforge__OperandNetwork__toDot
        std::cout << "    [" <<  cnt++ << "] = " << n.first << " ( " << n.first->class_name() << " )" << std::endl;
#endif
        nodes.insert(n.first);
        auto & edge = getEdge(n.first, ndi.first, edges);
        edge.ekind = n.second;
      }
    }

#if DEBUG__Typeforge__OperandNetwork__toDot
    std::cout << "  # successors = " << successors[ndi.first].size() << std::endl;
    cnt = 0;
#endif
    auto it_succs = successors.find(ndi.first);
    if (it_succs != successors.end()) {
      for (auto n : it_succs->second) {
#if DEBUG__Typeforge__OperandNetwork__toDot
        std::cout << "    [" <<  cnt++ << "] = " << n.first << " ( " << n.first->class_name() << " )" << std::endl;
#endif
        nodes.insert(n.first);
        auto & edge = getEdge(ndi.first, n.first, edges);
        edge.ekind = n.second;
      }
    }

#if DEBUG__Typeforge__OperandNetwork__toDot
    std::cout << "  # edge_labels = " << data->edge_labels.size() << std::endl;
    cnt = 0;
#endif
    for (auto n : data->edge_labels) {
#if DEBUG__Typeforge__OperandNetwork__toDot
      std::cout << "    [" <<  cnt++ << "] = " << n.second << " ( " << n.second->class_name() << " )" << std::endl;
#endif
      if (n.second == nullptr) continue;
      nodes.insert(n.second);

      auto & edge = getEdge(ndi.first, n.second, edges);
      edge.label = n.first;
    }
  }

  {
    SgUnparse_Info * uinfo = new SgUnparse_Info();
      uinfo->set_SkipComments();
      uinfo->set_SkipWhitespaces();
      uinfo->set_SkipEnumDefinition();
      uinfo->set_SkipClassDefinition();
      uinfo->set_SkipFunctionDefinition();
      uinfo->set_SkipBasicBlock();
//    uinfo->set_isTypeFirstPart();
    for (auto n: nodes) {
      auto it = node_to_data.find(n);
      if (it == node_to_data.end()) {
        continue;
      }
      toDot_node(out, n, it->second, uinfo);
    }
    delete uinfo;
  }

  for (auto e : edges) {
    if (
      node_to_data.find(e.first.first)  == node_to_data.end() ||
      node_to_data.find(e.first.second) == node_to_data.end()
    ) {
      continue;
     }
    toDot_edge(out, e);
  }

  out << "}" << std::endl;

#if DEBUG__Typeforge__OperandNetwork__toDot
  std::cout << "LEAVE Typeforge::OperandNetwork::toDot" << std::endl;
#endif
}

///////////////////
// 5 - Accessors //
///////////////////

std::string const OperandNetwork::getHandle(SgLocatedNode * const lnode) const {
  auto n = node_to_data.find(lnode);
  return ( n != node_to_data.end() ) ? n->second->handle : "";
}

SgLocatedNode * const OperandNetwork::getNode(std::string const & handle) const {
  auto n = handle_to_data.find(handle);
  return ( n != handle_to_data.end() ) ? n->second->lnode : nullptr;
}

SgType * const OperandNetwork::getOriginalType(SgLocatedNode * const lnode) const {
  auto n = node_to_data.find(lnode);
  return ( n != node_to_data.end() ) ? n->second->original_type : nullptr;
}

SgType * const OperandNetwork::getOriginalType(std::string const & handle) const {
  auto n = handle_to_data.find(handle);
  return ( n != handle_to_data.end() ) ? n->second->original_type : nullptr;
}

std::string const OperandNetwork::getFilename(SgLocatedNode * const lnode) const {
  auto n = node_to_data.find(lnode);
  return ( n != node_to_data.end() ) ? n->second->filename : "";
}

std::string const OperandNetwork::getFilename(std::string const & handle) const {
  auto n = handle_to_data.find(handle);
  return ( n != handle_to_data.end() ) ? n->second->filename : "";
}

size_t const OperandNetwork::getLineStart(SgLocatedNode * const lnode) const {
  auto n = node_to_data.find(lnode);
  return ( n != node_to_data.end() ) ? n->second->line_start : 0;
}

size_t const OperandNetwork::getLineStart(std::string const & handle) const {
  auto n = handle_to_data.find(handle);
  return ( n != handle_to_data.end() ) ? n->second->line_start : 0;
}

size_t const OperandNetwork::getLineEnd(SgLocatedNode * const lnode) const {
  auto n = node_to_data.find(lnode);
  return ( n != node_to_data.end() ) ? n->second->line_end : 0;
}

size_t const OperandNetwork::getLineEnd(std::string const & handle) const {
  auto n = handle_to_data.find(handle);
  return ( n != handle_to_data.end() ) ? n->second->line_end : 0;
}

size_t const OperandNetwork::getColumnStart(SgLocatedNode * const lnode) const {
  auto n = node_to_data.find(lnode);
  return ( n != node_to_data.end() ) ? n->second->column_start : 0;
}

size_t const OperandNetwork::getColumnStart(std::string const & handle) const {
  auto n = handle_to_data.find(handle);
  return ( n != handle_to_data.end() ) ? n->second->column_start : 0;
}

size_t const OperandNetwork::getColumnEnd(SgLocatedNode * const lnode) const {
  auto n = node_to_data.find(lnode);
  return ( n != node_to_data.end() ) ? n->second->column_end : 0;
}

size_t const OperandNetwork::getColumnEnd(std::string const & handle) const {
  auto n = handle_to_data.find(handle);
  return ( n != handle_to_data.end() ) ? n->second->column_end : 0;
}

std::set<SgLocatedNode *> const & OperandNetwork::getAll(const OperandKind opkind) const {
  auto s = kind_to_nodes.find(opkind);
  if (s == kind_to_nodes.end()) {
    std::abort();
  }
  return s->second;
}

OperandNetwork opnet;

namespace CollapseRules {

const std::string AssignOperator::lhs{"ref"};
const std::string AssignOperator::rhs{"val"};
const std::string MemberAccess::lhs{"obj"};
const std::string MemberAccess::rhs{"fld"};
const std::string ArrayAccess::lhs{"arr"};
const std::string ArrayAccess::rhs{"idx"};
const std::string Dereference::operand{"ref"};
const std::string AddressOf::operand{"obj"};

}

static SgType * stripType__(SgType * type, bool strip_std_vector) {
  assert(type != NULL);

#if DEBUG__Typeforge__stripType
  std::cout << "Typeforge::stripType" << std::endl;
  std::cout << "  type      = " << type << " ( " << type->class_name() << "): " << type->unparseToString() << "" << std::endl;
#endif

  type = type->stripType(
    SgType::STRIP_ARRAY_TYPE     |
    SgType::STRIP_POINTER_TYPE   |
    SgType::STRIP_MODIFIER_TYPE  |
    SgType::STRIP_REFERENCE_TYPE |
    SgType::STRIP_RVALUE_REFERENCE_TYPE
  );

  SgTypedefType * td_type = isSgTypedefType(type);
  SgClassType * xtype = isSgClassType(type);
  if (td_type != nullptr) {
    type = stripType__(td_type->get_base_type(), strip_std_vector);
  } else if (strip_std_vector && xtype != nullptr) {
    SgDeclarationStatement * decl_stmt = xtype->get_declaration();
    assert(decl_stmt != nullptr);

#if DEBUG__Typeforge__stripType
    std::cout << "  decl_stmt = " << decl_stmt << " ( " << decl_stmt->class_name() << "): " << decl_stmt->unparseToString() << "" << std::endl;
#endif

    SgTemplateInstantiationDecl * ti_decl = isSgTemplateInstantiationDecl(decl_stmt);
    if (ti_decl != nullptr) {
#if DEBUG__Typeforge__stripType
      std::cout << "      ->get_qualified_name() = " << ti_decl->get_qualified_name() << std::endl;
      std::cout << "      ->get_name()           = " << ti_decl->get_name() << std::endl;
#endif

      SgTemplateClassDeclaration * td_decl = ti_decl->get_templateDeclaration();
      assert(td_decl != nullptr);

#if DEBUG__Typeforge__stripType
      std::cout << "  td_decl   = " << td_decl << " ( " << td_decl->class_name() << "): " << td_decl->unparseToString() << "" << std::endl;
      std::cout << "      ->get_qualified_name() = " << td_decl->get_qualified_name() << std::endl;
#endif

      if (td_decl->get_qualified_name() == "::std::vector") {
        auto tpl_args = ti_decl->get_templateArguments();
        assert(tpl_args.size() > 0);

        SgType * tpl_type_arg = tpl_args[0]->get_type();
        assert(tpl_type_arg != nullptr);

        type = stripType__(tpl_type_arg, false);
      }
    }
  }

  return type;
}

bool is_template_with_dependent_return_type(lnode_ptr fnc) {
  if (SgTemplateInstantiationFunctionDecl * ti_fdecl = isSgTemplateInstantiationFunctionDecl(fnc)) {
    // case: call to: template < ... , typename Tx , ... > Tx const & foo(...);
    SgTemplateFunctionDeclaration * t_fdecl = ti_fdecl->get_templateDeclaration();
    assert(t_fdecl != nullptr);
    SgFunctionType * ftype = t_fdecl->get_type();
    assert(ftype != nullptr);
    SgType * r_ftype = ftype->get_return_type();
    assert(r_ftype != nullptr);
    SgNonrealType * nrtype = isSgNonrealType(::Typeforge::stripType__(r_ftype, true));
    if (nrtype != nullptr) {
      SgNonrealDecl * nrdecl = isSgNonrealDecl(nrtype->get_declaration());
      assert(nrdecl != nullptr);
      if (nrdecl->get_is_template_param()) { // TODO check that it is a template parameter from `t_fdecl`
        return true;
      }
    }
  }
  return false;
}

}

