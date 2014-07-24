
#include "DLX/Logger/language.hpp"

namespace DLX {

namespace Logger {

std::string language_t::language_label;
language_t::construct_label_map_t language_t::s_construct_labels;
language_t::construct_kinds_e language_t::s_blank_construct;
language_t::clause_labels_map_t language_t::s_clause_labels;
language_t::directives_relation_label_map_t language_t::s_directives_relation_labels;

void language_t::init() {

  // Language label

  language_label = "logger";

  // Language Constructs

  Directives::setConstructLabel<language_t>(e_logger_construct_log, "log");
  s_blank_construct = e_construct_last;

  // Language Clauses

  Directives::addClauseLabel<language_t>(e_logger_clause_where,   "where");
  Directives::addClauseLabel<language_t>(e_logger_clause_message, "message");
  Directives::addClauseLabel<language_t>(e_logger_clause_conds,    "cond");
  Directives::addClauseLabel<language_t>(e_logger_clause_params,  "params");

}

}

}

