
#include "sage3basic.h"

#include "KLT/TileK/api-accelerator.hpp"

#if !defined(VERBOSE)
#  define VERBOSE 0
#endif

namespace KLT {

namespace TileK {

void accelerator_host_t::loadUser(const ::MDCG::Model::model_t & model) {
  bool res = true;

  ::MDCG::Model::class_t class_;
  ::MDCG::Model::field_t field_;
  SgClassSymbol * kernel_config_class;

  res = api_t::load(class_   , kernel_config_class , model, "klt_user_config_t" , NULL);   assert(res == true);
    res = api_t::load(field_ , num_gangs_field     , model,   "num_gangs"       , class_); assert(res == true);
    res = api_t::load(field_ , num_workers_field   , model,   "num_workers"     , class_); assert(res == true);
}

SgStatement * accelerator_host_t::buildNumGangsAssign(SgVariableSymbol * kernel_sym, size_t gang_id, SgExpression * rhs) const {
  return SageBuilder::buildAssignStatement(
           SageBuilder::buildPntrArrRefExp(
             SageBuilder::buildArrowExp(
               SageBuilder::buildArrowExp(
                 SageBuilder::buildVarRefExp(kernel_sym),
                 SageBuilder::buildVarRefExp(kernel_config_field)
               ),
               SageBuilder::buildVarRefExp(num_gangs_field)
             ),
             SageBuilder::buildIntVal(gang_id)
           ), rhs
         );
}

SgStatement * accelerator_host_t::buildNumWorkersAssign(SgVariableSymbol * kernel_sym, size_t worker_id, SgExpression * rhs) const {
  return SageBuilder::buildAssignStatement(
           SageBuilder::buildPntrArrRefExp(
             SageBuilder::buildArrowExp(
               SageBuilder::buildArrowExp(
                 SageBuilder::buildVarRefExp(kernel_sym),
                 SageBuilder::buildVarRefExp(kernel_config_field)
               ),
               SageBuilder::buildVarRefExp(num_workers_field)
             ),
             SageBuilder::buildIntVal(worker_id)
           ), rhs
         );
}

accelerator_call_interface_t::accelerator_call_interface_t(::MFB::Driver< ::MFB::Sage> & driver, ::KLT::API::kernel_t * kernel_api) : ::KLT::API::call_interface_t(driver, kernel_api) {}

void accelerator_call_interface_t::addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const {
  std::vector<SgVariableSymbol *>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++) {
    SgVariableSymbol * param_sym = *it;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type = param_sym->get_type();
    param_list->append_arg(SageBuilder::buildInitializedName(param_name, param_type, NULL));
  }
}

void accelerator_call_interface_t::addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<Descriptor::data_t *> & data) const {
  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++) {
    SgVariableSymbol * data_sym = (*it)->symbol;
    std::string data_name = data_sym->get_name().getString();
    SgType * data_type = SageBuilder::buildPointerType((*it)->base_type);
    param_list->append_arg(kernel_api->buildGlobalVariable(data_name, data_type, NULL));
  }
}

void accelerator_call_interface_t::getSymbolForParameter(SgFunctionDefinition * kernel_defn, const std::vector<SgVariableSymbol *> & parameters, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const {
  std::vector<SgVariableSymbol *>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++) {
    SgVariableSymbol * param_sym = *it;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type = param_sym->get_type();

    driver.useType(param_type, kernel_defn);

    SgVariableSymbol * new_sym = kernel_defn->lookup_variable_symbol(param_name);
    assert(new_sym != NULL);

    symbol_map.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, new_sym));
  }
}

void accelerator_call_interface_t::getSymbolForData(SgFunctionDefinition * kernel_defn, const std::vector<Descriptor::data_t *> & data, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const {
  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++) {
    SgVariableSymbol * data_sym = (*it)->symbol;
    std::string data_name = data_sym->get_name().getString();
    SgType * data_type = (*it)->base_type;

    driver.useType(data_type, kernel_defn);

    SgVariableSymbol * new_sym = kernel_defn->lookup_variable_symbol(data_name);
    assert(new_sym != NULL);

    symbol_map.data.insert(std::pair<SgVariableSymbol *, Descriptor::data_t *>(data_sym, *it));
    symbol_map.data_trans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, new_sym));
    symbol_map.data_rtrans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(new_sym, data_sym));
  }
}

SgExpression * accelerator_call_interface_t::getTileIdx(const Descriptor::tile_t & tile) const {
      accelerator_kernel_t * api = dynamic_cast<accelerator_kernel_t *>(kernel_api);
      assert(api != NULL);

#if VERBOSE
  std::cerr << "[Info] (accelerator_call_interface_t::getTileIdx) kind=" << tile.kind << ", param=" << tile.param;
  if (tile.param != NULL) {
    std::cerr << " : \"" << tile.param->unparseToString() << "\" (" << tile.param->class_name() << ")";
  }
  std::cerr << std::endl;
#endif

  SgIntVal * param = isSgIntVal(tile.param);
  assert(param != NULL);

  int lvl = param->get_value();
  assert(lvl >= 0 && lvl <= 2);

  switch ((unsigned long)tile.kind) {
    case 2: return api->buildGangsTileIdx(lvl);
    case 3: return api->buildWorkersTileIdx(lvl);
    default: assert(false);
  }
}

} // namespace KLT::TileK

} // namespace KLT

