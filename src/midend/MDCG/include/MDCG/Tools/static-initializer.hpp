/** 
 * \file MDCG/include/MDCG/code-generator.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_TOOLS_STATIC_INITIALIZER_HPP__
#define __MDCG_TOOLS_STATIC_INITIALIZER_HPP__

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"

#include "MDCG/Model/model.hpp"

#include <string>
#include <sstream>
#include <iterator>

namespace MDCG {

namespace Tools {

/*!
 * \addtogroup grp_mdcg_static_initializer
 * @{
*/

class StaticInitializer {
  private:
    static SgVariableSymbol * instantiateDeclaration(MFB::Driver<MFB::Sage> & driver, std::string decl_name, size_t file_id, SgType * type, SgInitializer * init);

  public:
    template <class ModelTraversal>
    static SgInitializer * createInitializer(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      const typename ModelTraversal::input_t & input,
      MFB::file_id_t file_id
    );

    template <class ModelTraversal>
    static SgExpression * createPointer(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      const typename ModelTraversal::input_t & input,
      MFB::file_id_t file_id,
      const std::string & decl_name
    );

    template <class ModelTraversal, class Iterator>
    static SgAggregateInitializer * createArray(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      Iterator input_begin, Iterator input_end,
      MFB::file_id_t file_id
    );

    template <class ModelTraversal, class Iterator>
    static SgAggregateInitializer * createPointerArray(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      Iterator input_begin, Iterator input_end,
      MFB::file_id_t file_id,
      const std::string & decl_prefix
    );

    template <class ModelTraversal, class Iterator>
    static SgExpression * createArrayPointer(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      size_t num_element, Iterator input_begin, Iterator input_end,
      MFB::file_id_t file_id,
      const std::string & decl_name
    );

    template <class ModelTraversal, class Iterator>
    static SgExpression * createPointerArrayPointer(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      size_t num_element, Iterator input_begin, Iterator input_end,
      MFB::file_id_t file_id,
      const std::string & decl_name,
      const std::string & sub_decl_prefix
    );

    template <class ModelTraversal>
    static SgVariableSymbol * addDeclaration(
      MFB::Driver<MFB::Sage> & driver,
      Model::class_t element,
      const typename ModelTraversal::input_t & input,
      MFB::file_id_t file_id,
      std::string decl_name
    );
};

/** @} */

template <class ModelTraversal>
SgInitializer * StaticInitializer::createInitializer(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  const typename ModelTraversal::input_t & input,
  MFB::file_id_t file_id
) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

  driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id);

  std::vector<Model::field_t>::const_iterator it_field = element->scope->field_children.begin();
  SgExpression * expr = ModelTraversal::createFieldInitializer(driver, *it_field, 0, input, file_id);
  if (expr == NULL) return NULL;
  expr_list->append_expression(expr);
  it_field++;
  size_t field_id = 1;
  for (; it_field != element->scope->field_children.end(); it_field++) {
    expr = ModelTraversal::createFieldInitializer(driver, *it_field, field_id++, input, file_id);
    assert(expr != NULL);
    expr_list->append_expression(expr);
  }

  return SageBuilder::buildAggregateInitializer(expr_list);
}

template <class ModelTraversal>
SgExpression * StaticInitializer::createPointer(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  const typename ModelTraversal::input_t & input,
  MFB::file_id_t file_id,
  const std::string & decl_name
) {
  SgInitializer * initializer = createInitializer<ModelTraversal>(driver, element, input, file_id);
  assert(initializer != NULL);

  driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id, false);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(driver, decl_name, file_id, type, initializer);
  assert(symbol != NULL);

  return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(symbol));
}

template <class ModelTraversal, class Iterator>
SgAggregateInitializer * StaticInitializer::createArray(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  Iterator input_begin,
  Iterator input_end,
  MFB::file_id_t file_id
) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

  Iterator it;
  for (it = input_begin; it != input_end; it++) {
    SgExpression * expr = createInitializer<ModelTraversal>(driver, element, *it, file_id);
    if (expr != NULL)
      expr_list->append_expression(expr);
  }

  return SageBuilder::buildAggregateInitializer(expr_list);
}

template <class ModelTraversal, class Iterator>
SgAggregateInitializer * StaticInitializer::createPointerArray(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  Iterator input_begin,
  Iterator input_end,
  MFB::file_id_t file_id,
  const std::string & decl_prefix
) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

  size_t cnt = 0;

  Iterator it;
  for (it = input_begin; it != input_end; it++) {
    std::ostringstream decl_name;
      decl_name << decl_prefix << "_" << cnt++;
    SgExpression * expr = createPointer<ModelTraversal>(driver, element, *it, file_id, decl_name.str());
    if (expr != NULL)
      expr_list->append_expression(expr);
  }

  return SageBuilder::buildAggregateInitializer(expr_list);
}

template <class ModelTraversal, class Iterator>
SgExpression * StaticInitializer::createArrayPointer(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  size_t num_element,
  Iterator input_begin,
  Iterator input_end,
  MFB::file_id_t file_id,
  const std::string & decl_name
) {
  SgAggregateInitializer * aggr_init = createArray<ModelTraversal, Iterator>(driver, element, input_begin, input_end, file_id);

  driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);
  type = SageBuilder::buildArrayType(type, SageBuilder::buildUnsignedLongVal(num_element));
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(driver, decl_name, file_id, type, aggr_init);
  assert(symbol != NULL);

  return SageBuilder::buildVarRefExp(symbol);
}

template <class ModelTraversal, class Iterator>
SgExpression * StaticInitializer::createPointerArrayPointer(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  size_t num_element,
  Iterator input_begin,
  Iterator input_end,
  MFB::file_id_t file_id,
  const std::string & decl_name,
  const std::string & sub_decl_prefix
) {
  SgAggregateInitializer * aggr_init = createPointerArray<ModelTraversal, Iterator>(driver, element, input_begin, input_end, file_id, sub_decl_prefix);

  driver.useSymbol<SgClassDeclaration>(element->node->symbol, file_id);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);
  type = SageBuilder::buildPointerType(type);
  assert(type != NULL);
  type = SageBuilder::buildArrayType(type, SageBuilder::buildUnsignedLongVal(num_element));
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(driver, decl_name, file_id, type, aggr_init);
  assert(symbol != NULL);

  return SageBuilder::buildVarRefExp(symbol);
}

template <class ModelTraversal>
SgVariableSymbol * StaticInitializer::addDeclaration(
  MFB::Driver<MFB::Sage> & driver,
  Model::class_t element,
  const typename ModelTraversal::input_t & input,
  MFB::file_id_t file_id,
  std::string decl_name
) {
  SgInitializer * initializer = createInitializer<ModelTraversal>(driver, element, input, file_id);
  assert(initializer != NULL);

  SgType * type = element->node->symbol->get_type();
  assert(type != NULL);

  SgVariableSymbol * symbol = instantiateDeclaration(driver, decl_name, file_id, type, initializer);
  assert(symbol != NULL);
      
  return symbol;
}

}

}

#endif /* __MDCG_TOOLS_STATIC_INITIALIZER_HPP__ */

