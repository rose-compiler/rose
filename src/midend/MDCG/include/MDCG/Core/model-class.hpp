/** 
 * \file MDCG/include/MDCG/model-class.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_CLASS_HPP__
#define __MDCG_MODEL_CLASS_HPP__

#include "MDCG/Core/model-base.hpp"

#include <string>

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

/*! 
 * 
 * Node part of model_t (element_t<e_model_class>)
 */
template <>
struct node_t<e_model_class> {
  /// Pointer to the associated class symbol
  class_symbol_t * symbol;

  /// Describe how the associated class inherit from another class
  struct inheritance_kind_t {
    /// Access to inherited fields and methods: private, protected, public
    enum {
      e_private,
      e_protected,
      e_public
    } inheritance_privilege;
    /// The class is inherited virtually
    bool virtual_inheritance;
  };
  /// Associate to each inherited classes: access privileges and virtual inheritance flag
  std::map<class_t, inheritance_kind_t> base_classes;
};

template <>
struct scope_t<e_model_class> {
  union {
    namespace_t a_namespace;
    class_t a_class;
  } parent;

  std::vector<type_t>      type_children;
  std::vector<class_t>     class_children;
  std::vector<field_t>     field_children;
  std::vector<method_t>    method_children;

  field_t getField(const std::string & name) const;
  method_t getMethod(const std::string & name) const;
};

/** @} */

}

}

#endif /* __MDCG_MODEL_CLASS_HPP__ */

