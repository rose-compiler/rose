
#include "sage3basic.h"

#include "KLT/TileK/api-threads.hpp"

namespace KLT {

namespace TileK {

void threads_host_t::loadUser(const ::MDCG::Model::model_t & model) {
  bool res = true;

  ::MDCG::Model::class_t class_;
  ::MDCG::Model::field_t field_;
  SgClassSymbol * kernel_config_class;

  res = api_t::load(class_   , kernel_config_class , model, "klt_user_config_t" , NULL);   assert(res == true);
    res = api_t::load(field_ , num_threads_field   , model,   "num_threads"     , class_); assert(res == true);
}

SgStatement * threads_host_t::buildNumThreadsAssign(SgVariableSymbol * kernel_sym, SgExpression * rhs) const {
  return SageBuilder::buildAssignStatement(
           SageBuilder::buildArrowExp(
             SageBuilder::buildArrowExp(
               SageBuilder::buildVarRefExp(kernel_sym),
               SageBuilder::buildVarRefExp(kernel_config_field)
             ),
             SageBuilder::buildVarRefExp(num_threads_field)
           ), rhs
         );
}

threads_call_interface_t::threads_call_interface_t(::MFB::Driver< ::MFB::Sage> & driver, ::KLT::API::kernel_t * kernel_api) : basic_call_interface_t(driver, kernel_api), tid_symbol(NULL) {}

void threads_call_interface_t::prependUserArguments(SgFunctionParameterList * param_list) const {
  param_list->append_arg(SageBuilder::buildInitializedName("tid", SageBuilder::buildIntType(), NULL));
}

void threads_call_interface_t::getSymbolForUserArguments(SgFunctionDefinition * kernel_defn, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) {
  tid_symbol = kernel_defn->lookup_variable_symbol("tid");
  assert(tid_symbol != NULL);
}

SgExpression * threads_call_interface_t::getTileIdx(const Descriptor::tile_t & tile) const {
  assert(tid_symbol != NULL);
  return SageBuilder::buildVarRefExp(tid_symbol);
}

} // namespace KLT::TileK

} // namespace KLT

