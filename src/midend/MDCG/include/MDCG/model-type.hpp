/** 
 * \file MDCG/include/MDCG/model-type.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_TYPE_HPP__
#define __MDCG_MODEL_TYPE_HPP__

#include "MDCG/model-base.hpp"

class SgType;

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

template <>
struct node_t<e_model_type> {
  SgType * type;

  enum {
    e_array_type,
    e_pointer_type,
    e_reference_type,
    e_typedef_type,
    e_class_type,
    e_enum_type,
    e_native_type
  } kind;

  /// valid iff kind == e_array_type/e_pointer_type/e_reference_type/e_typedef_type
  type_t base_type;

  /// valid iff kind == e_typedef_type
  typedef_symbol_t * typedef_symbol;

  /// valid iff kind == e_class_type
  class_t base_class;

  /// valid iff kind == e_enum_type
  enum_symbol_t * enum_symbol;
};

template <>
struct scope_t<e_model_type> {
  union {
    namespace_t a_namespace;
    class_t a_class;
  } parent;
};

/** @} */

}

}

#endif /* __MDCG_MODEL_TYPE_HPP__ */

