
#ifndef __MDCG_CORE_API_HPP__
#define __MDCG_CORE_API_HPP__

#include "MFB/Sage/driver.hpp"

#include "MDCG/Core/model.hpp"
#include "MDCG/Core/model-namespace.hpp"
#include "MDCG/Core/model-function.hpp"
#include "MDCG/Core/model-variable.hpp"
#include "MDCG/Core/model-class.hpp"
#include "MDCG/Core/model-method.hpp"
#include "MDCG/Core/model-field.hpp"
#include "MDCG/Core/model-type.hpp"

#include <set>

class SgSymbol;
class SgNamespaceSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgVariableSymbol;
class SgMemberFunctionSymbol;

namespace MDCG {

namespace API {

typedef SgSymbol symbol_t;
typedef SgSymbol * symbol_ptr;

typedef Model::namespace_t namespace_t;
typedef SgNamespaceSymbol namespace_symbol_t;
typedef SgNamespaceSymbol * namespace_symbol_ptr;
typedef std::set<namespace_symbol_ptr> namespace_symbol_set_t;

typedef Model::function_t function_t;
typedef SgFunctionSymbol function_symbol_t;
typedef SgFunctionSymbol * function_symbol_ptr;
typedef std::set<function_symbol_ptr> function_symbol_set_t;

typedef Model::variable_t variable_t;
typedef SgVariableSymbol variable_symbol_t;
typedef SgVariableSymbol * variable_symbol_ptr;
typedef std::set<variable_symbol_ptr> variable_symbol_set_t;

typedef Model::class_t class_t;
typedef SgClassSymbol class_symbol_t;
typedef SgClassSymbol * class_symbol_ptr;
typedef std::set<class_symbol_ptr> class_symbol_set_t;

typedef Model::field_t field_t;
typedef SgVariableSymbol field_symbol_t;
typedef SgVariableSymbol * field_symbol_ptr;
typedef std::set<field_symbol_ptr> field_symbol_set_t;

typedef MDCG::Model::method_t method_t;
typedef SgMemberFunctionSymbol method_symbol_t;
typedef SgMemberFunctionSymbol * method_symbol_ptr;
typedef std::set<method_symbol_ptr> method_symbol_set_t;

typedef MDCG::Model::type_t type_t;

typedef MDCG::Model::blank_element_t blank_t;

struct api_t {
  public:
    struct symbols_collection_t {
      namespace_symbol_set_t namespaces;
      function_symbol_set_t  functions;
      variable_symbol_set_t  variables;
      class_symbol_set_t     classes;
      method_symbol_set_t    methods;
      field_symbol_set_t     fields;
    };
  protected:
    symbols_collection_t symbols;
    
    bool load(namespace_t & element, namespace_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const namespace_t parent);
    bool load( function_t & element,  function_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const namespace_t parent);
    bool load( variable_t & element,  variable_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const namespace_t parent);
    bool load(    class_t & element,     class_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const blank_t parent);
    bool load(   method_t & element,    method_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const class_t parent);
    bool load(    field_t & element,     field_symbol_ptr & symbol, const MDCG::Model::model_t & model, const std::string & name, const class_t parent);

  public:
    void use(::MFB::Driver< ::MFB::Sage> & driver, ::MFB::file_id_t file_id) const;
    void use(::MFB::Driver< ::MFB::Sage> & driver, SgScopeStatement * scope) const;
    void use(::MFB::Driver< ::MFB::Sage> & driver, SgSourceFile * file) const;

    virtual void load(const MDCG::Model::model_t & model) = 0;
};

} // namespace MDCG::API

} // namespace MDCG

#endif /* __MDCG_CORE_API_HPP__ */

