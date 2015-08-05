
#include "MDCG/Model/namespace.hpp"
#include "MDCG/Model/variable.hpp"
#include "MDCG/Model/function.hpp"
#include "MDCG/Model/class.hpp"
#include "MDCG/Model/field.hpp"
#include "MDCG/Model/method.hpp"
#include "MDCG/Model/type.hpp"

#include <cassert>

namespace MDCG {

namespace Model {

template <>
node_t<e_model_namespace> * copy<e_model_namespace>(node_t<e_model_namespace> * orig, std::map<void*, void*> & copies_map) {
  node_t<e_model_namespace> * res = new node_t<e_model_namespace>();

  res->symbol = orig->symbol;

  return res;
}

template <>
scope_t<e_model_namespace> * copy<e_model_namespace>(scope_t<e_model_namespace> * orig, std::map<void*, void*> & copies_map) {
  scope_t<e_model_namespace> * res = new scope_t<e_model_namespace>();

  res->parent = copy(orig->parent, copies_map);

  std::vector<namespace_t>::const_iterator it_nspc;
  for (it_nspc = orig->namespace_children.begin(); it_nspc != orig->namespace_children.end(); it_nspc++)
    res->namespace_children.push_back(copy(*it_nspc, copies_map));

  std::vector<variable_t>::const_iterator it_var;
  for (it_var = orig->variable_children.begin(); it_var != orig->variable_children.end(); it_var++)
    res->variable_children.push_back(copy(*it_var, copies_map));

  std::vector<function_t>::const_iterator it_func;
  for (it_func = orig->function_children.begin(); it_func != orig->function_children.end(); it_func++)
    res->function_children.push_back(copy(*it_func, copies_map));

  std::vector<class_t>::const_iterator it_class;
  for (it_class = orig->class_children.begin(); it_class != orig->class_children.end(); it_class++)
    res->class_children.push_back(copy(*it_class, copies_map));

  std::vector<type_t>::const_iterator it_type;
  for (it_type = orig->type_children.begin(); it_type != orig->type_children.end(); it_type++)
    res->type_children.push_back(copy(*it_type, copies_map));

  return res;
}

template <>
node_t<e_model_variable> * copy<e_model_variable>(node_t<e_model_variable> * orig, std::map<void*, void*> & copies_map) {
  node_t<e_model_variable> * res = new node_t<e_model_variable>();

  res->symbol = orig->symbol;
  res->type = copy(orig->type, copies_map);

  return res;
}

template <>
scope_t<e_model_variable> * copy<e_model_variable>(scope_t<e_model_variable> * orig, std::map<void*, void*> & copies_map) {
  scope_t<e_model_variable> * res = new scope_t<e_model_variable>();

  res->parent = copy(orig->parent, copies_map);

  return res;
}

template <>
node_t<e_model_function> * copy<e_model_function>(node_t<e_model_function> * orig, std::map<void*, void*> & copies_map) {
  node_t<e_model_function> * res = new node_t<e_model_function>();

  res->symbol = orig->symbol;
  res->return_type = copy(orig->return_type, copies_map);

  std::vector<type_t>::const_iterator it_type;
  for (it_type = orig->args_types.begin(); it_type != orig->args_types.end(); it_type++)
    res->args_types.push_back(copy(*it_type, copies_map));

  return res;
}

template <>
scope_t<e_model_function> * copy<e_model_function>(scope_t<e_model_function> * orig, std::map<void*, void*> & copies_map) {
  scope_t<e_model_function> * res = new scope_t<e_model_function>();

  res->parent = copy(orig->parent, copies_map);

  return res;
}

template <>
node_t<e_model_class> * copy<e_model_class>(node_t<e_model_class> * orig, std::map<void*, void*> & copies_map) {
  node_t<e_model_class> * res = new node_t<e_model_class>();

  res->symbol = orig->symbol;

  std::map<class_t, node_t<e_model_class>::inheritance_kind_t>::const_iterator it;
  for (it = orig->base_classes.begin(); it != orig->base_classes.end(); it++)
    res->base_classes.insert(std::pair<class_t, node_t<e_model_class>::inheritance_kind_t>(copy(it->first, copies_map), it->second));

  return res;
}

template <>
scope_t<e_model_class> * copy<e_model_class>(scope_t<e_model_class> * orig, std::map<void*, void*> & copies_map) {
  scope_t<e_model_class> * res = new scope_t<e_model_class>();

  if (dynamic_cast<namespace_t>(orig->parent.a_namespace) != NULL)
    res->parent.a_namespace = copy(orig->parent.a_namespace, copies_map);
  else if (dynamic_cast<class_t>(orig->parent.a_class) != NULL)
    res->parent.a_class = copy(orig->parent.a_class, copies_map);
  else
    assert(false);

  std::vector<class_t>::const_iterator it_class;
  for (it_class = orig->class_children.begin(); it_class != orig->class_children.end(); it_class++)
    res->class_children.push_back(copy(*it_class, copies_map));

  std::vector<field_t>::const_iterator it_field;
  for (it_field = orig->field_children.begin(); it_field != orig->field_children.end(); it_field++)
    res->field_children.push_back(copy(*it_field, copies_map));

  std::vector<method_t>::const_iterator it_method;
  for (it_method = orig->method_children.begin(); it_method != orig->method_children.end(); it_method++)
    res->method_children.push_back(copy(*it_method, copies_map));

  std::vector<type_t>::const_iterator it_type;
  for (it_type = orig->type_children.begin(); it_type != orig->type_children.end(); it_type++)
    res->type_children.push_back(copy(*it_type, copies_map));

  return res;
}

template <>
node_t<e_model_field> * copy<e_model_field>(node_t<e_model_field> * orig, std::map<void*, void*> & copies_map) {
  node_t<e_model_field> * res = new node_t<e_model_field>();

  res->symbol = orig->symbol;
  res->type = copy(orig->type, copies_map);  

  return res;
}

template <>
scope_t<e_model_field> * copy<e_model_field>(scope_t<e_model_field> * orig, std::map<void*, void*> & copies_map) {
  scope_t<e_model_field> * res = new scope_t<e_model_field>();

  res->parent = copy(orig->parent, copies_map);

  return res;
}

template <>
node_t<e_model_method> * copy<e_model_method>(node_t<e_model_method> * orig, std::map<void*, void*> & copies_map) {
  node_t<e_model_method> * res = new node_t<e_model_method>();

  res->symbol = orig->symbol;
  res->return_type = copy(orig->return_type, copies_map);

  std::vector<type_t>::const_iterator it_type;
  for (it_type = orig->args_types.begin(); it_type != orig->args_types.end(); it_type++)
    res->args_types.push_back(copy(*it_type, copies_map));

  return res;
}

template <>
scope_t<e_model_method> * copy<e_model_method>(scope_t<e_model_method> * orig, std::map<void*, void*> & copies_map) {
  scope_t<e_model_method> * res = new scope_t<e_model_method>();

  res->parent = copy(orig->parent, copies_map);

  return res;
}

template <>
node_t<e_model_type> * copy<e_model_type>(node_t<e_model_type> * orig, std::map<void*, void*> & copies_map) {
  node_t<e_model_type> * res = new node_t<e_model_type>();

  res->type = orig->type;
  res->kind = orig->kind;
  res->base_type = copy(orig->base_type, copies_map);
  res->typedef_symbol = orig->typedef_symbol;
  res->base_class = copy(orig->base_class, copies_map);
  res->enum_symbol = orig->enum_symbol;

  return res;
}

template <>
scope_t<e_model_type> * copy<e_model_type>(scope_t<e_model_type> * orig, std::map<void*, void*> & copies_map) {
  scope_t<e_model_type> * res = new scope_t<e_model_type>();

  if (dynamic_cast<namespace_t>(orig->parent.a_namespace) != NULL)
    res->parent.a_namespace = copy(orig->parent.a_namespace, copies_map);
  else if (dynamic_cast<class_t>(orig->parent.a_class) != NULL)
    res->parent.a_class = copy(orig->parent.a_class, copies_map);
  else
    assert(false);

  return res;
}

}

}

