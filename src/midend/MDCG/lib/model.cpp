/** 
 * \file MDCG/lib/model.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MDCG/model.hpp"
#include "MDCG/model-base.hpp"
#include "MDCG/model-namespace.hpp"
#include "MDCG/model-variable.hpp"
#include "MDCG/model-function.hpp"
#include "MDCG/model-type.hpp"
#include "MDCG/model-class.hpp"
#include "MDCG/model-field.hpp"
#include "MDCG/model-method.hpp"

#include "rose.h"

#include <cassert>

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

model_t::model_t() :
  variables(),
  functions(),
  types(),
  classes(),
  fields(),
  methods(),
  namespaces()
{}

model_t::model_t(const model_t & model) :
  variables(),
  functions(),
  types(),
  classes(),
  fields(),
  methods(),
  namespaces()
{
  /// \todo copy
}

variable_t model_t::lookup_variable_by(SgVariableSymbol * symbol) const {
  std::vector<variable_t>::const_iterator it;
  for (it = variables.begin(); it != variables.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

function_t model_t::lookup_function_by(SgFunctionSymbol * symbol) const {
  std::vector<function_t>::const_iterator it;
  for (it = functions.begin(); it != functions.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

type_t model_t::lookup_type_by(SgType * type) const {
  std::vector<type_t>::const_iterator it;
  for (it = types.begin(); it != types.end(); it++)
    if ((*it)->node->type == type)
      return *it;

  return NULL;
}

class_t model_t::lookup_class_by(SgClassSymbol * symbol) const {
  std::vector<class_t>::const_iterator it;
  for (it = classes.begin(); it != classes.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

field_t model_t::lookup_field_by(SgVariableSymbol * symbol) const {
  std::vector<field_t>::const_iterator it;
  for (it = fields.begin(); it != fields.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

method_t model_t::lookup_method_by(SgMemberFunctionSymbol * symbol) const {
  std::vector<method_t>::const_iterator it;
  for (it = methods.begin(); it != methods.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

namespace_t model_t::lookup_namespace_by(SgNamespaceSymbol * symbol) const {
  std::vector<namespace_t>::const_iterator it;
  for (it = namespaces.begin(); it != namespaces.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

template <Model::model_elements_e kind>
void toDotNode(std::ostream & out, Model::element_t<kind> * element, std::set<void *> & processed_nodes);

template <>
void toDotNode<Model::e_model_variable>(std::ostream & out, Model::variable_t element, std::set<void *> & processed_nodes);

template <>
void toDotNode<Model::e_model_field>(std::ostream & out, Model::field_t element, std::set<void *> & processed_nodes);

template <>
void toDotNode<Model::e_model_function>(std::ostream & out, Model::function_t element, std::set<void *> & processed_nodes);

template <>
void toDotNode<Model::e_model_method>(std::ostream & out, Model::method_t element, std::set<void *> & processed_nodes);

template <>
void toDotNode<Model::e_model_type>(std::ostream & out, Model::type_t element, std::set<void *> & processed_nodes);

template <>
void toDotNode<Model::e_model_class>(std::ostream & out, Model::class_t element, std::set<void *> & processed_nodes);

template <>
void toDotNode<Model::e_model_namespace>(std::ostream & out, Model::namespace_t element, std::set<void *> & processed_nodes);

template <>
void toDotNode<Model::e_model_variable>(std::ostream & out, Model::variable_t element, std::set<void *> & processed_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=octagon]" << std::endl;
  
  if (element->node->type != NULL) {
    toDotNode<Model::e_model_type>(out, element->node->type, processed_nodes);
    out << "element_" << element << " -> element_" << element->node->type << " [label=\"type\", color=\"blue\"]" << std::endl;
  }
}

template <>
void toDotNode<Model::e_model_field>(std::ostream & out, Model::field_t element, std::set<void *> & processed_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=doubleoctagon]" << std::endl;
  
  if (element->node->type != NULL) {
    toDotNode<Model::e_model_type>(out, element->node->type, processed_nodes);
    out << "element_" << element << " -> element_" << element->node->type << " [label=\"type\", color=\"blue\"]" << std::endl;
  }
}

template <>
void toDotNode<Model::e_model_function>(std::ostream & out, Model::function_t element, std::set<void *> & processed_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=hexagon]" << std::endl;
  
  if (element->node->return_type != NULL) {
    toDotNode<Model::e_model_type>(out, element->node->return_type, processed_nodes);
    out << "element_" << element << " -> element_" << element->node->return_type << " [label=\"return_type\", color=\"blue\"]" << std::endl;
  }

  std::vector<Model::type_t>::const_iterator it;
  unsigned cnt = 0;
  for (it = element->node->args_types.begin(); it != element->node->args_types.end(); it++) {
    toDotNode<Model::e_model_type>(out, *it, processed_nodes);
    out << "element_" << element << " -> element_" << *it << " [label=\"type argument[" << cnt++ << "]\", color=\"blue\"]" << std::endl;
  }
}

template <>
void toDotNode<Model::e_model_method>(std::ostream & out, Model::method_t element, std::set<void *> & processed_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=hexagon]" << std::endl;
  
  if (element->node->return_type != NULL) {
    toDotNode<Model::e_model_type>(out, element->node->return_type, processed_nodes);
    out << "element_" << element << " -> element_" << element->node->return_type << " [label=\"return_type\"], color=\"blue\"" << std::endl;
  }

  std::vector<Model::type_t>::const_iterator it;
  unsigned cnt = 0;
  for (it = element->node->args_types.begin(); it != element->node->args_types.end(); it++) {
    toDotNode<Model::e_model_type>(out, *it, processed_nodes);
    out << "element_" << element << " -> element_" << *it << " [label=\"type argument[" << cnt++ << "]\", color=\"blue\"]" << std::endl;
  }
}

template <>
void toDotNode<Model::e_model_type>(std::ostream & out, Model::type_t element, std::set<void *> & processed_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  switch (element->node->kind) {
    case Model::node_t<Model::e_model_type>::e_array_type:
      out << "element_" << element << " [label=\"Array\", shape=oval]" << std::endl;
      if (element->node->base_type != NULL) {
        toDotNode<Model::e_model_type>(out, element->node->base_type, processed_nodes);
        out << "element_" << element << " -> element_" << element->node->base_type << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_pointer_type:
      out << "element_" << element << " [label=\"Pointer\", shape=oval]" << std::endl;
      if (element->node->base_type != NULL) {
        toDotNode<Model::e_model_type>(out, element->node->base_type, processed_nodes);
        out << "element_" << element << " -> element_" << element->node->base_type << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_reference_type:
      out << "element_" << element << " [label=\"Reference\", shape=oval]" << std::endl;
      if (element->node->base_type != NULL) {
        toDotNode<Model::e_model_type>(out, element->node->base_type, processed_nodes);
        out << "element_" << element << " -> element_" << element->node->base_type << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_typedef_type:
      out << "element_" << element << " [label=\"Typedef\", shape=oval]" << std::endl;
      if (element->node->base_type != NULL) {
        toDotNode<Model::e_model_type>(out, element->node->base_type, processed_nodes);
        out << "element_" << element << " -> element_" << element->node->base_type << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_class_type:
      out << "element_" << element << " [label=\"Class\", shape=oval]" << std::endl;
      if (element->node->base_class != NULL) {
        toDotNode<Model::e_model_class>(out, element->node->base_class, processed_nodes);
        out << "element_" << element << " -> element_" << element->node->base_class << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_enum_type:
      out << "element_" << element << " [label=\"Enum\", shape=oval]" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_native_type:
      out << "element_" << element << " [label=\"" << element->node->type->unparseToString() << "\", shape=oval]" << std::endl;
      break;
    default:
      assert(false);
  }
}

template <>
void toDotNode<Model::e_model_class>(std::ostream & out, Model::class_t element, std::set<void *> & processed_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=box]" << std::endl;

  unsigned cnt = 0;
  std::vector<Model::type_t>::const_iterator it_type;
  for (it_type = element->scope->type_children.begin(); it_type != element->scope->type_children.end(); it_type++) {
    toDotNode<Model::e_model_type>(out, *it_type, processed_nodes);
    out << "element_" << element << " -> element_" << *it_type << " [label=\"[" << cnt++ << "]\", color=\"red:green\"]" << std::endl;
  }

  cnt = 0;
  std::vector<Model::class_t>::const_iterator it_class;
  for (it_class = element->scope->class_children.begin(); it_class != element->scope->class_children.end(); it_class++) {
    toDotNode<Model::e_model_class>(out, *it_class, processed_nodes);
    out << "element_" << element << " -> element_" << *it_class << " [label=\"[" << cnt++ << "]\", color=\"red:orange\"]" << std::endl;
  }

  cnt = 0;
  std::vector<Model::field_t>::const_iterator it_field;
  for (it_field = element->scope->field_children.begin(); it_field != element->scope->field_children.end(); it_field++) {
    toDotNode<Model::e_model_field>(out, *it_field, processed_nodes);
    out << "element_" << element << " -> element_" << *it_field << " [label=\"[" << cnt++ << "]\", color=\"red:red\"]" << std::endl;
  }

  cnt = 0;
  std::vector<Model::method_t>::const_iterator it_method;
  for (it_method = element->scope->method_children.begin(); it_method != element->scope->method_children.end(); it_method++) {
    toDotNode<Model::e_model_method>(out, *it_method, processed_nodes);
    out << "element_" << element << " -> element_" << *it_method << " [label=\"[" << cnt++ << "]\", color=\"red:blue\"]" << std::endl;
  }
}

template <>
void toDotNode<Model::e_model_namespace>(std::ostream & out, Model::namespace_t element, std::set<void *> & processed_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\" shape=component]" << std::endl;

  /// \todo
}

void model_t::toDot(std::ostream & out, std::string prefix_filter, bool variable_root, bool function_root, bool class_root) const {
  out << "digraph {" << std::endl;

  std::set<void *> processed_nodes;

  if (variable_root) {
    std::vector<Model::variable_t>::const_iterator it_variable;
    for (it_variable = variables.begin(); it_variable != variables.end(); it_variable++)
      if ((*it_variable)->node->symbol->get_name().getString().find(prefix_filter) != std::string::npos)
        toDotNode<Model::e_model_variable>(out, *it_variable, processed_nodes);
  }

  if (function_root) {
    std::vector<Model::function_t>::const_iterator it_function;
    for (it_function = functions.begin(); it_function != functions.end(); it_function++)
      if ((*it_function)->node->symbol->get_name().getString().find(prefix_filter) != std::string::npos)
        toDotNode<Model::e_model_function>(out, *it_function, processed_nodes);
  }
/*
  std::vector<Model::field_t>::const_iterator it_field;
  for (it_field = fields.begin(); it_field != fields.end(); it_field++)
    toDotNode<Model::e_model_field>(out, *it_field);

  std::vector<Model::method_t>::const_iterator it_method;
  for (it_method = methods.begin(); it_method != methods.end(); it_method++)
    toDotNode<Model::e_model_method>(out, *it_method);

  std::vector<Model::type_t>::const_iterator it_type;
  for (it_type = types.begin(); it_type != types.end(); it_type++)
    toDotNode<Model::e_model_type>(out, *it_type);
*/

  if (class_root) {
    std::vector<Model::class_t>::const_iterator it_class;
    for (it_class = classes.begin(); it_class != classes.end(); it_class++)
      if ((*it_class)->node->symbol->get_name().getString().find(prefix_filter) != std::string::npos)
        toDotNode<Model::e_model_class>(out, *it_class, processed_nodes);
  }
/*
  std::vector<Model::namespace_t>::const_iterator it_namespace;
  for (it_namespace = namespaces.begin(); it_namespace != namespaces.end(); it_namespace++)
    toDotNode<Model::e_model_namespace>(out, *it_namespace);
*/
  out << "}" << std::endl;
}

/** @} */

}

}

