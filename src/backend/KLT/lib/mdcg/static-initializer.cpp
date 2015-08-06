
#include "sage3basic.h"

#include "KLT/MDCG/static-initializer.hpp"

namespace KLT {

namespace MDCG {

SgExpression * createParamSizeOf(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<SgVariableSymbol *> & parameters) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();
  SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);

  std::vector<SgVariableSymbol *>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++)
    expr_list->append_expression(SageBuilder::buildSizeOfOp((*it)->get_type()));

  SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildUnsignedLongVal(parameters.size()));

  return SageBuilder::buildVarRefExp(::MDCG::Tools::StaticInitializer::instantiateDeclaration(driver, decl_name, file_id, type, init));
}

SgExpression * createDataSizeOf(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<Descriptor::data_t *> & data) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();
  SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++)
    expr_list->append_expression(SageBuilder::buildSizeOfOp((*it)->base_type));

  SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildUnsignedLongVal(data.size()));

  return SageBuilder::buildVarRefExp(::MDCG::Tools::StaticInitializer::instantiateDeclaration(driver, decl_name, file_id, type, init));
}

SgExpression * createDataNDims(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<Descriptor::data_t *> & data) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();
  SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++)
    expr_list->append_expression(SageBuilder::buildIntVal((*it)->sections.size()));

  SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildUnsignedLongVal(data.size()));

  return SageBuilder::buildVarRefExp(::MDCG::Tools::StaticInitializer::instantiateDeclaration(driver, decl_name, file_id, type, init));
}

SgExpression * DataContainer::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  size_t file_id
) {
  switch (field_id) {
    case 0:
    { // int num_param;
      return SageBuilder::buildIntVal(input.parameters.size());
    }
    case 1:
    { // int * sizeof_param;
      std::ostringstream decl_name; decl_name << "sizeof_param_" << &input;
      return createParamSizeOf(driver, decl_name.str(), file_id, input.parameters);
    }
    case 2:
    { // int num_data;
      return SageBuilder::buildIntVal(input.data.size());
    }
    case 3:
    { // int * sizeof_data;
      std::ostringstream decl_name; decl_name << "sizeof_data_" << &input;
      return createDataSizeOf(driver, decl_name.str(), file_id, input.data);
    }
    case 4:
    { // int * ndims_data;
      std::ostringstream decl_name; decl_name << "ndims_data_" << &input;
      return createDataNDims(driver, decl_name.str(), file_id, input.data);
    }
    default:
      assert(false);
  }
}

SgExpression * TileDesc::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  size_t file_id
) {
  switch (field_id) {
    case 0:
    { // int idx;
      return SageBuilder::buildIntVal(input->id);
    }
    case 1:
    { // enum tile_kind_e kind;
      return SageBuilder::buildIntVal(input->kind);
    }
    case 2:
    { // struct klt_tile_desc_t * tile_desc;
      return SageInterface::copyExpression(input->param);
    }
    default:
      assert(false);
  }
}

SgExpression * LoopDesc::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  size_t file_id
) {
  switch (field_id) {
    case 0:
    { // int idx;
      return SageBuilder::buildIntVal(input->id);
    }
    case 1:
    { // int num_tiles;
      return SageBuilder::buildIntVal(input->tiles.size());
    }
    case 2:
    { // struct klt_tile_desc_t * tile_desc;
      std::ostringstream decl_name; decl_name << "tile_desc_" << input;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("tile_desc", "klt_tile_desc_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<TileDesc>(
                 driver, field_class, input->tiles.size(), input->tiles.begin(), input->tiles.end(), file_id, decl_name.str()
             );
    }
    default:
      assert(false);
  }
}

SgExpression * TopLoopContainer::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  size_t file_id
) {
  switch (field_id) {
    case 0:
    { // int num_loops;
      return SageBuilder::buildIntVal(input.size());
    }
    case 1:
    { // int num_tiles;
      return SageBuilder::buildIntVal(0); // klt_loop_container_t for the original loop-tree => no tile
    }
    case 2:
    { // struct klt_loop_desc_t * loop_desc;
      std::ostringstream decl_name; decl_name << "top_loop_desc_" << &input;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("loop_desc", "klt_loop_desc_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<LoopDesc>(
                 driver, field_class, input.size(), input.begin(), input.end(), file_id, decl_name.str()
             );
    }
    default:
      assert(false);
  }
}

SgExpression * LoopContainer::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  size_t file_id
) {
  switch (field_id) {
    case 0:
    { // int num_loops;
      return SageBuilder::buildIntVal(input.loops.size());
    }
    case 1:
    { // int num_tiles;
      return SageBuilder::buildIntVal(input.tiles.size());
    }
    case 2:
    { // struct klt_loop_desc_t * loop_desc;
      std::ostringstream decl_name; decl_name << "loop_desc_" << &input;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("loop_desc", "klt_loop_desc_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<LoopDesc>(
                 driver, field_class, input.loops.size(), input.loops.begin(), input.loops.end(), file_id, decl_name.str()
             );
    }
    default:
     assert(false);
  }
}

SgExpression * SubKernelDesc::createFieldInitializer(
  MFB::Driver<MFB::Sage> & driver,
  ::MDCG::Model::field_t element,
  size_t field_id,
  const input_t & input,
  size_t file_id
) {
  switch (field_id) {
    case 0:
    { // struct klt_loop_container_t loop;
      ::MDCG::Model::class_t field_class = element->node->getBaseClass("loop", "klt_loop_container_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createInitializer<LoopContainer>(driver, field_class, *input.first, file_id);
    }
    case 1:
    { // int num_params;
      return SageBuilder::buildIntVal(input.first->parameters.size());
    }
    case 2:
    { // int * param_ids;
      return SageBuilder::buildIntVal(0); // TODO
    }
    case 3:
    { // int num_data;
      return SageBuilder::buildIntVal(input.first->data.size());
    }
    case 4:
    { // int * data_ids;
      return SageBuilder::buildIntVal(0); // TODO
    }
    case 5:
    { // int num_loops;
      return SageBuilder::buildIntVal(input.first->loops.size());
    }
    case 6:
    { // int * loop_ids;
      return SageBuilder::buildIntVal(0); // TODO
    }
    case 7:
    { // int num_deps;
      return SageBuilder::buildIntVal(input.second.size());
    }
    case 8:
    { // int * deps_ids;
      return SageBuilder::buildIntVal(0); // TODO
    }
    case 9:
    { // struct klt_subkernel_config_t * config;
      return SageBuilder::buildIntVal(0); // TODO
    }
    default:
      assert(false);
  }
}

} // namespace KLT::MDCG

} // namespace KLT

