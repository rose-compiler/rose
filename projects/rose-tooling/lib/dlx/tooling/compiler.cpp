
#include "DLX/Tooling/compiler.hpp"
#include "DLX/Tooling/language.hpp"

#include "sage3basic.h"

#include <cassert>

namespace DLX {

namespace Tooling {

compiler_modules_t::compiler_modules_t(SgProject * project_) :
  project(project_)
{}

}

namespace Compiler {

typedef Directives::construct_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_construct_set>       tooling_set_construct_t;
typedef Directives::construct_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_construct_init>      tooling_init_construct_t;
typedef Directives::construct_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_construct_callgraph> tooling_callgraph_construct_t;
typedef Directives::construct_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_construct_outline>   tooling_outline_construct_t;
typedef Directives::construct_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_construct_replay>    tooling_replay_construct_t;
typedef Directives::construct_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_construct_grapher>   tooling_grapher_construct_t;

typedef Directives::directive_t<DLX::Tooling::language_t>::clause_list_t clause_list_t;

typedef Directives::clause_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_clause_var>    tooling_var_clause_t;
typedef Directives::clause_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_clause_value>  tooling_value_clause_t;
typedef Directives::clause_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_clause_file>   tooling_file_clause_t;
typedef Directives::clause_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_clause_format> tooling_format_clause_t;
typedef Directives::clause_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_clause_tag>    tooling_tag_clause_t;
typedef Directives::clause_t<DLX::Tooling::language_t, DLX::Tooling::language_t::e_tooling_clause_pack>   tooling_pack_clause_t;

void processSetConstruct(tooling_set_construct_t * construct, const clause_list_t & clause) {
  std::cout << "processSetConstruct" << std::endl;
  /// \todo
}

void processInitConstruct(tooling_init_construct_t * construct, const clause_list_t & clause) {
  std::cout << "processInitConstruct" << std::endl;
  /// \todo
}

void processCallgraphConstruct(tooling_callgraph_construct_t * construct, const clause_list_t & clause) {
  std::cout << "processCallgraphConstruct" << std::endl;
  /// \todo
}

void processOutlineConstruct(tooling_outline_construct_t * construct, const clause_list_t & clause) {
  std::cout << "processOutlineConstruct" << std::endl;
  /// \todo
}

void processReplayConstruct(tooling_replay_construct_t * construct, const clause_list_t & clause) {
  std::cout << "processReplayConstruct" << std::endl;
  /// \todo
}

class ASTtoDot : public AstTopDownProcessing<SgNode *> {
  private:
    std::ostream & out;
  public:
    ASTtoDot(std::ostream & out_) : AstTopDownProcessing<SgNode *>(), out(out_) {}
    SgNode * evaluateInheritedAttribute(SgNode * node, SgNode * parent) {
      out << "node_" << node << " [label=\"" << node->class_name() << "\"];" << std::endl;
      if (parent != NULL)
        out << "node_" << parent << " -> node_" << node << std::endl;
      return node;
    }
};

void generateDotFromAST(std::ostream & out, SgNode * node) {
  ASTtoDot ast_to_dot(out);
  ast_to_dot.traverse(node, NULL);
}

void processGrapherConstruct(tooling_grapher_construct_t * construct, const clause_list_t & clause) {
  std::cout << "processGrapherConstruct" << std::endl;  

  clause_list_t::const_iterator it_clause;
  for (it_clause = clause.begin(); it_clause != clause.end(); it_clause++) {
    tooling_file_clause_t * file = (*it_clause)->is<DLX::Tooling::language_t::e_tooling_clause_file>();
    if (file != NULL) {
      /// \todo
    }
  }

  generateDotFromAST(std::cout, construct->assoc_nodes.stmt);
}

template <>
bool Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t>::compile(
  const Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t>::directives_ptr_set_t & directives,
  const Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t>::directives_ptr_set_t & graph_entry,
  const Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t>::directives_ptr_set_t & graph_final
) {
  Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t>::directives_ptr_set_t::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    Directives::directive_t<language_t> * directive = *it_directive;
    assert(directive != NULL);

    switch (directive->construct->kind) {
      case DLX::Tooling::language_t::e_tooling_construct_set:
        processSetConstruct(directive->construct->is<DLX::Tooling::language_t::e_tooling_construct_set>(), directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_init:
        processInitConstruct(directive->construct->is<DLX::Tooling::language_t::e_tooling_construct_init>(), directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_callgraph:
        processCallgraphConstruct(directive->construct->is<DLX::Tooling::language_t::e_tooling_construct_callgraph>(), directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_outline:
        processOutlineConstruct(directive->construct->is<DLX::Tooling::language_t::e_tooling_construct_outline>(), directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_replay:
        processReplayConstruct(directive->construct->is<DLX::Tooling::language_t::e_tooling_construct_replay>(), directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_grapher:
        processGrapherConstruct(directive->construct->is<DLX::Tooling::language_t::e_tooling_construct_grapher>(), directive->clause_list);
        break;
      default:
        assert(false);
    }
  }
  return true;
}

}

}

