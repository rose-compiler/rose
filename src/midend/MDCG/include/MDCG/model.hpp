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

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

struct model_t {
  model_t();
  model_t(const model_t & model);

  // Variables

  std::vector<variable_t> variables;

  variable_t lookup_variable_by(SgVariableSymbol * symbol) const;

  // Functions

  std::vector<function_t> functions;

  function_t lookup_function_by(SgFunctionSymbol * symbol) const;

  // Types

  std::vector<type_t> types;

  type_t lookup_type_by(SgType * type) const;

  // Classes

  std::vector<class_t> classes;

  class_t lookup_class_by(SgClassSymbol * symbol) const;

  // Fields

  std::vector<field_t> fields;

  field_t lookup_field_by(SgVariableSymbol * symbol) const;

  // Methods

  std::vector<method_t> methods;

  method_t lookup_method_by(SgMemberFunctionSymbol * symbol) const;

  // Namespaces

  std::vector<namespace_t> namespaces;

  namespace_t lookup_namespace_by(SgNamespaceSymbol * symbol) const;

  // Printer

  void toDot(std::ostream & out, std::string prefix_filter, bool, bool, bool) const;
};

/** @} */

}

}

#endif /* __MDCG_MODEL_HPP__ */

