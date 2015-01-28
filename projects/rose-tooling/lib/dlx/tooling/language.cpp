
#include "DLX/Tooling/language.hpp"

namespace DLX {

namespace Tooling {

std::string language_t::language_label;
language_t::construct_label_map_t language_t::s_construct_labels;
language_t::construct_kinds_e language_t::s_blank_construct;
language_t::clause_labels_map_t language_t::s_clause_labels;
language_t::directives_relation_label_map_t language_t::s_directives_relation_labels;
language_t::constructs_dependencies_t language_t::s_constructs_dependencies;

void language_t::init() {

  // Language label

  language_label = "tooling";

  // Language Constructs

  Directives::setConstructLabel<language_t>(e_tooling_construct_set, "set");
  Directives::setConstructLabel<language_t>(e_tooling_construct_init, "init");
  Directives::setConstructLabel<language_t>(e_tooling_construct_callgraph, "callgraph");
  Directives::setConstructLabel<language_t>(e_tooling_construct_outline, "outline");
  Directives::setConstructLabel<language_t>(e_tooling_construct_replay, "replay");
  Directives::setConstructLabel<language_t>(e_tooling_construct_grapher, "grapher");
  s_blank_construct = e_construct_last;

  // Language Clauses

  Directives::addClauseLabel<language_t>(e_tooling_clause_var, "var");
  Directives::addClauseLabel<language_t>(e_tooling_clause_value, "value");
  Directives::addClauseLabel<language_t>(e_tooling_clause_file, "file");
  Directives::addClauseLabel<language_t>(e_tooling_clause_format, "format");
  Directives::addClauseLabel<language_t>(e_tooling_clause_tag, "tag");
  Directives::addClauseLabel<language_t>(e_tooling_clause_pack, "pack");

  //

  s_directives_relation_labels.insert(std::pair<directives_relation_e, std::string>(e_depends_on, "depends-on"));

  //

  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_callgraph, e_tooling_construct_set));
  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_outline,   e_tooling_construct_set));
  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_replay,    e_tooling_construct_set));
  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_grapher,   e_tooling_construct_set));

  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_callgraph, e_tooling_construct_init));
  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_outline,   e_tooling_construct_init));
  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_replay,    e_tooling_construct_init));
  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_grapher,   e_tooling_construct_init));

  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_callgraph, e_tooling_construct_outline));
  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_grapher,   e_tooling_construct_outline));

  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_callgraph, e_tooling_construct_replay));
  s_constructs_dependencies.insert(construct_kind_pair_t(e_tooling_construct_grapher,   e_tooling_construct_replay));
}

}

}

