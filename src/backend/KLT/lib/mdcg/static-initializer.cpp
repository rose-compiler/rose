
#include "sage3basic.h"

#include "KLT/MDCG/static-initializer.hpp"

namespace KLT {

namespace MDCG {

// I hate using globals but I cannot see another way here
std::map<  SgVariableSymbol *, size_t> param_ids_map; // Filled by: , Used by: createParamIds
std::map<Descriptor::data_t *, size_t> data_ids_map;  // Filled by: , Used by: createDataIds

void registerParamAndDataIds(const Kernel::kernel_t & original) {
  std::vector<SgVariableSymbol *>::const_iterator it_vsym;
  for (it_vsym = original.parameters.begin(); it_vsym != original.parameters.end(); it_vsym++)
    param_ids_map.insert(std::pair<SgVariableSymbol *, size_t>(*it_vsym, param_ids_map.size()));
  std::vector<Descriptor::data_t *>::const_iterator it_data;
  for (it_data = original.data.begin(); it_data != original.data.end(); it_data++)
    data_ids_map.insert(std::pair<Descriptor::data_t *, size_t>(*it_data, data_ids_map.size()));
}

void clearParamAndDataIds() {
  param_ids_map.clear();
  data_ids_map.clear();
}

std::pair<SgVarRefExp *, SgExprListExp *> createIntArray(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, size_t size) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();
  SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);
  SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildUnsignedLongVal(size));
  SgVarRefExp * var_ref = SageBuilder::buildVarRefExp(::MDCG::Tools::StaticInitializer::instantiateDeclaration(driver, decl_name, file_id, type, init));
  return std::pair<SgVarRefExp *, SgExprListExp *>(var_ref, expr_list);
}

SgExpression * createParamSizeOf(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<SgVariableSymbol *> & parameters) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createIntArray(driver, decl_name, file_id, parameters.size());

  std::vector<SgVariableSymbol *>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++) {
    driver.useType((*it)->get_type(), file_id);
    res.second->append_expression(SageBuilder::buildSizeOfOp((*it)->get_type()));
  }

  return res.first;
}

SgExpression * createDataSizeOf(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<Descriptor::data_t *> & data) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createIntArray(driver, decl_name, file_id, data.size());

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++) {
    driver.useType((*it)->base_type, file_id);
    res.second->append_expression(SageBuilder::buildSizeOfOp((*it)->base_type));
  }

  return res.first;
}

SgExpression * createDataNDims(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<Descriptor::data_t *> & data) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createIntArray(driver, decl_name, file_id, data.size());

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++)
    res.second->append_expression(SageBuilder::buildIntVal((*it)->sections.size()));

  return res.first;
}

SgExpression * createParamIds(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<SgVariableSymbol *> & parameters) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createIntArray(driver, decl_name, file_id, parameters.size());

  std::vector<SgVariableSymbol *>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++) {
    std::map<SgVariableSymbol *, size_t>::const_iterator it_param = param_ids_map.find(*it);
    assert(it_param != param_ids_map.end());
    res.second->append_expression(SageBuilder::buildIntVal(it_param->second));
  }

  return res.first;
}

SgExpression * createDataIds(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<Descriptor::data_t *> & data) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createIntArray(driver, decl_name, file_id, data.size());

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++) {
    std::map<Descriptor::data_t *, size_t>::const_iterator it_data = data_ids_map.find(*it);
    assert(it_data != data_ids_map.end());
    res.second->append_expression(SageBuilder::buildIntVal(it_data->second));
  }

  return res.first;
}

SgExpression * createLoopIds(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<Descriptor::loop_t *> & loops) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createIntArray(driver, decl_name, file_id, loops.size());

  std::vector<Descriptor::loop_t *>::const_iterator it;
  for (it = loops.begin(); it != loops.end(); it++)
    res.second->append_expression(SageBuilder::buildIntVal((*it)->id));

  return res.first;
}

SgExpression * createDepsIds(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<Descriptor::kernel_t *> & deps) {
  std::pair<SgVarRefExp *, SgExprListExp *> res = createIntArray(driver, decl_name, file_id, deps.size());

  std::vector<Descriptor::kernel_t *>::const_iterator it;
  for (it = deps.begin(); it != deps.end(); it++)
    res.second->append_expression(SageBuilder::buildIntVal((*it)->id));

  return res.first;
}

size_t DataContainer::cnt[3] = {0,0,0};

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
      std::ostringstream decl_name; decl_name << "sizeof_param_" << cnt[0]++;
      return createParamSizeOf(driver, decl_name.str(), file_id, input.parameters);
    }
    case 2:
    { // int num_data;
      return SageBuilder::buildIntVal(input.data.size());
    }
    case 3:
    { // int * sizeof_data;
      std::ostringstream decl_name; decl_name << "sizeof_data_" << cnt[1]++;
      return createDataSizeOf(driver, decl_name.str(), file_id, input.data);
    }
    case 4:
    { // int * ndims_data;
      std::ostringstream decl_name; decl_name << "ndims_data_" << cnt[2]++;
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
    { // int param;
      if (input->param != NULL)
        return SageInterface::copyExpression(input->param);
      else
        return SageBuilder::buildIntVal(0);
    }
    default:
      assert(false);
  }
}

size_t LoopDesc::cnt[1] = {0};

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
      std::ostringstream decl_name; decl_name << "tile_desc_" << cnt[0]++;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("tile_desc", "klt_tile_desc_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<TileDesc>(
                 driver, field_class, input->tiles.size(), input->tiles.begin(), input->tiles.end(), file_id, decl_name.str()
             );
    }
    default:
      assert(false);
  }
}

size_t TopLoopContainer::cnt[1] = {0};

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
      std::ostringstream decl_name; decl_name << "top_loop_desc_" << cnt[0]++;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("loop_desc", "klt_loop_desc_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<LoopDesc>(
                 driver, field_class, input.size(), input.begin(), input.end(), file_id, decl_name.str()
             );
    }
    default:
      assert(false);
  }
}

size_t LoopContainer::cnt[1] = {0};

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
      std::ostringstream decl_name; decl_name << "loop_desc_" << cnt[0]++;
      ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("loop_desc", "klt_loop_desc_t"); assert(field_class != NULL);
      return ::MDCG::Tools::StaticInitializer::createArrayPointer<LoopDesc>(
                 driver, field_class, input.loops.size(), input.loops.begin(), input.loops.end(), file_id, decl_name.str()
             );
    }
    default:
     assert(false);
  }
}

} // namespace KLT::MDCG

} // namespace KLT

