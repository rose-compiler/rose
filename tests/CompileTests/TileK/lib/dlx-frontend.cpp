
#include "sage3basic.h"

#include "DLX/Core/frontend.hpp"
#include "DLX/Core/directives.hpp"
#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/TileK/language.hpp"

#include <queue>
#include <iostream>

#include <cassert>

namespace DLX {

namespace Frontend {

template <> 
template <> 
bool Frontend<TileK::language_t>::findAssociatedNodes<TileK::language_t::e_construct_kernel>(
  SgLocatedNode * directive_node,
  Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_kernel> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.kernel_region = isSgScopeStatement(SageInterface::getNextStatement(pragma_decl));
  assert(construct->assoc_nodes.kernel_region != NULL);

  return true;
}

template <> 
template <> 
bool Frontend<TileK::language_t>::findAssociatedNodes<TileK::language_t::e_construct_loop>(
  SgLocatedNode * directive_node,
  Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_loop> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.for_loop = isSgForStatement(SageInterface::getNextStatement(pragma_decl));
  assert(construct->assoc_nodes.for_loop != NULL);

  return true;
}

template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_data>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_data> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  if (!parser.parse_list(clause->parameters.data_sections, '(', ')', ',')) return false;

  directive_str = parser.getDirectiveString(); return true;
}

template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_tile>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_tile> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  if (parser.consume('[')) {
    parser.skip_whitespace();
    if (!parser.parse<size_t>(clause->parameters.order)) return false;
    parser.skip_whitespace();
    if (!parser.consume(']')) return false;
  }
  else clause->parameters.order = -1;

  if (!parser.consume('(')) return false;
  parser.skip_whitespace();
  if (parser.consume("dynamic")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_dynamic_tile;
    clause->parameters.param = NULL;
  }
  else if (parser.consume("static")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_static_tile;
    parser.skip_whitespace();
    if (!parser.consume(',')) return false;
    parser.skip_whitespace();
    if (!parser.parse<SgExpression *>(clause->parameters.param)) return false;
  }
#ifdef TILEK_THREADS
  else if (parser.consume("thread")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_thread_tile;
    clause->parameters.param = NULL;
  }
#endif
#ifdef TILEK_ACCELERATOR
  else if (parser.consume("gang")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_gang_tile;
    parser.skip_whitespace();
    if (!parser.consume(',')) return false;
    parser.skip_whitespace();
    if (!parser.parse<SgExpression *>(clause->parameters.param)) return false;
  }
  else if (parser.consume("worker")) {
    clause->parameters.kind = Directives::generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile>::e_worker_tile;
    parser.skip_whitespace();
    if (!parser.consume(',')) return false;
    parser.skip_whitespace();
    if (!parser.parse<SgExpression *>(clause->parameters.param)) return false;
  }
#endif
  else return false;

  parser.skip_whitespace();
  if (!parser.consume(')')) return false;

  directive_str = parser.getDirectiveString(); return true;
}

#ifdef TILEK_THREADS
template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_num_threads>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_num_threads> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  if (!parser.consume('('))                                          return false;
  if (!parser.parse<SgExpression *>(clause->parameters.num_threads)) return false;
  if (!parser.consume(')'))                                          return false;

  directive_str = parser.getDirectiveString(); return true;
}
#endif

#ifdef TILEK_ACCELERATOR
template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_num_gangs>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_num_gangs> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  if (parser.consume('[')) {
    parser.skip_whitespace();
    if (!parser.parse<size_t>(clause->parameters.gang_id)) return false;
    parser.skip_whitespace();
    if (!parser.consume(']')) return false;
  }
  else clause->parameters.gang_id = 1;

  if (!parser.consume('('))                                return false;
  if (!parser.parse<SgExpression *>(clause->parameters.num_gangs)) return false;
  if (!parser.consume(')'))                                return false;

  directive_str = parser.getDirectiveString(); return true;
}

template <>
template <>
bool Frontend<TileK::language_t>::parseClauseParameters<TileK::language_t::e_clause_num_workers>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<TileK::language_t, TileK::language_t::e_clause_num_workers> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  if (parser.consume('[')) {
    parser.skip_whitespace();
    if (!parser.parse<size_t>(clause->parameters.worker_id)) return false;
    parser.skip_whitespace();
    if (!parser.consume(']')) return false;
  }
  else clause->parameters.worker_id = 1;

  if (!parser.consume('('))                                  return false;
  if (!parser.parse<SgExpression *>(clause->parameters.num_workers)) return false;
  if (!parser.consume(')'))                                  return false;

  directive_str = parser.getDirectiveString(); return true;
}
#endif

void lookup_region_successors(
  SgStatement * region,
  Directives::directive_t<TileK::language_t> * directive,
  const std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *> & translation_map
) {
  if (isSgPragmaDeclaration(region)) {
    std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *>::const_iterator it_region = translation_map.find(region);
    assert(it_region != translation_map.end());
    directive->add_successor(TileK::language_t::e_child_scope, it_region->second);
    it_region->second->add_predecessor(TileK::language_t::e_parent_scope, directive);
  }
  else {
    std::queue<SgStatement *> nodes;
    nodes.push(region);
    while (!nodes.empty()) {
      SgStatement * node = nodes.front();
      nodes.pop();

      if (node == NULL) continue;

      switch (node->variantT()) {
        case V_SgDoWhileStmt:
          /// \todo do-while-loop condition: Can it contain directives? It could call functions known to use TileK.
          nodes.push(((SgDoWhileStmt *)node)->get_body());
          break;
        case V_SgForStatement:
          /// \todo for-loop initialization, condition, increment: Can they contain directives? They could call functions known to use TileK.
          nodes.push(((SgForStatement *)node)->get_loop_body());
          break;
        case V_SgIfStmt:
          /// \todo if-stmt condition: Can it contain directives? It could call functions known to use TileK.
          nodes.push(((SgIfStmt *)node)->get_true_body());
          nodes.push(((SgIfStmt *)node)->get_false_body());
          break;
        case V_SgSwitchStatement:
          /// \todo switch-stmt item selector: Can it contain directives? It could call functions known to use TileK.
          nodes.push(((SgSwitchStatement *)node)->get_body());
          break;
        case V_SgWhileStmt:
          /// \todo while-loop condition: Can it contain directives? It could call functions known to use TileK.
          nodes.push(((SgWhileStmt *)node)->get_body());
          break;
        case V_SgBasicBlock:
        {
          SgBasicBlock * bb_stmt = (SgBasicBlock *)node;
          std::vector<SgStatement *>::const_iterator it_stmt;
          for (it_stmt = bb_stmt->get_statements().begin();  it_stmt != bb_stmt->get_statements().end(); it_stmt++) {
            std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *>::const_iterator it_stmt_directive = translation_map.find(*it_stmt);
            if (it_stmt_directive != translation_map.end()) {
              directive->add_successor(TileK::language_t::e_child_scope, it_stmt_directive->second);
              it_stmt_directive->second->add_predecessor(TileK::language_t::e_parent_scope, directive);

              // Exclude statements scoped under data/parallel/kernel/loop constructs
              //   do-loop handles succession of directives scoped one under the other, for example:
              //             sg_scope->{acc_data, acc_parallel, acc_loop, sg_for->{...}, sg_stmt, ... }
              //     -> find acc_data
              //     -> ignore acc_parallel, acc_loop and sg_for
              //     -> restart with sg_stmt
              do {
                if (
                  it_stmt_directive->second->construct->kind == TileK::language_t::e_construct_kernel   ||
                  it_stmt_directive->second->construct->kind == TileK::language_t::e_construct_loop
                ) {
                  it_stmt++;

                  if (it_stmt == bb_stmt->get_statements().end())
                    break;

                  it_stmt_directive = translation_map.find(*it_stmt);
                }
                else break;
              } while (it_stmt != bb_stmt->get_statements().end() && it_stmt_directive != translation_map.end());
            }
            else nodes.push(*it_stmt);
          }
          break;
        }
        case V_SgPragmaDeclaration:
          assert(false); /// \todo Non TileK pragma would not be in 'translation_map' and trigger this assertion
        case V_SgExprStatement:
          break; /// \todo might be a call-site for a function known to use TileK
        case V_SgVariableDeclaration:
          break; /// \todo might be a call-site for a function known to use TileK
        default:
          assert(false);
      }
    }
  }
}

void lookup_loop_successors(
  SgForStatement * for_loop,
  Directives::directive_t<TileK::language_t> * directive,
  const std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *> & translation_map
) {
  assert(for_loop != NULL);

  std::queue<SgStatement *> nodes;
  nodes.push(for_loop->get_loop_body());
  while (!nodes.empty()) {
    SgStatement * node = nodes.front();
    nodes.pop();

    if (node == NULL) continue;

    switch (node->variantT()) {
      case V_SgForStatement:
        /// \todo for-loop initialization, condition, increment: Can they contain directives? They could call functions known to use TileK.
        nodes.push(((SgForStatement *)node)->get_loop_body());
        break;
      case V_SgIfStmt:
        /// \todo if-stmt condition: Can it contain directives? It could call functions known to use TileK.
        nodes.push(((SgIfStmt *)node)->get_true_body());
        nodes.push(((SgIfStmt *)node)->get_false_body());
        break;
      case V_SgBasicBlock:
      {
        SgBasicBlock * bb_stmt = (SgBasicBlock *)node;
        std::vector<SgStatement *>::const_iterator it_stmt;
        for (it_stmt = bb_stmt->get_statements().begin();  it_stmt != bb_stmt->get_statements().end(); it_stmt++) {
          std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *>::const_iterator it_stmt_directive = translation_map.find(*it_stmt);
          if (it_stmt_directive != translation_map.end()) {
            assert(it_stmt_directive->second->construct->kind == TileK::language_t::e_construct_loop);
            directive->add_successor(TileK::language_t::e_child_scope, it_stmt_directive->second);
            it_stmt_directive->second->add_predecessor(TileK::language_t::e_parent_scope, directive);
            it_stmt++;
          }
          else nodes.push(*it_stmt);
        }
        break;
      }
      case V_SgExprStatement:
        break; /// \todo might be a call-site for a user-defined function that has to be inlined 
      case V_SgVariableDeclaration:
        break; /// \todo might be a call-site for a user-defined function that has to be inlined 
      case V_SgPragmaDeclaration:
        assert(false); /// \todo Non TileK pragma would not be in 'translation_map' and trigger this assertion
      case V_SgDoWhileStmt:
      case V_SgSwitchStatement:
      case V_SgWhileStmt:
        assert(false); /// \todo not supported inside parallel/kernel region => cannot be scoped under loop construct.
      default:
        assert(false);
    }
  }
}

template <>
bool Frontend<TileK::language_t>::build_graph(
  const std::map<SgLocatedNode *, Directives::directive_t<TileK::language_t> *> & translation_map
) {
  std::vector<Directives::directive_t<TileK::language_t> *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    switch ((*it_directive)->construct->kind) {
      case TileK::language_t::e_construct_kernel:
      {
        Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_kernel> * construct =
                 (Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_kernel> *)((*it_directive)->construct);
        lookup_region_successors(construct->assoc_nodes.kernel_region, *it_directive, translation_map);
        break;
      }
      case TileK::language_t::e_construct_loop:
      {
        Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_loop> * construct =
                 (Directives::construct_t<TileK::language_t, TileK::language_t::e_construct_loop> *)((*it_directive)->construct);
        lookup_loop_successors(construct->assoc_nodes.for_loop, *it_directive, translation_map);
        break;
      }
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

/** @} */

}

}

