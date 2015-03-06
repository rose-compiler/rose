
#include "DLX/Logger/language.hpp"

namespace DLX {

namespace Frontend {

template <>
template <>
bool Frontend<Logger::language_t>::findAssociatedNodes<Logger::language_t::e_logger_construct_log>(
  SgLocatedNode * directive_node,
  Directives::construct_t<Logger::language_t, Logger::language_t::e_logger_construct_log> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.logged_region = SageInterface::getNextStatement(construct->assoc_nodes.pragma_decl);
  assert(construct->assoc_nodes.logged_region != NULL);

  return true;
}

template <>
template <>
bool Frontend<Logger::language_t>::parseClauseParameters<Logger::language_t::e_logger_clause_where>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Logger::language_t, Logger::language_t::e_logger_clause_where> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.consume('('));
  parser.skip_whitespace();
  if (parser.consume("before"))
    clause->parameters.position = Directives::generic_clause_t<Logger::language_t>::parameters_t<Logger::language_t::e_logger_clause_where>::e_before;
  else if (parser.consume("after"))
    clause->parameters.position = Directives::generic_clause_t<Logger::language_t>::parameters_t<Logger::language_t::e_logger_clause_where>::e_after;
  else if (parser.consume("both"))
    clause->parameters.position = Directives::generic_clause_t<Logger::language_t>::parameters_t<Logger::language_t::e_logger_clause_where>::e_both;
  else assert(false);
  parser.skip_whitespace();
  assert(parser.consume(')'));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<Logger::language_t>::parseClauseParameters<Logger::language_t::e_logger_clause_message>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Logger::language_t, Logger::language_t::e_logger_clause_message> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.consume('('));
  parser.skip_whitespace();
  assert(parser.parse<std::string>(clause->parameters.message));
  parser.skip_whitespace();
  assert(parser.consume(')'));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<Logger::language_t>::parseClauseParameters<Logger::language_t::e_logger_clause_conds>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Logger::language_t, Logger::language_t::e_logger_clause_conds> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.consume('('));
  parser.skip_whitespace();
  assert(parser.parse<SgExpression *>(clause->parameters.expr));
  parser.skip_whitespace();
  assert(parser.consume(','));
  parser.skip_whitespace();
  assert(parser.parse<SgValueExp *>(clause->parameters.value));
  parser.skip_whitespace();
  assert(parser.consume(')'));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<Logger::language_t>::parseClauseParameters<Logger::language_t::e_logger_clause_params>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Logger::language_t, Logger::language_t::e_logger_clause_params> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.parse_list<SgExpression *>(clause->parameters.params, '(', ')', ','));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
bool Frontend<Logger::language_t>::build_graph(const std::map<SgLocatedNode *, directive_t *> & translation_map) {
  std::vector<Directives::directive_t<Logger::language_t> *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    switch ((*it_directive)->construct->kind) {
      case Logger::language_t::e_logger_construct_log:
        break;
      default:
        assert(false);
    }
  }

  graph_entry.clear();
  graph_final.clear();

  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    if ((*it_directive)->predecessor_list.empty())
      graph_entry.push_back(*it_directive);
    if ((*it_directive)->successor_list.empty())
      graph_final.push_back(*it_directive);
  }

  return true;
}

}

}

