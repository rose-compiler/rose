/** 
 * \file MDCG/lib/model-builder.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MDCG/model-builder.hpp"
#include "MFB/Sage/api.hpp"
#include "MFB/Sage/driver.hpp"

#include <cassert>

namespace MDCG {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

ModelBuilder::ModelBuilder(MFB::Driver<MFB::Sage> & mfb_driver) :
  p_mfb_driver(mfb_driver),
  p_models()
{}
 
unsigned ModelBuilder::create() {
  unsigned model = p_models.size();
  p_models.push_back(Model::model_t());
  return model;
}

void ModelBuilder::addOne(
  unsigned model_id,
  const std::string & name,
  const std::string & path,
  std::string suffix
) {
  unsigned long file_id = p_mfb_driver.loadStandaloneSourceFile(name, path, suffix);

  MFB::api_t * api = p_mfb_driver.getAPI(file_id);

  add(p_models[model_id], api);
}

void ModelBuilder::add(Model::model_t & model, const MFB::api_t * api) {
  std::set<SgNamespaceSymbol *>::const_iterator it_namespace_symbol;
  for (it_namespace_symbol = api->namespace_symbols.begin(); it_namespace_symbol != api->namespace_symbols.end(); it_namespace_symbol++)
    if (model.lookup_namespace_by(*it_namespace_symbol) == NULL) 
      add(model, *it_namespace_symbol);

  std::set<SgVariableSymbol *>::const_iterator it_variable_symbol;
  for (it_variable_symbol = api->variable_symbols.begin(); it_variable_symbol != api->variable_symbols.end(); it_variable_symbol++)
    if (model.lookup_variable_by(*it_variable_symbol) == NULL && model.lookup_field_by(*it_variable_symbol) == NULL)
      add(model, *it_variable_symbol);

  std::set<SgFunctionSymbol *>::const_iterator it_function_symbol;
  for (it_function_symbol = api->function_symbols.begin(); it_function_symbol != api->function_symbols.end(); it_function_symbol++)
    if (model.lookup_function_by(*it_function_symbol) == NULL)
      add(model, *it_function_symbol);

  std::set<SgClassSymbol *>::const_iterator it_class_symbol;
  for (it_class_symbol = api->class_symbols.begin(); it_class_symbol != api->class_symbols.end(); it_class_symbol++)
    if (model.lookup_class_by(*it_class_symbol) == NULL)
      add(model, *it_class_symbol);

  std::set<SgMemberFunctionSymbol *>::const_iterator it_member_function_symbol;
  for (it_member_function_symbol = api->member_function_symbols.begin(); it_member_function_symbol != api->member_function_symbols.end(); it_member_function_symbol++)
    if (model.lookup_method_by(*it_member_function_symbol) == NULL)
      add(model, *it_member_function_symbol);
}

void ModelBuilder::add(Model::model_t & model, SgNamespaceSymbol * namespace_symbol) {
  SgScopeStatement * scope = namespace_symbol->get_scope();
  SgNamespaceDefinitionStatement * nsp_defn = isSgNamespaceDefinitionStatement(scope);
  if (nsp_defn != NULL) {
    SgNamespaceDeclarationStatement * nsp_decl = nsp_defn->get_namespaceDeclaration();
    assert(nsp_decl != NULL);
    SgNamespaceSymbol * parent_namespace_symbol = isSgNamespaceSymbol(nsp_decl->get_symbol_from_symbol_table());
    assert(parent_namespace_symbol != NULL);
    if (model.lookup_namespace_by(parent_namespace_symbol) == NULL) 
      add(model, parent_namespace_symbol);
    assert(false); /// \todo build  elem to insert
  }
  else {
    assert(isSgGlobal(scope));
    assert(false); /// \todo build  elem to insert
  }
}

void ModelBuilder::add(Model::model_t & model, SgVariableSymbol * variable_symbol) {
  /// \todo
}

void ModelBuilder::add(Model::model_t & model, SgFunctionSymbol * function_symbol) {
  /// \todo
}

void ModelBuilder::add(Model::model_t & model, SgClassSymbol * class_symbol) {
  /// \todo
}

void ModelBuilder::add(Model::model_t & model, SgMemberFunctionSymbol * member_function_symbol) {
  /// \todo
}

void ModelBuilder::addPair(
  unsigned model,
  const std::string & name,
  const std::string & header_path,
  const std::string & source_path,
  std::string header_suffix,
  std::string source_suffix
) {
  assert(false); /// \todo
}
 
const Model::model_t & ModelBuilder::get(const unsigned model_id) const {
  return p_models[model_id];
}

void ModelBuilder::print(std::ostream & out, unsigned model) const {
  assert(false); /// \todo
}

/** @} */

}

