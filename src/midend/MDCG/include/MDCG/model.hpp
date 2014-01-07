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

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

struct model_t {
  model_t();
  model_t(const model_t & model);

  std::vector<variable_t> variables;
  std::vector<function_t> functions;
  std::vector<type_t> types;
  std::vector<class_t> classes;
  std::vector<field_t> fields;
  std::vector<method_t> methods;
  std::vector<namespace_t> namespaces;
};

/** @} */

}

}

#endif /* __MDCG_MODEL_HPP__ */

