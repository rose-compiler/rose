/** 
 * \file MDCG/lib/model.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MDCG/Core/model.hpp"
#include "MDCG/Core/model-base.hpp"
#include "MDCG/Core/model-namespace.hpp"
#include "MDCG/Core/model-variable.hpp"
#include "MDCG/Core/model-function.hpp"
#include "MDCG/Core/model-type.hpp"
#include "MDCG/Core/model-class.hpp"
#include "MDCG/Core/model-field.hpp"
#include "MDCG/Core/model-method.hpp"

#include "sage3basic.h"

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
  std::map<void*, void*> copies_map;

  std::vector<variable_t>::const_iterator it_var;
  for (it_var = model.variables.begin(); it_var != model.variables.end(); it_var++)
    variables.push_back(copy(*it_var, copies_map));

  std::vector<function_t>::const_iterator it_func;
  for (it_func = model.functions.begin(); it_func != model.functions.end(); it_func++)
    functions.push_back(copy(*it_func, copies_map));

  std::vector<type_t>::const_iterator it_type;
  for (it_type = model.types.begin(); it_type != model.types.end(); it_type++)
    types.push_back(copy(*it_type, copies_map));

  std::vector<class_t>::const_iterator it_class;
  for (it_class = model.classes.begin(); it_class != model.classes.end(); it_class++)
    classes.push_back(copy(*it_class, copies_map));

  std::vector<field_t>::const_iterator it_field;
  for (it_field = model.fields.begin(); it_field != model.fields.end(); it_field++)
    fields.push_back(copy(*it_field, copies_map));

  std::vector<method_t>::const_iterator it_method;
  for (it_method = model.methods.begin(); it_method != model.methods.end(); it_method++)
    methods.push_back(copy(*it_method, copies_map));

  std::vector<namespace_t>::const_iterator it_nspc;
  for (it_nspc = model.namespaces.begin(); it_nspc != model.namespaces.end(); it_nspc++)
    namespaces.push_back(copy(*it_nspc, copies_map));
}

model_t::~model_t() {
  std::vector<variable_t>::const_iterator it_var;
  for (it_var = variables.begin(); it_var != variables.end(); it_var++)
    delete *it_var;

  std::vector<function_t>::const_iterator it_func;
  for (it_func = functions.begin(); it_func != functions.end(); it_func++)
    delete *it_func;

  std::vector<type_t>::const_iterator it_type;
  for (it_type = types.begin(); it_type != types.end(); it_type++)
    delete *it_type;

  std::vector<class_t>::const_iterator it_class;
  for (it_class = classes.begin(); it_class != classes.end(); it_class++)
    delete *it_class;

  std::vector<field_t>::const_iterator it_field;
  for (it_field = fields.begin(); it_field != fields.end(); it_field++)
    delete *it_field;

  std::vector<method_t>::const_iterator it_method;
  for (it_method = methods.begin(); it_method != methods.end(); it_method++)
    delete *it_method;

  std::vector<namespace_t>::const_iterator it_nspc;
  for (it_nspc = namespaces.begin(); it_nspc != namespaces.end(); it_nspc++)
    delete *it_nspc;
}

variable_t model_t::lookup_variable(SgVariableSymbol * symbol) const {
  std::vector<variable_t>::const_iterator it;
  for (it = variables.begin(); it != variables.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

function_t model_t::lookup_function(SgFunctionSymbol * symbol) const {
  std::vector<function_t>::const_iterator it;
  for (it = functions.begin(); it != functions.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

type_t model_t::lookup_type(SgType * type) const {
  SgModifierType * modifier_type = isSgModifierType(type);
  if (modifier_type != NULL)
    return lookup_type(modifier_type->get_base_type());

  std::vector<type_t>::const_iterator it;
  for (it = types.begin(); it != types.end(); it++)
    if ((*it)->node->type == type)
      return *it;

  return NULL;
}

class_t model_t::lookup_class(SgClassSymbol * symbol) const {
  std::vector<class_t>::const_iterator it;
  for (it = classes.begin(); it != classes.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

field_t model_t::lookup_field(SgVariableSymbol * symbol) const {
  std::vector<field_t>::const_iterator it;
  for (it = fields.begin(); it != fields.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

method_t model_t::lookup_method(SgMemberFunctionSymbol * symbol) const {
  std::vector<method_t>::const_iterator it;
  for (it = methods.begin(); it != methods.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

namespace_t model_t::lookup_namespace(SgNamespaceSymbol * symbol) const {
  std::vector<namespace_t>::const_iterator it;
  for (it = namespaces.begin(); it != namespaces.end(); it++)
    if ((*it)->node->symbol == symbol)
      return *it;

  return NULL;
}

template <>
void model_t::lookup<Model::variable_t>(const std::string & name, std::set<Model::variable_t> & result) const {
  std::vector<Model::variable_t>::const_iterator it_variable;
  for (it_variable = variables.begin(); it_variable != variables.end(); it_variable++)
    if ((*it_variable)->node->symbol->get_name().getString() == name)
      result.insert(*it_variable);
}

template <>
void model_t::lookup<Model::field_t>(const std::string & name, std::set<Model::field_t> & result) const {
  std::vector<Model::field_t>::const_iterator it_field;
  for (it_field = fields.begin(); it_field != fields.end(); it_field++)
    if ((*it_field)->node->symbol->get_name().getString() == name)
      result.insert(*it_field);
}

template <>
void model_t::lookup<Model::function_t>(const std::string & name, std::set<Model::function_t> & result) const {
  std::vector<Model::function_t>::const_iterator it_function;
  for (it_function = functions.begin(); it_function != functions.end(); it_function++)
    if ((*it_function)->node->symbol->get_name().getString() == name)
      result.insert(*it_function);
}

template <>
void model_t::lookup<Model::method_t>(const std::string & name, std::set<Model::method_t> & result) const {
  std::vector<Model::method_t>::const_iterator it_method;
  for (it_method = methods.begin(); it_method != methods.end(); it_method++)
    if ((*it_method)->node->symbol->get_name().getString() == name)
      result.insert(*it_method);
}

template <>
void model_t::lookup<Model::class_t>(const std::string & name, std::set<Model::class_t> & result) const {
  std::vector<Model::class_t>::const_iterator it_class;
  for (it_class = classes.begin(); it_class != classes.end(); it_class++)
    if ((*it_class)->node->symbol->get_name().getString() == name)
      result.insert(*it_class);
}

template <>
void model_t::lookup<Model::namespace_t>(const std::string & name, std::set<Model::namespace_t> & result) const {
  std::vector<Model::namespace_t>::const_iterator it_namespace;
  for (it_namespace = namespaces.begin(); it_namespace != namespaces.end(); it_namespace++)
    if ((*it_namespace)->node->symbol->get_name().getString() == name)
      result.insert(*it_namespace);
}

template <>
void model_t::lookup<Model::type_t>(const std::string & name, std::set<Model::type_t> & result) const {
  std::vector<Model::type_t>::const_iterator it_type;
  for (it_type = types.begin(); it_type != types.end(); it_type++)
    if ((*it_type)->node->kind == node_t<e_model_type>::e_typedef_type &&
        (*it_type)->node->typedef_symbol != NULL &&
        (*it_type)->node->typedef_symbol->get_declaration() != NULL &&
        (*it_type)->node->typedef_symbol->get_declaration()->get_name() == name)
      result.insert(*it_type);
}

template <Model::model_elements_e kind>
void toDotNode(std::ostream & out, Model::element_t<kind> * element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes);

template <>
void toDotNode<Model::e_model_variable>(std::ostream & out, Model::variable_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes);

template <>
void toDotNode<Model::e_model_field>(std::ostream & out, Model::field_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes);

template <>
void toDotNode<Model::e_model_function>(std::ostream & out, Model::function_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes);

template <>
void toDotNode<Model::e_model_method>(std::ostream & out, Model::method_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes);

template <>
void toDotNode<Model::e_model_type>(std::ostream & out, Model::type_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes);

template <>
void toDotNode<Model::e_model_class>(std::ostream & out, Model::class_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes);

template <>
void toDotNode<Model::e_model_namespace>(std::ostream & out, Model::namespace_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes);

template <Model::model_elements_e kind>
void toDotLabelNode(std::ostream & out, Model::element_t<kind> * element, std::set<void *> & described_nodes);

template <Model::model_elements_e kind>
void toTextNode(std::ostream & out, Model::element_t<kind> * element);

template <>
void toDotLabelNode<Model::e_model_variable>(std::ostream & out, Model::variable_t element, std::set<void *> & described_nodes) {
  if (described_nodes.find(element) != described_nodes.end()) return;
  described_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=octagon]" << std::endl;
}

template <>
void toTextNode<Model::e_model_variable>(std::ostream & out, Model::element_t<Model::e_model_variable> * element) {
  out << "variable:" << element << " => \"" << element->node->symbol->get_name().str() << "\"" << std::endl;
}

template <>
void toDotNode<Model::e_model_variable>(std::ostream & out, Model::variable_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  toDotLabelNode<Model::e_model_variable>(out, element, described_nodes);
  
  if (element->node->type != NULL) {
    toDotNode<Model::e_model_type>(out, element->node->type, processed_nodes, described_nodes);
    out << "element_" << element << " -> element_" << element->node->type << " [label=\"type\", color=\"blue\"]" << std::endl;
  }
}

template <>
void toDotLabelNode<Model::e_model_field>(std::ostream & out, Model::field_t element, std::set<void *> & described_nodes) {
  if (described_nodes.find(element) != described_nodes.end()) return;
  described_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=doubleoctagon]" << std::endl;
}

template <>
void toTextNode<Model::e_model_field>(std::ostream & out, Model::element_t<Model::e_model_field> * element) {
  out << "field:" << element << " => \"" << element->node->symbol->get_name().str() << "\"" << std::endl;
}

template <>
void toDotNode<Model::e_model_field>(std::ostream & out, Model::field_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  toDotLabelNode<Model::e_model_field>(out, element, described_nodes);
  
  if (element->node->type != NULL) {
    toDotNode<Model::e_model_type>(out, element->node->type, processed_nodes, described_nodes);
    out << "element_" << element << " -> element_" << element->node->type << " [label=\"type\", color=\"blue\"]" << std::endl;
  }
}

template <>
void toDotLabelNode<Model::e_model_function>(std::ostream & out, Model::function_t element, std::set<void *> & described_nodes) {
  if (described_nodes.find(element) != described_nodes.end()) return;
  described_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=hexagon]" << std::endl;
}

template <>
void toTextNode<Model::e_model_function>(std::ostream & out, Model::element_t<Model::e_model_function> * element) {
  out << "function:" << element << " => \"" << element->node->symbol->get_name().str() << "\"" << std::endl;
}

template <>
void toDotNode<Model::e_model_function>(std::ostream & out, Model::function_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  toDotLabelNode<Model::e_model_function>(out, element, described_nodes);

  std::vector<Model::type_t>::const_iterator it;
  size_t cnt = 0;
  for (it = element->node->args_types.begin(); it != element->node->args_types.end(); it++) {
    toDotNode<Model::e_model_type>(out, *it, processed_nodes, described_nodes);
    out << "element_" << element << " -> element_" << *it << " [label=\"type argument[" << cnt++ << "]\", color=\"blue\"]" << std::endl;
  }
}

template <>
void toDotLabelNode<Model::e_model_method>(std::ostream & out, Model::method_t element, std::set<void *> & described_nodes) {
  if (described_nodes.find(element) != described_nodes.end()) return;
  described_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=hexagon]" << std::endl;
}

template <>
void toTextNode<Model::e_model_method>(std::ostream & out, Model::element_t<Model::e_model_method> * element) {
  out << "method:" << element << " => \"" << element->node->symbol->get_name().str() << "\"" << std::endl;
}

template <>
void toDotNode<Model::e_model_method>(std::ostream & out, Model::method_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  toDotLabelNode<Model::e_model_method>(out, element, described_nodes);
  
  if (element->node->return_type != NULL) {
    toDotNode<Model::e_model_type>(out, element->node->return_type, processed_nodes, described_nodes);
    out << "element_" << element << " -> element_" << element->node->return_type << " [label=\"return_type\"], color=\"blue\"" << std::endl;
  }

  std::vector<Model::type_t>::const_iterator it;
  size_t cnt = 0;
  for (it = element->node->args_types.begin(); it != element->node->args_types.end(); it++) {
    toDotNode<Model::e_model_type>(out, *it, processed_nodes, described_nodes);
    out << "element_" << element << " -> element_" << *it << " [label=\"type argument[" << cnt++ << "]\", color=\"blue\"]" << std::endl;
  }
}

template <>
void toDotLabelNode<Model::e_model_type>(std::ostream & out, Model::type_t element, std::set<void *> & described_nodes) {
  if (described_nodes.find(element) != described_nodes.end()) return;
  described_nodes.insert(element);

  switch (element->node->kind) {
    case Model::node_t<Model::e_model_type>::e_array_type:
      out << "element_" << element << " [label=\"Array\", shape=oval]" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_pointer_type:
      out << "element_" << element << " [label=\"Pointer\", shape=oval]" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_reference_type:
      out << "element_" << element << " [label=\"Reference\", shape=oval]" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_typedef_type:
      out << "element_" << element << " [label=\"Typedef\", shape=oval]" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_class_type:
      out << "element_" << element << " [label=\"Class\", shape=oval]" << std::endl;
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
void toTextNode<Model::e_model_type>(std::ostream & out, Model::element_t<Model::e_model_type> * element) {
  switch (element->node->kind) {
    case Model::node_t<Model::e_model_type>::e_array_type:
      out << "type:" << element << " => \"Array\"" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_pointer_type:
      out << "type:" << element << " => \"Pointer\"" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_reference_type:
      out << "type:" << element << " => \"Reference\"" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_typedef_type:
      out << "type:" << element << " => \"Typedef\"" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_class_type:
      out << "type:" << element << " => \"Class\"" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_enum_type:
      out << "type:" << element << " => \"Enum\"" << std::endl;
      break;
    case Model::node_t<Model::e_model_type>::e_native_type:
      out << "type:" << element << " => \"" << element->node->type->unparseToString() << "\"" << std::endl;
      break;
    default:
      assert(false);
  }
}

template <>
void toDotNode<Model::e_model_type>(std::ostream & out, Model::type_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  toDotLabelNode<Model::e_model_type>(out, element, described_nodes);

  switch (element->node->kind) {
    case Model::node_t<Model::e_model_type>::e_array_type:
      if (element->node->base_type != NULL) {
        toDotNode<Model::e_model_type>(out, element->node->base_type, processed_nodes, described_nodes);
        out << "element_" << element << " -> element_" << element->node->base_type << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_pointer_type:
      if (element->node->base_type != NULL) {
        toDotNode<Model::e_model_type>(out, element->node->base_type, processed_nodes, described_nodes);
        out << "element_" << element << " -> element_" << element->node->base_type << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_reference_type:
      if (element->node->base_type != NULL) {
        toDotNode<Model::e_model_type>(out, element->node->base_type, processed_nodes, described_nodes);
        out << "element_" << element << " -> element_" << element->node->base_type << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_typedef_type:
      if (element->node->base_type != NULL) {
        toDotNode<Model::e_model_type>(out, element->node->base_type, processed_nodes, described_nodes);
        out << "element_" << element << " -> element_" << element->node->base_type << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_class_type:
      if (element->node->base_class != NULL) {
        toDotNode<Model::e_model_class>(out, element->node->base_class, processed_nodes, described_nodes);
        out << "element_" << element << " -> element_" << element->node->base_class << " [label=\"base\", color=\"green\"]" << std::endl;
      }
      break;
    case Model::node_t<Model::e_model_type>::e_enum_type:
    case Model::node_t<Model::e_model_type>::e_native_type:
      break;
    default:
      assert(false);
  }
}

template <>
void toDotLabelNode<Model::e_model_class>(std::ostream & out, Model::class_t element, std::set<void *> & described_nodes) {
  if (described_nodes.find(element) != described_nodes.end()) return;
  described_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\", shape=box]" << std::endl;
}

template <>
void toTextNode<Model::e_model_class>(std::ostream & out, Model::element_t<Model::e_model_class> * element) {
  out << "class:" << element << " => \"" << element->node->symbol->get_name().str() << "\"" << std::endl;
}

template <>
void toDotNode<Model::e_model_class>(std::ostream & out, Model::class_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  toDotLabelNode<Model::e_model_class>(out, element, described_nodes);

  size_t cnt = 0;
  std::vector<Model::type_t>::const_iterator it_type;
  for (it_type = element->scope->type_children.begin(); it_type != element->scope->type_children.end(); it_type++) {
    toDotNode<Model::e_model_type>(out, *it_type, processed_nodes, described_nodes);
    out << "element_" << element << " -> element_" << *it_type << " [label=\"[" << cnt++ << "]\", color=\"red:green\"]" << std::endl;
  }

  cnt = 0;
  std::vector<Model::class_t>::const_iterator it_class;
  for (it_class = element->scope->class_children.begin(); it_class != element->scope->class_children.end(); it_class++) {
    toDotNode<Model::e_model_class>(out, *it_class, processed_nodes, described_nodes);
    out << "element_" << element << " -> element_" << *it_class << " [label=\"[" << cnt++ << "]\", color=\"red:orange\"]" << std::endl;
  }

  cnt = 0;
  std::vector<Model::field_t>::const_iterator it_field;
  for (it_field = element->scope->field_children.begin(); it_field != element->scope->field_children.end(); it_field++) {
    toDotNode<Model::e_model_field>(out, *it_field, processed_nodes, described_nodes);
    out << "element_" << element << " -> element_" << *it_field << " [label=\"[" << cnt++ << "]\", color=\"red:red\"]" << std::endl;
  }

  cnt = 0;
  std::vector<Model::method_t>::const_iterator it_method;
  for (it_method = element->scope->method_children.begin(); it_method != element->scope->method_children.end(); it_method++) {
    toDotNode<Model::e_model_method>(out, *it_method, processed_nodes, described_nodes);
    out << "element_" << element << " -> element_" << *it_method << " [label=\"[" << cnt++ << "]\", color=\"red:blue\"]" << std::endl;
  }
}

template <>
void toDotLabelNode<Model::e_model_namespace>(std::ostream & out, Model::namespace_t element, std::set<void *> & described_nodes) {
  if (described_nodes.find(element) != described_nodes.end()) return;
  described_nodes.insert(element);

  out << "element_" << element << " [label=\"" << element->node->symbol->get_name().str() << "\" shape=component]" << std::endl;
}

template <>
void toTextNode<Model::e_model_namespace>(std::ostream & out, Model::element_t<Model::e_model_namespace> * element) {
  out << "namespace:" << element << " => \"" << element->node->symbol->get_name().str() << "\"" << std::endl;
}

template <>
void toDotNode<Model::e_model_namespace>(std::ostream & out, Model::namespace_t element, std::set<void *> & processed_nodes, std::set<void *> & described_nodes) {
  if (processed_nodes.find(element) != processed_nodes.end()) return;
  else processed_nodes.insert(element);

  toDotLabelNode<Model::e_model_namespace>(out, element, described_nodes);

  /// \todo
}

void model_t::toDot(std::ostream & out) const {
  std::set<Model::variable_t>  variable_set  ( variables.begin(),  variables.end()  );
  std::set<Model::function_t>  function_set  ( functions.begin(),  functions.end()  );
  std::set<Model::field_t>     field_set     ( fields.begin(),     fields.end()     );
  std::set<Model::method_t>    method_set    ( methods.begin(),    methods.end()    );
  std::set<Model::type_t>      type_set      ( types.begin(),      types.end()      );
  std::set<Model::class_t>     class_set     ( classes.begin(),    classes.end()    );
  std::set<Model::namespace_t> namespace_set ( namespaces.begin(), namespaces.end() );

  Model::toDot(out, variable_set, function_set, field_set, method_set, type_set, class_set, namespace_set);
}

void model_t::toDot(std::ostream & out, std::string prefix_filter, bool variable_root, bool function_root, bool class_root) const {
  std::set<Model::variable_t>  variable_set;
  std::set<Model::function_t>  function_set;
  std::set<Model::field_t>     field_set;
  std::set<Model::method_t>    method_set;
  std::set<Model::type_t>      type_set;
  std::set<Model::class_t>     class_set;
  std::set<Model::namespace_t> namespace_set;

  if (variable_root) {
    std::vector<Model::variable_t>::const_iterator it_variable;
    for (it_variable = variables.begin(); it_variable != variables.end(); it_variable++)
      if ((*it_variable)->node->symbol->get_name().getString().find(prefix_filter) != std::string::npos)
        variable_set.insert(*it_variable);
  }

  if (function_root) {
    std::vector<Model::function_t>::const_iterator it_function;
    for (it_function = functions.begin(); it_function != functions.end(); it_function++)
      if ((*it_function)->node->symbol->get_name().getString().find(prefix_filter) != std::string::npos)
        function_set.insert(*it_function);
  }

  if (class_root) {
    std::vector<Model::class_t>::const_iterator it_class;
    for (it_class = classes.begin(); it_class != classes.end(); it_class++)
      if ((*it_class)->node->symbol->get_name().getString().find(prefix_filter) != std::string::npos)
        class_set.insert(*it_class);
  }
  
  Model::toDot(out, variable_set, function_set, field_set, method_set, type_set, class_set, namespace_set);
}

void toDot(
  std::ostream & out,
  const std::set<Model::variable_t>  & variable_set,
  const std::set<Model::function_t>  & function_set,
  const std::set<Model::field_t>     & field_set,
  const std::set<Model::method_t>    & method_set,
  const std::set<Model::type_t>      & type_set,
  const std::set<Model::class_t>     & class_set,
  const std::set<Model::namespace_t> & namespace_set
) {
  out << "digraph {" << std::endl;

  std::set<void *> processed_nodes;
  std::set<void *> described_nodes;

//  out << "subgraph cluster_variable {" << std::endl;
//  out << "  label = \"Variables\";" << std::endl;
  std::set<Model::variable_t>::const_iterator it_variable;
  for (it_variable = variable_set.begin(); it_variable != variable_set.end(); it_variable++)
    toDotLabelNode<Model::e_model_variable>(out, *it_variable, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_function {" << std::endl;
//  out << "  label = \"Functions\";" << std::endl;
  std::set<Model::function_t>::const_iterator it_function;
  for (it_function = function_set.begin(); it_function != function_set.end(); it_function++)
    toDotLabelNode<Model::e_model_function>(out, *it_function, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_field {" << std::endl;
//  out << "  label = \"Fields\";" << std::endl;
  std::set<Model::field_t>::const_iterator it_field;
  for (it_field = field_set.begin(); it_field != field_set.end(); it_field++)
    toDotLabelNode<Model::e_model_field>(out, *it_field, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_method {" << std::endl;
//  out << "  label = \"Methods\";" << std::endl;
  std::set<Model::method_t>::const_iterator it_method;
  for (it_method = method_set.begin(); it_method != method_set.end(); it_method++)
    toDotLabelNode<Model::e_model_method>(out, *it_method, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_type {" << std::endl;
//  out << "  label = \"Types\";" << std::endl;
  std::set<Model::type_t>::const_iterator it_type;
  for (it_type = type_set.begin(); it_type != type_set.end(); it_type++)
    toDotLabelNode<Model::e_model_type>(out, *it_type, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_class {" << std::endl;
//  out << "  label = \"Classes\";" << std::endl;
  std::set<Model::class_t>::const_iterator it_class;
  for (it_class = class_set.begin(); it_class != class_set.end(); it_class++)
    toDotLabelNode<Model::e_model_class>(out, *it_class, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_namespace {" << std::endl;
//  out << "  label = \"Namespaces\";" << std::endl;
  std::set<Model::namespace_t>::const_iterator it_namespace;
  for (it_namespace = namespace_set.begin(); it_namespace != namespace_set.end(); it_namespace++)
    toDotLabelNode<Model::e_model_namespace>(out, *it_namespace, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_variable {" << std::endl;
//  out << "  label = \"Variables\";" << std::endl;
  for (it_variable = variable_set.begin(); it_variable != variable_set.end(); it_variable++)
    toDotNode<Model::e_model_variable>(out, *it_variable, processed_nodes, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_function {" << std::endl;
//  out << "  label = \"Functions\";" << std::endl;
  for (it_function = function_set.begin(); it_function != function_set.end(); it_function++)
    toDotNode<Model::e_model_function>(out, *it_function, processed_nodes, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_field {" << std::endl;
//  out << "  label = \"Fields\";" << std::endl;
  for (it_field = field_set.begin(); it_field != field_set.end(); it_field++)
    toDotNode<Model::e_model_field>(out, *it_field, processed_nodes, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_method {" << std::endl;
//  out << "  label = \"Methods\";" << std::endl;
  for (it_method = method_set.begin(); it_method != method_set.end(); it_method++)
    toDotNode<Model::e_model_method>(out, *it_method, processed_nodes, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_type {" << std::endl;
//  out << "  label = \"Types\";" << std::endl;
  for (it_type = type_set.begin(); it_type != type_set.end(); it_type++)
    toDotNode<Model::e_model_type>(out, *it_type, processed_nodes, described_nodes);
//  out << "}" << std::endl;

//  out << "subgraph cluster_class {" << std::endl;
//  out << "  label = \"Classes\";" << std::endl;
  for (it_class = class_set.begin(); it_class != class_set.end(); it_class++)
    toDotNode<Model::e_model_class>(out, *it_class, processed_nodes, described_nodes);
//  out << "}" << std::endl;

//out << "subgraph cluster_namespace {" << std::endl;
//out << "  label = \"Namespaces\";" << std::endl;
  for (it_namespace = namespace_set.begin(); it_namespace != namespace_set.end(); it_namespace++)
    toDotNode<Model::e_model_namespace>(out, *it_namespace, processed_nodes, described_nodes);
//out << "}" << std::endl;
  out << "}" << std::endl;
}

void model_t::toText(std::ostream & out) const {
  std::set<Model::variable_t>  variable_set  ( variables.begin(),  variables.end()  );
  std::set<Model::function_t>  function_set  ( functions.begin(),  functions.end()  );
  std::set<Model::field_t>     field_set     ( fields.begin(),     fields.end()     );
  std::set<Model::method_t>    method_set    ( methods.begin(),    methods.end()    );
  std::set<Model::type_t>      type_set      ( types.begin(),      types.end()      );
  std::set<Model::class_t>     class_set     ( classes.begin(),    classes.end()    );
  std::set<Model::namespace_t> namespace_set ( namespaces.begin(), namespaces.end() );

  Model::toText(out, variable_set, function_set, field_set, method_set, type_set, class_set, namespace_set);
}

void model_t::toText(std::ostream & out, std::string prefix_filter, bool variable_root, bool function_root, bool class_root) const {
  std::set<Model::variable_t>  variable_set;
  std::set<Model::function_t>  function_set;
  std::set<Model::field_t>     field_set;
  std::set<Model::method_t>    method_set;
  std::set<Model::type_t>      type_set;
  std::set<Model::class_t>     class_set;
  std::set<Model::namespace_t> namespace_set;

  if (variable_root) {
    std::vector<Model::variable_t>::const_iterator it_variable;
    for (it_variable = variables.begin(); it_variable != variables.end(); it_variable++)
      if ((*it_variable)->node->symbol->get_name().getString().find(prefix_filter) != std::string::npos)
        variable_set.insert(*it_variable);
  }

  if (function_root) {
    std::vector<Model::function_t>::const_iterator it_function;
    for (it_function = functions.begin(); it_function != functions.end(); it_function++)
      if ((*it_function)->node->symbol->get_name().getString().find(prefix_filter) != std::string::npos)
        function_set.insert(*it_function);
  }

  if (class_root) {
    std::vector<Model::class_t>::const_iterator it_class;
    for (it_class = classes.begin(); it_class != classes.end(); it_class++)
      if ((*it_class)->node->symbol->get_name().getString().find(prefix_filter) != std::string::npos)
        class_set.insert(*it_class);
  }
  
  Model::toText(out, variable_set, function_set, field_set, method_set, type_set, class_set, namespace_set);
}

void toText(
  std::ostream & out,
  const std::set<Model::variable_t>  & variable_set,
  const std::set<Model::function_t>  & function_set,
  const std::set<Model::field_t>     & field_set,
  const std::set<Model::method_t>    & method_set,
  const std::set<Model::type_t>      & type_set,
  const std::set<Model::class_t>     & class_set,
  const std::set<Model::namespace_t> & namespace_set
) {
  std::set<Model::variable_t>::const_iterator it_variable;
  for (it_variable = variable_set.begin(); it_variable != variable_set.end(); it_variable++)
    toTextNode<Model::e_model_variable>(out, *it_variable);

  std::set<Model::function_t>::const_iterator it_function;
  for (it_function = function_set.begin(); it_function != function_set.end(); it_function++)
    toTextNode<Model::e_model_function>(out, *it_function);

  std::set<Model::field_t>::const_iterator it_field;
  for (it_field = field_set.begin(); it_field != field_set.end(); it_field++)
    toTextNode<Model::e_model_field>(out, *it_field);

  std::set<Model::method_t>::const_iterator it_method;
  for (it_method = method_set.begin(); it_method != method_set.end(); it_method++)
    toTextNode<Model::e_model_method>(out, *it_method);

  std::set<Model::type_t>::const_iterator it_type;
  for (it_type = type_set.begin(); it_type != type_set.end(); it_type++)
    toTextNode<Model::e_model_type>(out, *it_type);

  std::set<Model::class_t>::const_iterator it_class;
  for (it_class = class_set.begin(); it_class != class_set.end(); it_class++)
    toTextNode<Model::e_model_class>(out, *it_class);

  std::set<Model::namespace_t>::const_iterator it_namespace;
  for (it_namespace = namespace_set.begin(); it_namespace != namespace_set.end(); it_namespace++)
    toTextNode<Model::e_model_namespace>(out, *it_namespace);
}

/** @} */

}

}

