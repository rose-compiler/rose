
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

void processSetConstruct(tooling_set_construct_t * construct, const clause_list_t & clause) {
  std::cout << "processSetConstruct" << std::endl;
  /// \todo
}

void processInitConstruct(tooling_init_construct_t * construct, const clause_list_t & clause) {
  /// \todo
}

void processCallgraphConstruct(tooling_callgraph_construct_t * construct, const clause_list_t & clause) {
  /// \todo
}

void processOutlineConstruct(tooling_outline_construct_t * construct, const clause_list_t & clause) {
  /// \todo
}

void processReplayConstruct(tooling_replay_construct_t * construct, const clause_list_t & clause) {
  /// \todo
}

void processGrapherConstruct(tooling_grapher_construct_t * construct, const clause_list_t & clause) {
  /// \todo
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
        processSetConstruct((tooling_set_construct_t *)directive->construct, directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_init:
        processInitConstruct((tooling_init_construct_t *)directive->construct, directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_callgraph:
        processCallgraphConstruct((tooling_callgraph_construct_t *)directive->construct, directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_outline:
        processOutlineConstruct((tooling_outline_construct_t *)directive->construct, directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_replay:
        processReplayConstruct((tooling_replay_construct_t *)directive->construct, directive->clause_list);
        break;
      case DLX::Tooling::language_t::e_tooling_construct_grapher:
        processGrapherConstruct((tooling_grapher_construct_t *)directive->construct, directive->clause_list);
        break;
      default:
        assert(false);
    }
  }
  return true;
}

}

}

