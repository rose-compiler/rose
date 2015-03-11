/** 
 * \file MDCG/include/MDCG/model.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_HPP__
#define __MDCG_MODEL_HPP__

#include "MDCG/model-variable.hpp"
#include "MDCG/model-function.hpp"
#include "MDCG/model-type.hpp"
#include "MDCG/model-class.hpp"
#include "MDCG/model-field.hpp"
#include "MDCG/model-method.hpp"
#include "MDCG/model-namespace.hpp"

#include <vector>
#include <set>

#include <iostream>
#include <string>

#include <cassert>

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

struct model_t {
  model_t();
  model_t(const model_t & model);

  ~model_t();

  // Variables

  std::vector<variable_t> variables;

  variable_t lookup_variable(SgVariableSymbol * symbol) const;

  // Functions

  std::vector<function_t> functions;

  function_t lookup_function(SgFunctionSymbol * symbol) const;

  // Types

  std::vector<type_t> types;

  type_t lookup_type(SgType * type) const;

  // Classes

  std::vector<class_t> classes;

  class_t lookup_class(SgClassSymbol * symbol) const;

  // Fields

  std::vector<field_t> fields;

  field_t lookup_field(SgVariableSymbol * symbol) const;

  // Methods

  std::vector<method_t> methods;

  method_t lookup_method(SgMemberFunctionSymbol * symbol) const;

  // Namespaces

  std::vector<namespace_t> namespaces;

  namespace_t lookup_namespace(SgNamespaceSymbol * symbol) const;
  
  template <typename element_t>
  void lookup(const std::string & name, std::set<element_t> & result) const;
  
  template <typename element_t>
  element_t lookup(const std::string & name) const {
    std::set<element_t> element_set;
    lookup<element_t>(name, element_set);
    if (element_set.size() != 1) {
      std::cerr << "Found " << element_set.size() << " matches for \"" << name << "\" (expect one)." << std::endl;
      assert(element_set.size() == 1);
    }
    element_t result = *(element_set.begin());
    assert(result != NULL);
    return result;
  }

  // Printer

  void toText(std::ostream & out) const;
  void toText(std::ostream & out, std::string prefix_filter, bool variable_root, bool function_root, bool class_root) const;

  void toDot(std::ostream & out) const;
  void toDot(std::ostream & out, std::string prefix_filter, bool variable_root, bool function_root, bool class_root) const;
};

void toDot(
  std::ostream & out,
  const std::set<Model::variable_t>  & variable_set,
  const std::set<Model::function_t>  & function_set,
  const std::set<Model::field_t>     & field_set,
  const std::set<Model::method_t>    & method_set,
  const std::set<Model::type_t>      & type_set,
  const std::set<Model::class_t>     & class_set,
  const std::set<Model::namespace_t> & namespace_set
);

void toText(
  std::ostream & out,
  const std::set<Model::variable_t>  & variable_set,
  const std::set<Model::function_t>  & function_set,
  const std::set<Model::field_t>     & field_set,
  const std::set<Model::method_t>    & method_set,
  const std::set<Model::type_t>      & type_set,
  const std::set<Model::class_t>     & class_set,
  const std::set<Model::namespace_t> & namespace_set
);

template <>
void model_t::lookup<Model::variable_t>(const std::string & name, std::set<Model::variable_t> & result) const;

template <>
void model_t::lookup<Model::field_t>(const std::string & name, std::set<Model::field_t> & result) const;

template <>
void model_t::lookup<Model::function_t>(const std::string & name, std::set<Model::function_t> & result) const;

template <>
void model_t::lookup<Model::method_t>(const std::string & name, std::set<Model::method_t> & result) const;

template <>
void model_t::lookup<Model::class_t>(const std::string & name, std::set<Model::class_t> & result) const;

template <>
void model_t::lookup<Model::namespace_t>(const std::string & name, std::set<Model::namespace_t> & result) const;

/** @} */

}

}

#endif /* __MDCG_MODEL_HPP__ */

