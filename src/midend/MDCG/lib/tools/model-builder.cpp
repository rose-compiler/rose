
#include "sage3basic.h"

#include "MDCG/Tools/model-builder.hpp"
#include "MFB/Sage/api.hpp"
#include "MFB/Sage/driver.hpp"

#include <cassert>

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace MDCG {

namespace Tools {

template <Model::model_elements_e kind>
void ModelBuilder::setParentFromScope(Model::model_t & model, Model::element_t<kind> * element, SgSymbol * symbol) {
  SgScopeStatement * scope = symbol->get_scope();
  assert(scope != NULL);

#if VERBOSE
  std::cerr << "[Info] (MDCG::Tools::ModelBuilder::setParentFromScope<" << kind << ">) symbol: " << symbol->get_name() << " (" << symbol->class_name() << ")." << std::endl;
#endif

  SgNamespaceDefinitionStatement * nsp_defn     = isSgNamespaceDefinitionStatement(scope);
  SgClassDefinition              * class_defn   = isSgClassDefinition(scope);
  SgGlobal                       * global_scope = isSgGlobal(scope);

  if (nsp_defn != NULL) {
    // field and method cannot have namespace for scope
    assert(kind != Model::e_model_field && kind != Model::e_model_method);
  
    SgNamespaceDeclarationStatement * nsp_decl = nsp_defn->get_namespaceDeclaration();
    assert(nsp_decl != NULL);
    
    SgNamespaceSymbol * parent_symbol = isSgNamespaceSymbol(nsp_decl->get_symbol_from_symbol_table());
    assert(parent_symbol != NULL);
    
    Model::namespace_t parent_element = model.lookup_namespace(parent_symbol);
    if (parent_element == NULL) {
      add(model, parent_symbol);
      parent_element = model.lookup_namespace(parent_symbol);
    }
    assert(parent_element != NULL);
    
    switch (kind) {
      case Model::e_model_variable:  parent_element->scope->variable_children.push_back  ( (Model::variable_t)  element); break;
      case Model::e_model_function:  parent_element->scope->function_children.push_back  ( (Model::function_t)  element); break;
      case Model::e_model_type:      parent_element->scope->type_children.push_back      ( (Model::type_t)      element); break;
      case Model::e_model_class:     parent_element->scope->class_children.push_back     ( (Model::class_t)     element); break;
      case Model::e_model_namespace: parent_element->scope->namespace_children.push_back ( (Model::namespace_t) element); break;

      case Model::e_model_field:
      case Model::e_model_method:
        assert(false);
    }
    
    switch (kind) {
      case Model::e_model_variable:  ( (Model::variable_t)  element)->scope->parent = parent_element; break;
      case Model::e_model_function:  ( (Model::function_t)  element)->scope->parent = parent_element; break;
      case Model::e_model_namespace: ( (Model::namespace_t) element)->scope->parent = parent_element; break;

      case Model::e_model_type:  ( (Model::type_t)  element)->scope->parent.a_namespace = parent_element; break;
      case Model::e_model_class: ( (Model::class_t) element)->scope->parent.a_namespace = parent_element; break;

      case Model::e_model_field:
      case Model::e_model_method:
        assert(false);
    }
  }
  else if (class_defn != NULL) {

#if VERBOSE
    std::cerr << "[Info] (MDCG::Tools::ModelBuilder::setParentFromScope<" << kind << ">) Scope is Class Defn. Decl: " << class_defn->get_declaration() << ", name: " << class_defn->get_declaration()->get_name().getString() << std::endl;
#endif

    // namespace, function, and variable cannot have class for scope
    assert(kind != Model::e_model_namespace);
    assert(kind != Model::e_model_function);
    assert(kind != Model::e_model_variable);
    
    SgClassDeclaration * class_decl = class_defn->get_declaration();
    assert(class_decl != NULL);
    
    SgClassSymbol * parent_symbol = isSgClassSymbol(class_decl->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table());
    assert(parent_symbol != NULL);
    
    Model::class_t parent_element = model.lookup_class(parent_symbol);
    if (parent_element == NULL) {
      add(model, parent_symbol);
      parent_element = model.lookup_class(parent_symbol);
    }
    assert(parent_element != NULL);

    switch (kind) {
      case Model::e_model_field:  parent_element->scope->field_children.push_back  ( (Model::field_t)  element); break;
      case Model::e_model_method: parent_element->scope->method_children.push_back ( (Model::method_t) element); break;
      case Model::e_model_type:   parent_element->scope->type_children.push_back   ( (Model::type_t)   element); break;
      case Model::e_model_class:  parent_element->scope->class_children.push_back  ( (Model::class_t)  element); break;

      case Model::e_model_namespace:
      case Model::e_model_variable:
      case Model::e_model_function:
        assert(false);
    }

    switch (kind) {
      case Model::e_model_field:  ( (Model::field_t)  element)->scope->parent = parent_element; break;
      case Model::e_model_method: ( (Model::method_t) element)->scope->parent = parent_element; break;
      
      case Model::e_model_type:  ( (Model::type_t)  element)->scope->parent.a_class = parent_element; break;
      case Model::e_model_class: ( (Model::class_t) element)->scope->parent.a_class = parent_element; break;

      case Model::e_model_namespace:
      case Model::e_model_variable:
      case Model::e_model_function:
        assert(false);
    }
  }
  else if (global_scope != NULL) {
    /// \todo Should we have a root namespace to represent the global scope ???
  }
  else {
    /// \todo error printing
    assert(false);
  }
}

ModelBuilder::ModelBuilder(MFB::Driver<MFB::Sage> & driver) :
  p_driver(driver),
  p_models()
{}

MFB::Driver<MFB::Sage> & ModelBuilder::getDriver() { return p_driver; }
const MFB::Driver<MFB::Sage> & ModelBuilder::getDriver() const { return p_driver; }
 
size_t ModelBuilder::create() {
  size_t model = p_models.size();
  p_models.push_back(Model::model_t());
  return model;
}

void ModelBuilder::add(
  size_t model_id,
  const std::string & name,
  const std::string & path,
  std::string suffix
) {
  boost::filesystem::path file_path = path + "/" + name + "." + suffix;
  size_t file_id = p_driver.add(file_path);

  MFB::api_t * api = p_driver.getAPI(file_id);

  add(p_models[model_id], api);
}

void ModelBuilder::add(Model::model_t & model, const MFB::api_t * api) {
  std::set<SgNamespaceSymbol *>::const_iterator it_namespace_symbol;
  for (it_namespace_symbol = api->namespace_symbols.begin(); it_namespace_symbol != api->namespace_symbols.end(); it_namespace_symbol++)
    if (model.lookup_namespace(*it_namespace_symbol) == NULL) 
      add(model, *it_namespace_symbol);

  std::set<SgVariableSymbol *>::const_iterator it_variable_symbol;
  for (it_variable_symbol = api->variable_symbols.begin(); it_variable_symbol != api->variable_symbols.end(); it_variable_symbol++)
    if (model.lookup_variable(*it_variable_symbol) == NULL && model.lookup_field(*it_variable_symbol) == NULL)
      add(model, *it_variable_symbol);

  std::set<SgFunctionSymbol *>::const_iterator it_function_symbol;
  for (it_function_symbol = api->function_symbols.begin(); it_function_symbol != api->function_symbols.end(); it_function_symbol++) {
    if (model.lookup_function(*it_function_symbol) == NULL) {
      add(model, *it_function_symbol);
    }
  }

  std::set<SgClassSymbol *>::const_iterator it_class_symbol;
  for (it_class_symbol = api->class_symbols.begin(); it_class_symbol != api->class_symbols.end(); it_class_symbol++)
    if (model.lookup_class(*it_class_symbol) == NULL)
      add(model, *it_class_symbol);

  std::set<SgMemberFunctionSymbol *>::const_iterator it_member_function_symbol;
  for (it_member_function_symbol = api->member_function_symbols.begin(); it_member_function_symbol != api->member_function_symbols.end(); it_member_function_symbol++)
    if (model.lookup_method(*it_member_function_symbol) == NULL)
      add(model, *it_member_function_symbol);
}

void ModelBuilder::add(Model::model_t & model, SgNamespaceSymbol * namespace_symbol) {
  Model::namespace_t element = Model::build<Model::e_model_namespace>();

  element->node->symbol = namespace_symbol;
  
  setParentFromScope<Model::e_model_namespace>(model, element, namespace_symbol);

  model.namespaces.push_back(element);
}

void ModelBuilder::add(Model::model_t & model, SgVariableSymbol * variable_symbol) {
  SgScopeStatement * scope = variable_symbol->get_scope();
  assert(scope != NULL);

  SgNamespaceDefinitionStatement * nsp_defn     = isSgNamespaceDefinitionStatement(scope);
  SgClassDefinition              * class_defn   = isSgClassDefinition(scope);
  SgGlobal                       * global_scope = isSgGlobal(scope);
  
  SgType * sg_type = variable_symbol->get_type();
  assert(sg_type != NULL);
  Model::type_t type = model.lookup_type(sg_type);
  if (type == NULL) {
    add(model, sg_type);
    type = model.lookup_type(sg_type);
  }
  assert(type != NULL);
  
  if (nsp_defn != NULL || global_scope != NULL) {
    Model::variable_t element = Model::build<Model::e_model_variable>();

    element->node->symbol = variable_symbol;
    element->node->type = type;
  
    setParentFromScope<Model::e_model_variable>(model, element, variable_symbol);

    model.variables.push_back(element);
  }
  else if (class_defn != NULL) {
    Model::field_t element = Model::build<Model::e_model_field>();

    element->node->symbol = variable_symbol;
    element->node->type = type;
  
    setParentFromScope<Model::e_model_field>(model, element, variable_symbol);

    model.fields.push_back(element);
  }
  else {
    /// \todo error printing
    assert(false);
  }
}

void ModelBuilder::add(Model::model_t & model, SgFunctionSymbol * function_symbol) {
#if VERBOSE
  std::cerr << "[Info] (MDCG::Tools::ModelBuilder::add, SgFunctionSymbol) function_symbol: " << function_symbol->get_name() << " (" << function_symbol->class_name() << ")." << std::endl;
#endif

  Model::function_t element = Model::build<Model::e_model_function>();

  element->node->symbol = function_symbol;

  SgFunctionType * func_type = isSgFunctionType(function_symbol->get_type());
  assert(func_type != NULL);

  SgType * func_return_type = func_type->get_return_type();
  assert(func_return_type != NULL);
  element->node->return_type = model.lookup_type(func_return_type);
  if (element->node->return_type == NULL) {
    add(model, func_return_type);
    element->node->return_type = model.lookup_type(func_return_type);
  }
  assert(element->node->return_type != NULL);

  SgFunctionParameterTypeList * param_type_list = func_type->get_argument_list();
  assert(param_type_list != NULL);
  const std::vector<SgType *> & arguments = param_type_list->get_arguments();
  std::vector<SgType *>::const_iterator it_argument;
  for (it_argument = arguments.begin(); it_argument != arguments.end(); it_argument++) {
    Model::type_t type = model.lookup_type(*it_argument);
    if (type == NULL) {
      add(model, *it_argument);
      type = model.lookup_type(*it_argument);
    }
    assert(type != NULL);
    element->node->args_types.push_back(type);
  }
  
  setParentFromScope<Model::e_model_function>(model, element, function_symbol);

  model.functions.push_back(element);
}

void ModelBuilder::add(Model::model_t & model, SgClassSymbol * class_symbol) {
  Model::class_t element = Model::build<Model::e_model_class>();

  element->node->symbol = class_symbol;

  /// \todo std::map<class_t, inheritance_kind_t> base_classes;
  
  setParentFromScope<Model::e_model_class>(model, element, class_symbol);

  model.classes.push_back(element);
}

void ModelBuilder::add(Model::model_t & model, SgMemberFunctionSymbol * member_function_symbol) {
#if VERBOSE
  std::cerr << "[Info] (MDCG::Tools::ModelBuilder::add, SgMemberFunctionSymbol) member_function_symbol: " << member_function_symbol->get_name() << " (" << member_function_symbol->class_name() << ")." << std::endl;
#endif

  Model::method_t element = Model::build<Model::e_model_method>();

  element->node->symbol = member_function_symbol;

  SgFunctionType * func_type = isSgFunctionType(member_function_symbol->get_type());
  assert(func_type != NULL);

  SgType * func_return_type = func_type->get_return_type();
  assert(func_return_type != NULL);
  element->node->return_type = model.lookup_type(func_return_type);
  if (element->node->return_type == NULL) {
    add(model, func_return_type);
    element->node->return_type = model.lookup_type(func_return_type);
  }
  assert(element->node->return_type != NULL);

  SgFunctionParameterTypeList * param_type_list = func_type->get_argument_list();
  assert(param_type_list != NULL);
  const std::vector<SgType *> & arguments = param_type_list->get_arguments();
  std::vector<SgType *>::const_iterator it_argument;
  for (it_argument = arguments.begin(); it_argument != arguments.end(); it_argument++) {
    Model::type_t type = model.lookup_type(*it_argument);
    if (type == NULL) {
      add(model, *it_argument);
      type = model.lookup_type(*it_argument);
    }
    assert(type != NULL);
    element->node->args_types.push_back(type);
  }
  
  setParentFromScope<Model::e_model_method>(model, element, member_function_symbol);

  model.methods.push_back(element);
}

void ModelBuilder::add(Model::model_t & model, SgType * sg_type) {
  SgModifierType * modifier_type  = isSgModifierType(sg_type);
  if (modifier_type != NULL) {
    add(model, modifier_type->get_base_type());
    return;
  }

  Model::type_t element = Model::build<Model::e_model_type>();

  element->node->type = sg_type;

  SgNamedType     * named_type     = isSgNamedType(sg_type);
  SgArrayType     * array_type     = isSgArrayType(sg_type);
  SgPointerType   * pointer_type   = isSgPointerType(sg_type);
  SgReferenceType * reference_type = isSgReferenceType(sg_type);
  if (named_type != NULL) {
    SgClassType   * class_type   = isSgClassType(named_type);
    SgEnumType    * enum_type    = isSgEnumType(named_type);
    SgTypedefType * typedef_type = isSgTypedefType(named_type);

    SgDeclarationStatement * decl_stmt = named_type->get_declaration()->get_firstNondefiningDeclaration();
    assert(decl_stmt != NULL);
    SgSymbol * decl_sym = decl_stmt->get_symbol_from_symbol_table();
    assert(decl_sym != NULL);

    if (class_type != NULL) {
      element->node->kind = Model::node_t<Model::e_model_type>::e_class_type;

      SgClassSymbol * class_sym = isSgClassSymbol(decl_sym);
      assert(class_sym != NULL);
      element->node->base_class = model.lookup_class(class_sym);
      if (element->node->base_class == NULL) {
        add(model, class_sym);
        element->node->base_class = model.lookup_class(class_sym);
      }
      assert(element->node->base_class != NULL);
    }
    else if (enum_type != NULL) {
      element->node->kind = Model::node_t<Model::e_model_type>::e_enum_type;

      SgEnumSymbol * enum_sym = isSgEnumSymbol(decl_sym);
      assert(enum_sym != NULL);
      element->node->enum_symbol = enum_sym;
    }
    else if (typedef_type != NULL) {
      element->node->kind = Model::node_t<Model::e_model_type>::e_typedef_type;

      SgTypedefSymbol * typedef_sym = isSgTypedefSymbol(decl_sym);
      assert(typedef_sym != NULL);
      element->node->typedef_symbol = typedef_sym;

      element->node->base_type = model.lookup_type(typedef_type->get_base_type());
      if (element->node->base_type == NULL) {
        add(model, typedef_type->get_base_type());
        element->node->base_type = model.lookup_type(typedef_type->get_base_type());
      }
      assert(element->node->base_type != NULL);
    }
    else assert(false);
  }
  else if (array_type != NULL) {
    element->node->kind = Model::node_t<Model::e_model_type>::e_array_type;

    element->node->base_type = model.lookup_type(array_type->get_base_type());
    if (element->node->base_type == NULL) {
      add(model, array_type->get_base_type());
      element->node->base_type = model.lookup_type(array_type->get_base_type());
    }
    assert(element->node->base_type != NULL);
  }
  else if (pointer_type != NULL) {
    element->node->kind = Model::node_t<Model::e_model_type>::e_pointer_type;

    element->node->base_type = model.lookup_type(pointer_type->get_base_type());
    if (element->node->base_type == NULL) {
      add(model, pointer_type->get_base_type());
      element->node->base_type = model.lookup_type(pointer_type->get_base_type());
    }
    assert(element->node->base_type != NULL);
  }
  else if (reference_type != NULL) {
    element->node->kind = Model::node_t<Model::e_model_type>::e_reference_type;

    element->node->base_type = model.lookup_type(reference_type->get_base_type());
    if (element->node->base_type == NULL) {
      add(model, reference_type->get_base_type());
      element->node->base_type = model.lookup_type(reference_type->get_base_type());
    }
    assert(element->node->base_type != NULL);
  }
  else {
    element->node->kind = Model::node_t<Model::e_model_type>::e_native_type;
  }
  
  element->scope->parent.a_namespace = NULL; /// \todo

  model.types.push_back(element);
}
 
const Model::model_t & ModelBuilder::get(const size_t model_id) const {
  return p_models[model_id];
}

void ModelBuilder::print(std::ostream & out, size_t model) const {
  assert(false); /// \todo
}

}

}

