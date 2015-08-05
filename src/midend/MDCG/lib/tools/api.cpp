
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/function-declaration.hpp"

#include "MDCG/Tools/api.hpp"

namespace MDCG {

namespace Tools {

bool api_t::load(namespace_t & element, namespace_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const namespace_t parent) {
  assert(parent == NULL); // TODO can be: NULL, or a namespace

  element = model.lookup<namespace_t>(name);
  if (element == NULL) return false;
  symbol = element->node->symbol;
  symbols.namespaces.insert(symbol);
  return (symbol != NULL);
}

bool api_t::load(function_t & element, function_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const namespace_t parent) {
  assert(parent == NULL); // TODO can be: NULL, or a namespace

  element = model.lookup<function_t>(name);
  if (element == NULL) return false;
  symbol = element->node->symbol;
  symbols.functions.insert(symbol);
  return (symbol != NULL);
}

bool api_t::load(variable_t & element, variable_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const namespace_t parent) {
  assert(parent == NULL); // TODO can be: NULL, or a namespace

  element = model.lookup<variable_t>(name);
  if (element == NULL) return false;
  symbol = element->node->symbol;
  symbols.variables.insert(symbol);
  return (symbol != NULL);
}

bool api_t::load(class_t & element, class_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const blank_t parent) {
  assert(parent == NULL); // TODO can be: NULL, or a namespace, or a class

  element = model.lookup<class_t>(name);
  if (element == NULL) return false;
  symbol = element->node->symbol;
  symbols.classes.insert(symbol);
  return (symbol != NULL);
}

bool api_t::load(method_t & element, method_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const class_t parent) {
  assert(parent != NULL);
  element = parent->scope->getMethod(name);
  symbol = element->node->symbol;
  symbols.methods.insert(symbol);
  return (symbol != NULL);;
}

bool api_t::load(field_t & element, field_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const class_t parent) {
  assert(parent != NULL);
  element = parent->scope->getField(name);
  symbol = element->node->symbol;
  symbols.fields.insert(symbol);
  return (symbol != NULL);;
}

typedef namespace_symbol_set_t::const_iterator namespace_citer_t;
typedef  function_symbol_set_t::const_iterator function_citer_t;
typedef  variable_symbol_set_t::const_iterator variable_citer_t;
typedef     class_symbol_set_t::const_iterator class_citer_t;
typedef     field_symbol_set_t::const_iterator field_citer_t;
typedef    method_symbol_set_t::const_iterator method_citer_t;

void api_t::use(::MFB::Driver< ::MFB::Sage> & driver, ::MFB::file_id_t file_id) const {
  // FIXME only import headers for class and function. What about other symbols? When do we need forward declaration only?
  for (class_citer_t it = symbols.classes.begin(); it != symbols.classes.end(); it++)
    driver.useSymbol<SgClassDeclaration>(*it, file_id);
  for (function_citer_t it = symbols.functions.begin(); it != symbols.functions.end(); it++)
    driver.useSymbol<SgFunctionDeclaration>(*it, file_id);
}

void api_t::use(::MFB::Driver< ::MFB::Sage> & driver, SgScopeStatement * scope) const {
  use(driver, driver.getFileID(scope));
}

void api_t::use(::MFB::Driver< ::MFB::Sage> & driver, SgSourceFile * file) const {
  use(driver, driver.getFileID(file));
}

} // namespace MDCG::Tools

} // namespace MDCG

