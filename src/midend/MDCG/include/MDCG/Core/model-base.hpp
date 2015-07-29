/** 
 * \file MDCG/include/MDCG/model-base.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_BASE_HPP__
#define __MDCG_MODEL_BASE_HPP__

#include <cstddef>

#include <vector>
#include <map>

class SgSymbol;
class SgVariableSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgMemberFunctionSymbol;
class SgNamespaceSymbol;
class SgTypedefSymbol;
class SgEnumSymbol;

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

/// Different kind of elements in the model
enum model_elements_e {
  e_model_blank,
  e_model_variable,
  e_model_function,
  e_model_field,
  e_model_method,
  e_model_type,
  e_model_class,
  e_model_namespace
};

/// Content of the element for [*_]symbol_t
template <model_elements_e kind_>
struct node_t;

/// Scoping information for [*_]symbol_t
template <model_elements_e kind_>
struct scope_t;

/// Actual representation of an element
template <model_elements_e kind_>
struct element_t {
  enum kind_e { kind = kind_ } inst_kind;  

  node_t<kind_>  * node;  ///< Node  part of the element: store the symbol and some extra-information
  scope_t<kind_> * scope; ///< Scope part of the element: Information on the scoping of the elements

  virtual ~element_t() {
    if (node != NULL) delete node;
    if (scope != NULL) delete scope;
  }
};

template <model_elements_e kind>
element_t<kind> * build() {
  element_t<kind> * element = new element_t<kind>();
    element->inst_kind = (typename element_t<kind>::kind_e)kind;
    element->node = new node_t<kind>();
    element->scope = new scope_t<kind>();
  return element;
}

template <model_elements_e kind>
node_t<kind> * copy(node_t<kind> * orig, std::map<void*, void*> & copies_map);

template <model_elements_e kind>
scope_t<kind> * copy(scope_t<kind> * orig, std::map<void*, void*> & copies_map);

template <model_elements_e kind>
element_t<kind> * copy(element_t<kind> * orig, std::map<void*, void*> & copies_map) {
  std::map<void*, void*>::iterator it = copies_map.find(orig);
  if (it == copies_map.end()) {
    element_t<kind> * element = new element_t<kind>();

    copies_map.insert(std::pair<void*, void*>(orig, element));

    element->node = copy(orig->node, copies_map);
    element->scope = copy(orig->scope, copies_map);

    return element;
  }
  else return (element_t<kind> *)it->second;
}

/// Blank element (used for cast)
template <> struct node_t<e_model_blank> {};
template <> struct scope_t<e_model_blank> {};
typedef element_t<e_model_blank> * blank_element_t;

/// A variable: part of a namespace. interact_with(type)
typedef element_t<e_model_variable> * variable_t;

/// A function: part of a namespace. interact_with(variable: global, type: argument and return value, method: call, function: call, field: of variable)
typedef element_t<e_model_function> * function_t;

/// A type: can be native types, structures, classes or typedef. interact_with(type: typedef, class: classes)
typedef element_t<e_model_type> * type_t;

/// A class: declare fields, methods, and types. interact_with(namespace: parent, class: child & parent, type: child, field: child, method:child)
typedef element_t<e_model_class> * class_t;

/// A field: part of a class. interact_with(class: parent, type)
typedef element_t<e_model_field> * field_t;

/// A method: part of a class. interact_with(class: parent, field: access, method: call, function: call, variable: global, type: argument and return value)
typedef element_t<e_model_method> * method_t;

/// A namespace. interact_with(child: variable & function & type & class & namespace, namespace: parent)
typedef element_t<e_model_namespace> * namespace_t;

/// [*_]symbol_t represents any symbol

typedef               SgSymbol symbol_t;
typedef      SgNamespaceSymbol namespace_symbol_t;
typedef       SgFunctionSymbol function_symbol_t;
typedef       SgVariableSymbol variable_symbol_t;
typedef          SgClassSymbol class_symbol_t;
typedef SgMemberFunctionSymbol method_symbol_t;
typedef      variable_symbol_t field_symbol_t; // FIXME no specific SgSymbol in ROSE for field_symbol_t, use SgVariableSymbol
typedef               symbol_t type_symbol_t;  // FIXME SgTypeSymbol? for SgEnumSymbol, SgTypedefSymbol, maybe SgClassType, and native type (see type_node_t for current work arround)

typedef        SgTypedefSymbol typedef_symbol_t;
typedef           SgEnumSymbol enum_symbol_t;

/** @} */

}

}

#endif /*  __MDCG_MODEL_BASE_HPP__ */

