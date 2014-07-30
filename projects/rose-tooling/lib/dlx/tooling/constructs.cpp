
#include "DLX/Tooling/language.hpp"

namespace DLX {

namespace Directives {

template <>
generic_construct_t<Tooling::language_t> * buildConstruct<Tooling::language_t>(Tooling::language_t::construct_kinds_e kind) {
  switch (kind) {
    case Tooling::language_t::e_tooling_construct_set:
     return new construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_set>();
    case Tooling::language_t::e_tooling_construct_init:
     return new construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_init>();
    case Tooling::language_t::e_tooling_construct_callgraph:
     return new construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_callgraph>();
    case Tooling::language_t::e_tooling_construct_outline:
     return new construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_outline>();
    case Tooling::language_t::e_tooling_construct_replay:
     return new construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_replay>();
    case Tooling::language_t::e_tooling_construct_grapher:
     return new construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_grapher>();
    case Tooling::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

template <>
bool findAssociatedNodes<Tooling::language_t>(
  SgLocatedNode * directive_node,
  generic_construct_t<Tooling::language_t> * construct,
  const std::map<SgLocatedNode *, directive_t<Tooling::language_t> *> & translation_map
) {
  switch (construct->kind) {
    case Tooling::language_t::e_tooling_construct_set:
     return Frontend::Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_set>(
       directive_node, (construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_set> *)construct, translation_map
     );
    case Tooling::language_t::e_tooling_construct_init:
     return Frontend::Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_init>(
       directive_node, (construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_init> *)construct, translation_map
     );
    case Tooling::language_t::e_tooling_construct_callgraph:
     return Frontend::Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_callgraph>(
       directive_node, (construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_callgraph> *)construct, translation_map
     );
    case Tooling::language_t::e_tooling_construct_outline:
     return Frontend::Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_outline>(
       directive_node, (construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_outline> *)construct, translation_map
     );
    case Tooling::language_t::e_tooling_construct_replay:
     return Frontend::Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_replay>(
       directive_node, (construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_replay> *)construct, translation_map
     );
    case Tooling::language_t::e_tooling_construct_grapher:
     return Frontend::Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_grapher>(
       directive_node, (construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_grapher> *)construct, translation_map
     );
    case Tooling::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

}

}

