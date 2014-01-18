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

type_t model_t::lookup_type_by(SgSymbol * symbol) const {
  std::vector<type_t>::const_iterator it;
  for (it = types.begin(); it != types.end(); it++)
    if ((*it)->node->symbol == symbol)
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

/** @} */

}

}

