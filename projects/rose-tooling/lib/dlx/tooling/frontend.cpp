
#include "DLX/Tooling/language.hpp"

namespace DLX {

namespace Frontend {

template <>
template <>
bool Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_set>(
  SgLocatedNode * directive_node,
  Directives::construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_set> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);

  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_init>(
  SgLocatedNode * directive_node,
  Directives::construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_init> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);

  SgVariableDeclaration * var_decl = isSgVariableDeclaration(SageInterface::getNextStatement(construct->assoc_nodes.pragma_decl));
  assert(var_decl != NULL);
  construct->assoc_nodes.var_to_init = SageInterface::getFirstInitializedName(var_decl);
  assert(construct->assoc_nodes.var_to_init != NULL);

  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_callgraph>(
  SgLocatedNode * directive_node,
  Directives::construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_callgraph> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);

  construct->assoc_nodes.function_decl = isSgFunctionDeclaration(SageInterface::getNextStatement(construct->assoc_nodes.pragma_decl));
  assert(construct->assoc_nodes.function_decl !=  NULL);

  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_outline>(
  SgLocatedNode * directive_node,
  Directives::construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_outline> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);

  construct->assoc_nodes.stmt = SageInterface::getNextStatement(construct->assoc_nodes.pragma_decl);
  assert(construct->assoc_nodes.stmt !=  NULL);

  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_replay>(
  SgLocatedNode * directive_node,
  Directives::construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_replay> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);

  construct->assoc_nodes.stmt = SageInterface::getNextStatement(construct->assoc_nodes.pragma_decl);
  assert(construct->assoc_nodes.stmt !=  NULL);

  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::findAssociatedNodes<Tooling::language_t::e_tooling_construct_grapher>(
  SgLocatedNode * directive_node,
  Directives::construct_t<Tooling::language_t, Tooling::language_t::e_tooling_construct_grapher> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);

  construct->assoc_nodes.stmt = SageInterface::getNextStatement(construct->assoc_nodes.pragma_decl);
  assert(construct->assoc_nodes.stmt !=  NULL);

  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_var>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_var> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.consume('('));
  parser.skip_whitespace();

  assert(parser.parse<SgVariableSymbol *>(clause->parameters.var_sym));

  parser.skip_whitespace();
  assert(parser.consume(')'));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_value>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_value> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.consume('('));
  parser.skip_whitespace();

  assert(parser.parse<SgExpression *>(clause->parameters.value_exp));

  parser.skip_whitespace();
  assert(parser.consume(')'));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_file>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_file> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.consume('('));
  parser.skip_whitespace();

  assert(parser.parse<boost::filesystem::path>(clause->parameters.file));

  parser.skip_whitespace();
  assert(parser.consume(')'));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_format>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_format> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.consume('('));
  parser.skip_whitespace();

  if (parser.consume("csv") || parser.consume("CSV"))
    clause->parameters.format = Directives::generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_format>::e_csv;
  else if (parser.consume("sqlite3") || parser.consume("SQLITE3") || parser.consume("SQLite3"))
    clause->parameters.format = Directives::generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_format>::e_sqlite3;
  else assert(false);

  parser.skip_whitespace();
  assert(parser.consume(')'));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_tag>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_tag> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.consume('('));
  parser.skip_whitespace();

  assert(parser.parse<int>(clause->parameters.tag));

  parser.skip_whitespace();
  assert(parser.consume(')'));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_pack>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_pack> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.parse_list<SgVariableSymbol *>(clause->parameters.symbols, '(', ')', ','));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
bool Frontend<Tooling::language_t>::build_graph(const std::map<SgLocatedNode *, directive_t *> & translation_map) {
  std::vector<Directives::directive_t<Tooling::language_t> *>::const_iterator it_directive_1, it_directive_2;
  for (it_directive_1 = directives.begin(); it_directive_1 != directives.end(); it_directive_1++) {
    for (it_directive_2 = directives.begin(); it_directive_2 != directives.end(); it_directive_2++) {
      if ( Tooling::language_t::s_constructs_dependencies.find(
             std::pair<Tooling::language_t::construct_kinds_e, Tooling::language_t::construct_kinds_e>(
              (*it_directive_1)->construct->kind, (*it_directive_2)->construct->kind
             )
           ) != Tooling::language_t::s_constructs_dependencies.end()
      ) {
        (*it_directive_1)->predecessor_list.push_back(
          std::pair<Tooling::language_t::directives_relation_e, Directives::directive_t<Tooling::language_t> *>(
            Tooling::language_t::e_depends_on, *it_directive_2
          )
        );
        (*it_directive_2)->successor_list.push_back(
          std::pair<Tooling::language_t::directives_relation_e, Directives::directive_t<Tooling::language_t> *>(
            Tooling::language_t::e_depends_on, *it_directive_1
          )
        );
      }
    }
  }

  graph_entry.clear();
  graph_final.clear();

  for (it_directive_1 = directives.begin(); it_directive_1 != directives.end(); it_directive_1++) {
    if ((*it_directive_1)->predecessor_list.empty())
      graph_entry.push_back(*it_directive_1);
    if ((*it_directive_1)->successor_list.empty())
      graph_final.push_back(*it_directive_1);
  }

  return true;
}

}

}

