/*!
 * 
 * \file lib/openacc/frontend.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "DLX/Core/frontend.hpp"
#include "DLX/Core/directives.hpp"
#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/OpenACC/language.hpp"

#include <iostream>

#include <cassert>

#include "rose.h"

#ifndef OPENACC_MULTIDEV
# define OPENACC_MULTIDEV 1
#endif
#ifdef OPENACC_DATA_ACCESS
# define OPENACC_DATA_ACCESS 1
#endif

namespace DLX {

namespace Frontend {

/*!
 * \addtogroup grp_dlx_openacc_frontend
 * @{
 */

template <>
template <>
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_data>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_data> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.data_region = SageInterface::getNextStatement(pragma_decl);
  assert(construct->assoc_nodes.data_region != NULL);

  return true;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_parallel>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_parallel> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.parallel_region = SageInterface::getNextStatement(pragma_decl);
  assert(construct->assoc_nodes.parallel_region != NULL);

  return true;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_kernel>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_kernel> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.kernel_region = SageInterface::getNextStatement(pragma_decl);
  assert(construct->assoc_nodes.kernel_region != NULL);

  return true;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_loop>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_loop> * construct,
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
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_host_data>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_host_data> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  return false;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_declare>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_declare> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  return false;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_cache>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_cache> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  return false;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_update>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_update> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_blank>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_blank> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_if>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_if> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse<SgExpression *>(clause->parameters.condition);
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_async>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_async> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse<SgExpression *>(clause->parameters.sync_tag);
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_num_gangs>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_gangs> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  if (!parser.parse_singleton<size_t>(clause->parameters.lvl, '[', ']'))
    clause->parameters.lvl = 0;
  assert(clause->parameters.lvl >= 0 && clause->parameters.lvl <= 2);
  bool res = parser.parse_singleton<SgExpression *>(clause->parameters.exp, '(', ')');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_num_workers>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_workers> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  if (!parser.parse_singleton<size_t>(clause->parameters.lvl, '[', ']'))
    clause->parameters.lvl = 0;
  assert(clause->parameters.lvl >= 0 && clause->parameters.lvl <= 2);
  bool res = parser.parse_singleton<SgExpression *>(clause->parameters.exp, '(', ')');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_vector_length>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector_length> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_singleton<SgExpression *>(clause->parameters.exp, '(', ')');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_reduction>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_reduction> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_copy>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copy> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_copyin>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyin> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_copyout>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyout> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_create>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_create> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present_or_copy>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copy> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present_or_copyin>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyin> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present_or_copyout>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyout> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present_or_create>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_create> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_deviceptr>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_deviceptr> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_private>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_private> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_firstprivate>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_firstprivate> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_use_device>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_use_device> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_device_resident>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_device_resident> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_collapse>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_collapse> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_auto>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_auto> * clause
) {
  assert(directive_str.size() == 0 || directive_str[0] != '(');
  return true;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_gang>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_gang> * clause
) {
  assert(directive_str.size() == 0 || directive_str[0] != '(');

  clause->parameters.dimension_id = -1;
  if (directive_str[0] == '[') {
    /// \todo read number in dimension_id, find matching ']'
  }
  else clause->parameters.dimension_id = 0;

  assert(clause->parameters.dimension_id != (size_t)-1);

  return true;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_worker>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_worker> * clause
) {
  assert(directive_str.size() == 0 || directive_str[0] != '(');

  clause->parameters.dimension_id = -1;
  if (directive_str[0] == '[') {
    /// \todo read number in dimension_id, find matching ']'
  }
  else clause->parameters.dimension_id = 0;

  assert(clause->parameters.dimension_id != (size_t)-1);

  return true;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_vector>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector> * clause
) {
  assert(directive_str.size() == 0 || directive_str[0] != '(');
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_seq>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_seq> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_independent>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_independent> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_host>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_host> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_device>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_device> * clause
) {
  return false;
}

#if OPENACC_MULTIDEV

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_split>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_split> * clause
) {
  /// \todo
  assert(false);
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_devices>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_devices> * clause
) {
  /// \todo
  assert(false);
  return false;
}

#endif

#if OPENACC_DATA_ACCESS

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_read>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_read> * clause
) {
  /// \todo
  assert(false);
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_write>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_write> * clause
) {
  /// \todo
  assert(false);
  return false;
}

#endif

void lookup_region_successors(
  SgStatement * region,
  Directives::directive_t<OpenACC::language_t> * directive,
  const std::map<SgLocatedNode *, Directives::directive_t<OpenACC::language_t> *> & translation_map
) {
  if (isSgPragmaDeclaration(region)) {
    std::map<SgLocatedNode *, Directives::directive_t<OpenACC::language_t> *>::const_iterator it_region = translation_map.find(region);
    assert(it_region != translation_map.end());
    directive->add_successor(OpenACC::language_t::e_child_scope, it_region->second);
    it_region->second->add_predecessor(OpenACC::language_t::e_parent_scope, directive);
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
          /// \todo do-while-loop condition: Can it contain directives? It could call functions known to use OpenACC.
          nodes.push(((SgDoWhileStmt *)node)->get_body());
          break;
        case V_SgForStatement:
          /// \todo for-loop initialization, condition, increment: Can they contain directives? They could call functions known to use OpenACC.
          nodes.push(((SgForStatement *)node)->get_loop_body());
          break;
        case V_SgIfStmt:
          /// \todo if-stmt condition: Can it contain directives? It could call functions known to use OpenACC.
          nodes.push(((SgIfStmt *)node)->get_true_body());
          nodes.push(((SgIfStmt *)node)->get_false_body());
          break;
        case V_SgSwitchStatement:
          /// \todo switch-stmt item selector: Can it contain directives? It could call functions known to use OpenACC.
          nodes.push(((SgSwitchStatement *)node)->get_body());
          break;
        case V_SgWhileStmt:
          /// \todo while-loop condition: Can it contain directives? It could call functions known to use OpenACC.
          nodes.push(((SgWhileStmt *)node)->get_body());
          break;
        case V_SgBasicBlock:
        {
          SgBasicBlock * bb_stmt = (SgBasicBlock *)node;
          std::vector<SgStatement *>::const_iterator it_stmt;
          for (it_stmt = bb_stmt->get_statements().begin();  it_stmt != bb_stmt->get_statements().end(); it_stmt++) {
            std::map<SgLocatedNode *, Directives::directive_t<OpenACC::language_t> *>::const_iterator it_stmt_directive = translation_map.find(*it_stmt);
            if (it_stmt_directive != translation_map.end()) {
              directive->add_successor(OpenACC::language_t::e_child_scope, it_stmt_directive->second);
              it_stmt_directive->second->add_predecessor(OpenACC::language_t::e_parent_scope, directive);

              // Exclude statements scoped under data/parallel/kernel/loop constructs
              //   do-loop handles succession of directives scoped one under the other, for example:
              //             sg_scope->{acc_data, acc_parallel, acc_loop, sg_for->{...}, sg_stmt, ... }
              //     -> find acc_data
              //     -> ignore acc_parallel, acc_loop and sg_for
              //     -> restart with sg_stmt
              do {
                if (
                  it_stmt_directive->second->construct->kind == OpenACC::language_t::e_acc_construct_data     ||
                  it_stmt_directive->second->construct->kind == OpenACC::language_t::e_acc_construct_parallel ||
                  it_stmt_directive->second->construct->kind == OpenACC::language_t::e_acc_construct_kernel   ||
                  it_stmt_directive->second->construct->kind == OpenACC::language_t::e_acc_construct_loop
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
          assert(false); /// \todo Non OpenACC pragma would not be in 'translation_map' and trigger this assertion
        case V_SgExprStatement:
          break; /// \todo might be a call-site for a function known to use OpenACC
        default:
          assert(false);
      }
    }
  }
}

void lookup_loop_successors(
  SgForStatement * for_loop,
  Directives::directive_t<OpenACC::language_t> * directive,
  const std::map<SgLocatedNode *, Directives::directive_t<OpenACC::language_t> *> & translation_map
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
        /// \todo for-loop initialization, condition, increment: Can they contain directives? They could call functions known to use OpenACC.
        nodes.push(((SgForStatement *)node)->get_loop_body());
        break;
      case V_SgIfStmt:
        /// \todo if-stmt condition: Can it contain directives? It could call functions known to use OpenACC.
        nodes.push(((SgIfStmt *)node)->get_true_body());
        nodes.push(((SgIfStmt *)node)->get_false_body());
        break;
      case V_SgBasicBlock:
      {
        SgBasicBlock * bb_stmt = (SgBasicBlock *)node;
        std::vector<SgStatement *>::const_iterator it_stmt;
        for (it_stmt = bb_stmt->get_statements().begin();  it_stmt != bb_stmt->get_statements().end(); it_stmt++) {
          std::map<SgLocatedNode *, Directives::directive_t<OpenACC::language_t> *>::const_iterator it_stmt_directive = translation_map.find(*it_stmt);
          if (it_stmt_directive != translation_map.end()) {
            assert(it_stmt_directive->second->construct->kind == OpenACC::language_t::e_acc_construct_loop);
            directive->add_successor(OpenACC::language_t::e_child_scope, it_stmt_directive->second);
            it_stmt_directive->second->add_predecessor(OpenACC::language_t::e_parent_scope, directive);
            it_stmt++;
          }
          else nodes.push(*it_stmt);
        }
        break;
      }
      case V_SgExprStatement:
        break; /// \todo might be a call-site for a user-defined function that has to be inlined 
      case V_SgPragmaDeclaration:
        assert(false); /// \todo Non OpenACC pragma would not be in 'translation_map' and trigger this assertion
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
bool Frontend<OpenACC::language_t>::build_graph(const std::map<SgLocatedNode *, directive_t *> & translation_map) {
  std::vector<Directives::directive_t<OpenACC::language_t> *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    switch ((*it_directive)->construct->kind) {
      case OpenACC::language_t::e_acc_construct_data:
      {
        Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_data> * construct =
                 (Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_data> *)((*it_directive)->construct);
        lookup_region_successors(construct->assoc_nodes.data_region, *it_directive, translation_map);
        break;
      }
      case OpenACC::language_t::e_acc_construct_parallel:
      {
        Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_parallel> * construct =
                 (Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_parallel> *)((*it_directive)->construct);
        lookup_region_successors(construct->assoc_nodes.parallel_region, *it_directive, translation_map);
        break;
      }
      case OpenACC::language_t::e_acc_construct_kernel:
      {
        Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_kernel> * construct =
                 (Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_kernel> *)((*it_directive)->construct);
        lookup_region_successors(construct->assoc_nodes.kernel_region, *it_directive, translation_map);
        break;
      }
      case OpenACC::language_t::e_acc_construct_loop:
      {
        Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_loop> * construct =
                 (Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_loop> *)((*it_directive)->construct);
        lookup_loop_successors(construct->assoc_nodes.for_loop, *it_directive, translation_map);
        break;
      }
      case OpenACC::language_t::e_acc_construct_host_data:
      case OpenACC::language_t::e_acc_construct_declare:
      case OpenACC::language_t::e_acc_construct_cache:
      case OpenACC::language_t::e_acc_construct_update:
      case OpenACC::language_t::e_acc_construct_blank:
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

