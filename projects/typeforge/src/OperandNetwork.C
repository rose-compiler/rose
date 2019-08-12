
#include "OperandData.h"

#include "sage3basic.h"

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

OperandData<OperandKind::base> * OperandNetwork::addNode(SgLocatedNode * lnode) {
#if DEBUG__Typeforge__OperandNetwork__addNode
  std::cout << "ENTER Typeforge::OperandNetwork::addNode" << std::endl;
  std::cout << "  lnode = " << lnode << " ( " << lnode->class_name() << " )" << std::endl;
#endif

  OperandData<OperandKind::base> * od = nullptr;
  auto idata = node_to_data.find(lnode); 
  if (idata == node_to_data.end()) {
    od = OperandData<OperandKind::base>::build(lnode);
    node_to_data[lnode] = od;
    handle_to_data[od->handle] = od;
    kind_to_nodes[od->opkind].insert(lnode);
  } else {
    od = idata->second;
  }
  return od;

#if DEBUG__Typeforge__OperandNetwork__addNode
  std::cout << "LEAVE Typeforge::OperandNetwork::addNode" << std::endl;
#endif
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
      }

      SgInitializedName * iname = isSgInitializedName(lnode);

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

  std::vector<SgType *> casts;

  ExprPathDesc(SgLocatedNode * __root) :
    root(__root),
    last(nullptr),
    casts()
  {}

  ExprPathDesc(ExprPathDesc const & v) :
    root(v.root),
    last(v.last),
    casts(v.casts.begin(), v.casts.end())
  {}
};

struct ExpressionTraversal : public AstTopDownProcessing<ExprPathDesc> {
  std::vector<ExprPathDesc> paths;

  virtual ExprPathDesc evaluateInheritedAttribute(SgNode * n, ExprPathDesc attr) {
    SgLocatedNode * lnode = isSgLocatedNode(n);
    assert(lnode != nullptr);

    if (attr.root == nullptr) {
//    std::cout << "lnode = " << lnode << " ( " << lnode->class_name() << " )" << std::endl;   
      attr.root = lnode;
      return attr;   
    } else {
      assert(attr.last == nullptr);

      if (SgCastExp * cexp = isSgCastExp(n)) {
        attr.casts.push_back(cexp->get_type());
        return attr;
      } else {
        attr.last = lnode;
        paths.push_back(attr);
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
      root = iname;
    } else if (
        isSgForStatement(parent)  || // > case of the update expression
        isSgExprStatement(parent)    // > normal expression statement
    ) {
      // NOP
    } else {
      std::cerr << "ENTER Typeforge::OperandNetwork::initialize" << std::endl;
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

    OperandData<OperandKind::base> * source = nullptr;
    if (path.root) {
      source = addNode(path.root);
      assert(source != nullptr);
      source->successors.insert(path.last);
    }

    OperandData<OperandKind::base> * target = addNode(path.last);
    assert(target != nullptr);

    target->casts.insert(target->casts.end(), path.casts.begin(), path.casts.end());
    target->predeccessors.insert(path.root);
  }

#if DEBUG__Typeforge__OperandNetwork__initialize
  std::cout << "LEAVE Typeforge::OperandNetwork::initialize" << std::endl;
  std::cout << "  # nodes = " << node_to_data.size() << std::endl;
#endif
}

void OperandNetwork::toDot(std::string const & fname) const {
  std::fstream out;
  out.open(fname, std::ios::out | std::ios::trunc);
  toDot(out);
  out.close();
}

std::string getColor(OperandData<OperandKind::base> const * data) {
  if (!data->can_be_changed) {
    return "black";
  }

  switch (data->opkind) {
    case OperandKind::variable:
    case OperandKind::function:
    case OperandKind::parameter: {
      return "blue";
    }

    case OperandKind::varref:
    case OperandKind::fref:
    case OperandKind::value: {
      return "green";
    }

    case OperandKind::arithmetic:
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
  { OperandKind::base         , "BASE"         },
  { OperandKind::variable     , "VARIABLE"     },
  { OperandKind::function     , "FUNCTION"     },
  { OperandKind::parameter    , "PARAMETER"    },
  { OperandKind::varref       , "VARREF"       },
  { OperandKind::fref         , "FREF"         },
  { OperandKind::value        , "VALUE"        },
  { OperandKind::arithmetic   , "ARITHMETIC"   },
  { OperandKind::call         , "CALL"         },
  { OperandKind::array_access , "ARRAY_ACCESS" },
  { OperandKind::address_of   , "ADDRESS_OF"   },
  { OperandKind::dereference  , "DEREFERENCE"  },
  { OperandKind::unknown      , "UNKNOWN"      },
};

std::string encode_html(std::string const & data) {
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

void OperandNetwork::toDot_node(std::ostream & out, SgLocatedNode * node, OperandData<OperandKind::base> const * data) const {
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
    case OperandKind::value: {
      out << encode_html(node->unparseToString());
      break;
    }

    case OperandKind::arithmetic:
    case OperandKind::call:
    case OperandKind::array_access:
    case OperandKind::address_of:
    case OperandKind::dereference:
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
  out << "      <td colspan=\"9\">" << encode_html(data->original_type->unparseToString()) << "</td>" << std::endl;
  out << "    </tr>" << std::endl;

  out << "    <tr>" << std::endl;
  out << "      <td colspan=\"1\">Position</td>" << std::endl;
  out << "      <td colspan=\"5\">" << encode_html(data->filename) << "</td>" << std::endl;
  out << "      <td colspan=\"1\">" << data->line_start   << "</td>" << std::endl;
  out << "      <td colspan=\"1\">" << data->column_start << "</td>" << std::endl;
  out << "      <td colspan=\"1\">" << data->line_start   << "</td>" << std::endl;
  out << "      <td colspan=\"1\">" << data->column_start << "</td>" << std::endl;
  out << "    </tr>" << std::endl;

  out << "  </table>>";
  out << ", color=\"" << getColor(data) << "\"];" << std::endl;
}

void OperandNetwork::toDot_edge(std::ostream & out, SgLocatedNode * source, SgLocatedNode * target) const {
  out << "  n_" << source << " -> n_" << target << " [];" << std::endl;
}

void OperandNetwork::toDot(std::ostream & out) const {
#if DEBUG__Typeforge__OperandNetwork__toDot
  std::cout << "ENTER Typeforge::OperandNetwork::toDot" << std::endl;
#endif

  SgUnparse_Info * uinfo = new SgUnparse_Info();
      uinfo->set_SkipComments();
      uinfo->set_SkipWhitespaces();
      uinfo->set_SkipEnumDefinition();
      uinfo->set_SkipClassDefinition();
      uinfo->set_SkipFunctionDefinition();
      uinfo->set_SkipBasicBlock();
//    uinfo->set_isTypeFirstPart();

  out << "digraph {" << std::endl;

  std::set<std::pair<SgLocatedNode *, SgLocatedNode *> > edges;
  for (auto ndi : node_to_data) {
    OperandData<OperandKind::base> const * data = ndi.second;

    for (auto n : data->predeccessors) {
      edges.insert(std::pair<SgLocatedNode *, SgLocatedNode *>(n, ndi.first));
    }
    for (auto n : data->successors) {
      edges.insert(std::pair<SgLocatedNode *, SgLocatedNode *>(ndi.first, n));
    }

//  if (data->predeccessors.size() + data->successors.size() > 0) {
      toDot_node(out, ndi.first, data);
//  }
  }
  for (auto e : edges) {
    toDot_edge(out, e.first, e.second);
  }

  out << "}" << std::endl;

  delete uinfo;

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

OperandNetwork opnet;

}

