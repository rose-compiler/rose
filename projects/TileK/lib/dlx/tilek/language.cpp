
#include "DLX/Core/directives.hpp"
#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/TileK/language.hpp"

#include <cassert>

namespace DLX {

namespace TileK {

std::string language_t::language_label;
language_t::construct_label_map_t language_t::s_construct_labels;
language_t::construct_kinds_e language_t::s_blank_construct;
language_t::clause_labels_map_t language_t::s_clause_labels;
language_t::directives_relation_label_map_t language_t::s_directives_relation_labels;

void language_t::init() {
  language_label = "tilek";

  Directives::setConstructLabel<language_t>(e_construct_kernel, "kernel");
  Directives::setConstructLabel<language_t>(e_construct_loop, "loop");

  s_blank_construct = e_construct_last;

  Directives::addClauseLabel<language_t>(e_clause_data, "data");
  Directives::addClauseLabel<language_t>(e_clause_tile, "tile");

  s_directives_relation_labels.insert(std::pair<directives_relation_e, std::string>(e_child_scope, "child-scope"));
  s_directives_relation_labels.insert(std::pair<directives_relation_e, std::string>(e_parent_scope, "parent-scope"));
}

}

}

