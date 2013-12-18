/** 
 * \file MDCG/include/MDCG/handle-builder.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_HANDLE_BUILDER_HPP__
#define __MDCG_HANDLE_BUILDER_HPP__

#include "MDCG/model.hpp"

#include "Handles/hooks.hpp"

#include <vector>
#include <utility>
#include <string>
#include <set>

class SgStatement;
class SgSymbol;
class SgLocatedNode;

namespace MDCG {

/*!
 * \addtogroup grp_mdcg_handles
 * @{
*/

struct load_model_t {
  enum opcode_e {
    e_create,
    e_append,
    e_opcode_last
  };
  struct fields_t {
    std::vector<std::pair<std::string, bool> > files; // boolean true if system header
    unsigned model;
  };
  struct return_fields_t {
    unsigned id;
  };
};

struct traverse_model_t {
  enum opcode_e {
    e_search,
    e_opcode_last
  };
  struct fields_t {
    std::string name;
    Model::model_elements_e kind;
  };
  struct return_fields_t {
    union {
      std::set<Model::variable_t>  variables;
      std::set<Model::function_t>  functions;
      std::set<Model::type_t>      types;
      std::set<Model::class_t>     classes;
      std::set<Model::field_t>     fields;
      std::set<Model::method_t>    methods;
      std::set<Model::namespace_t> namespaces;
    };
  };
};

struct use_model_t {
  enum opcode_e {
    e_instantiate_type,
    e_instantiate_class,
    e_call_function,
    e_call_method,
    e_opcode_last
  };
  struct fields_t {
    union {
      Model::type_t  used_type;
      Model::class_t used_class;

      Model::function_t used_function;
      Model::method_t   used_method;
    };
    std::string variable_name; /// for instantiations
    SgSymbol * object;         /// for method calls
  };
  struct return_fields_t {
    union {
      SgStatement  * stmt; /// Statement generated
      SgExpression * expr; /// Statement generated
    } generated;
    SgSymbol * symbol;  /// Associated symbol (for instantiations)
    SgExprListExp * call_args;    /// Arguments list for function calls and constructors
  };
};

class ModelBuilder;

/** @} */

}

namespace Handles {

/*!
 * \addtogroup grp_mdcg_handles
 * @{
*/

template <>
const unsigned long handle_hook_t<MDCG::ModelBuilder, MDCG::_model_t>::submit(handle_t *);

template <>
handle_hook_t<MDCG::ModelBuilder, MDCG::_model_t>::return_handle_t * handle_hook_t<MDCG::ModelBuilder, MDCG::_model_t>::get_result(const unsigned long);

/** @} */

}

#endif /* __MDCG_HANDLE_BUILDER_HPP__ */

