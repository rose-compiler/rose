/** 
 * \file MDCG/include/MDCG/code-generator.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_CODE_GENERATOR_HPP__
#define __MDCG_CODE_GENERATOR_HPP__

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"

#include "MDCG/Core/model.hpp"

#include <string>
#include <sstream>
#include <iterator>

namespace MDCG {

/*!
 * \addtogroup grp_mdcg_static_initializer
 * @{
*/

class StaticInitializer {
  private:
    static size_t s_var_gen_cnt;
  
    MFB::Driver<MFB::Sage> & p_mfb_driver;

    SgVariableSymbol * instantiateDeclaration(std::string decl_name, size_t file_id, SgType * type, SgInitializer * init) const;

  public:
    StaticInitializer(MFB::Driver<MFB::Sage> & mfb_driver);

    MFB::Driver<MFB::Sage> & getDriver() const;

    template <class ModelTraversal>
    SgInitializer * createInitializer(
      Model::class_t element,
      const typename ModelTraversal::input_t & input,
      MFB::file_id_t file_id
    ) const;

    template <class ModelTraversal>
    SgExpression * createPointer(
      Model::class_t element,
      const typename ModelTraversal::input_t & input,
      MFB::file_id_t file_id,
      const std::string & decl_name
    ) const;

    template <class ModelTraversal, class Iterator>
    SgAggregateInitializer * createArray(
      Model::class_t element,
      Iterator input_begin, Iterator input_end,
      MFB::file_id_t file_id
    ) const;

    template <class ModelTraversal, class Iterator>
    SgAggregateInitializer * createPointerArray(
      Model::class_t element,
      Iterator input_begin, Iterator input_end,
      MFB::file_id_t file_id,
      const std::string & decl_prefix
    ) const;

    template <class ModelTraversal, class Iterator>
    SgExpression * createArrayPointer(
      Model::class_t element,
      size_t num_element, Iterator input_begin, Iterator input_end,
      MFB::file_id_t file_id,
      const std::string & decl_name
    ) const;

    template <class ModelTraversal, class Iterator>
    SgExpression * createPointerArrayPointer(
      Model::class_t element,
      size_t num_element, Iterator input_begin, Iterator input_end,
      MFB::file_id_t file_id,
      const std::string & decl_name,
      const std::string & sub_decl_prefix
    ) const;

    template <class ModelTraversal>
    SgVariableSymbol * addDeclaration(
      Model::class_t element,
      typename ModelTraversal::input_t & input,
      MFB::file_id_t file_id,
      std::string decl_name
    ) const;

  /// match 'struct class_name field_name;'
  static Model::class_t getBaseClass(Model::field_t field, std::string field_name = std::string(), std::string class_name = std::string());

  /// match 'struct class_name * field_name;'
  static Model::class_t getBaseClassForPointerOnClass(Model::field_t field, std::string field_name = std::string(), std::string class_name = std::string());

  /// match 'struct class_name field_name[size];'
  static Model::class_t getBaseClassForArrayOnClass(Model::field_t field, std::string field_name = std::string(), std::string class_name = std::string());

  /// match 'typedef struct class_name * typedef_name; typedef_name field_name;'
  static Model::class_t getBaseClassForTypedefOnPointerOnClass(Model::field_t field, std::string field_name = std::string(), std::string class_name = std::string());

  /// match 'typedef struct class_name * typedef_name; typedef_name * field_name;'
  static Model::class_t getBaseClassForPointerOnTypedefOnPointerOnClass(Model::field_t field, std::string field_name = std::string(), std::string class_name = std::string());

  /// match 'typedef struct class_name * typedef_name; typedef_name field_name[size];'
  static Model::class_t getBaseClassForArrayOnTypedefOnPointerOnClass(Model::field_t field, std::string field_name = std::string(), std::string class_name = std::string());
};

/** @} */

template <class ModelTraversal>
SgInitializer * StaticInitializer::createInitializer(
  Model::class_t element,
  const typename ModelTraversal::input_t & input,
  MFB::file_id_t file_id
) const {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

  p_mfb_driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id);

  std::vector<Model::field_t>::const_iterator it_field = element->scope->field_children.begin();
  SgExpression * expr = ModelTraversal::createFieldInitializer(*this, *it_field, 0, input, file_id);
  if (expr == NULL) return NULL;
  expr_list->append_expression(expr);
  it_field++;
  size_t field_id = 1;
  for (; it_field != element->scope->field_children.end(); it_field++) {
    expr = ModelTraversal::createFieldInitializer(*this, *it_field, field_id++, input, file_id);
    assert(expr != NULL);
    expr_list->append_expression(expr);
  }

  return SageBuilder::buildAggregateInitializer(expr_list);
}

template <class ModelTraversal>
SgExpression * StaticInitializer::createPointer(
  Model::class_t element,
  const typename ModelTraversal::input_t & input,
  MFB::file_id_t file_id,
  const std::string & decl_name
) const {
  SgInitializer * initializer = createInitializer<ModelTraversal>(element, input, file_id);
  assert(initializer != NULL);

  p_mfb_driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id, false);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(decl_name, file_id, type, initializer);
  assert(symbol != NULL);

  return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(symbol));
}

template <class ModelTraversal, class Iterator>
SgAggregateInitializer * StaticInitializer::createArray(
  Model::class_t element,
  Iterator input_begin,
  Iterator input_end,
  MFB::file_id_t file_id
) const {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

  Iterator it;
  for (it = input_begin; it != input_end; it++) {
    SgExpression * expr = createInitializer<ModelTraversal>(element, *it, file_id);
    if (expr != NULL)
      expr_list->append_expression(expr);
  }

  return SageBuilder::buildAggregateInitializer(expr_list);
}

template <class ModelTraversal, class Iterator>
SgAggregateInitializer * StaticInitializer::createPointerArray(
  Model::class_t element,
  Iterator input_begin,
  Iterator input_end,
  MFB::file_id_t file_id,
  const std::string & decl_prefix
) const {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

  size_t cnt = 0;

  Iterator it;
  for (it = input_begin; it != input_end; it++) {
    std::ostringstream decl_name;
      decl_name << decl_prefix << "_" << cnt++;
    SgExpression * expr = createPointer<ModelTraversal>(element, *it, file_id, decl_name.str());
    if (expr != NULL)
      expr_list->append_expression(expr);
  }

  return SageBuilder::buildAggregateInitializer(expr_list);
}

template <class ModelTraversal, class Iterator>
SgExpression * StaticInitializer::createArrayPointer(
  Model::class_t element,
  size_t num_element,
  Iterator input_begin,
  Iterator input_end,
  MFB::file_id_t file_id,
  const std::string & decl_name
) const {
  SgAggregateInitializer * aggr_init = createArray<ModelTraversal, Iterator>(element, input_begin, input_end, file_id);

  p_mfb_driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);
  type = SageBuilder::buildArrayType(type, SageBuilder::buildUnsignedLongVal(num_element));
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(decl_name, file_id, type, aggr_init);
  assert(symbol != NULL);

  return SageBuilder::buildVarRefExp(symbol);
}

template <class ModelTraversal, class Iterator>
SgExpression * StaticInitializer::createPointerArrayPointer(
  Model::class_t element,
  size_t num_element,
  Iterator input_begin,
  Iterator input_end,
  MFB::file_id_t file_id,
  const std::string & decl_name,
  const std::string & sub_decl_prefix
) const {
  SgAggregateInitializer * aggr_init = createPointerArray<ModelTraversal, Iterator>(element, input_begin, input_end, file_id, sub_decl_prefix);

  p_mfb_driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);
  type = SageBuilder::buildPointerType(type);
  assert(type != NULL);
  type = SageBuilder::buildArrayType(type, SageBuilder::buildUnsignedLongVal(num_element));
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(decl_name, file_id, type, aggr_init);
  assert(symbol != NULL);

  return SageBuilder::buildVarRefExp(symbol);
}

template <class ModelTraversal>
SgVariableSymbol * StaticInitializer::addDeclaration(
  Model::class_t element,
  typename ModelTraversal::input_t & input,
  MFB::file_id_t file_id,
  std::string decl_name
) const {
  SgInitializer * initializer = createInitializer<ModelTraversal>(element, input, file_id);
  assert(initializer != NULL);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(decl_name, file_id, type, initializer);
  assert(symbol != NULL);
      
  return symbol;
}

}

#endif /* __MDCG_CODE_GENERATOR_HPP__ */

