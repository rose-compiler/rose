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
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_data> * construct
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.data_scope = isSgScopeStatement(SageInterface::getNextStatement(pragma_decl));
  assert(construct->assoc_nodes.data_scope != NULL);

  return true;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_parallel>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_parallel> * construct
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.parallel_scope = isSgScopeStatement(SageInterface::getNextStatement(pragma_decl));
  assert(construct->assoc_nodes.parallel_scope != NULL);

  return true;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_kernel>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_kernel> * construct
) {
  SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(pragma_decl != NULL);

  construct->assoc_nodes.parent_scope = isSgScopeStatement(pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.kernel_scope = isSgScopeStatement(SageInterface::getNextStatement(pragma_decl));
  assert(construct->assoc_nodes.kernel_scope != NULL);

  return true;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_loop>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_loop> * construct
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
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_host_data> * construct
) {
  return false;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_declare>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_declare> * construct
) {
  return false;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_cache>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_cache> * construct
) {
  return false;
}

template <> 
template <> 
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_update>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_update> * construct
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_blank>(
  SgLocatedNode * directive_node,
  Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_blank> * construct
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
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_vector_length>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector_length> * clause
) {
  return false;
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
  std::cout << "copy    start: " << directive_str << std::endl;
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  std::cout << "copy    stop : " << directive_str << std::endl;
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_copyin>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyin> * clause
) {
  std::cout << "copyin  start: " << directive_str << std::endl;
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  std::cout << "copyin  stop : " << directive_str << std::endl;
  return res;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_copyout>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyout> * clause
) {
  std::cout << "copyout start: " << directive_str << std::endl;
  DLX::Frontend::Parser parser(directive_str, directive_node);
  bool res = parser.parse_list(clause->parameters.data_sections, '(', ')', ',');
  if (res) directive_str = parser.getDirectiveString();
  std::cout << "copyout stop : " << directive_str << std::endl;
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
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_gang>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_gang> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_worker>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_worker> * clause
) {
  return false;
}

template <>
template <>
bool Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_vector>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector> * clause
) {
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

template <>
bool Frontend<OpenACC::language_t>::build_graph() {
  // TODO Frontend<OpenACC::language_t>::build_graph()
  return false;
}

/** @} */

}

}

