
#include "sage3basic.h"

#include "KLT/TileK/api-basic.hpp"

namespace KLT {

namespace TileK {

basic_call_interface_t::basic_call_interface_t(::MFB::Driver< ::MFB::Sage> & driver, ::KLT::API::kernel_t * kernel_api) : ::KLT::API::call_interface_t(driver, kernel_api) {}

void basic_call_interface_t::addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const {
  param_list->append_arg(SageBuilder::buildInitializedName("param", SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));
}

void basic_call_interface_t::addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<Descriptor::data_t *> & data) const {
  param_list->append_arg(SageBuilder::buildInitializedName("data", SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));
}

void basic_call_interface_t::getSymbolForParameter(SgFunctionDefinition * kernel_defn, const std::vector<SgVariableSymbol *> & parameters, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const {
  SgVariableSymbol * arg_param_sym = kernel_defn->lookup_variable_symbol("param");
  assert(arg_param_sym != NULL);
  int cnt = 0;

  std::vector<SgVariableSymbol *>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++) {
    SgVariableSymbol * param_sym = *it;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type = param_sym->get_type();

    driver.useType(param_type, kernel_defn);

    SgExpression * init = SageBuilder::buildPointerDerefExp(SageBuilder::buildCastExp(
                              SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_param_sym), SageBuilder::buildIntVal(cnt++)),
                              SageBuilder::buildPointerType(param_type)
                          ));
    SageInterface::prependStatement(SageBuilder::buildVariableDeclaration(param_name, param_type, SageBuilder::buildAssignInitializer(init), bb), bb);

    SgVariableSymbol * new_sym = bb->lookup_variable_symbol(param_name);
    assert(new_sym != NULL);

    symbol_map.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, new_sym));
  }
}

void basic_call_interface_t::getSymbolForData(SgFunctionDefinition * kernel_defn, const std::vector<Descriptor::data_t *> & data, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const {
  SgVariableSymbol * arg_data_sym = kernel_defn->lookup_variable_symbol("data");
  assert(arg_data_sym != NULL);
  int cnt = 0;

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++) {
    SgVariableSymbol * data_sym = (*it)->symbol;
    std::string data_name = data_sym->get_name().getString();
    SgType * data_type = (*it)->base_type;

    driver.useType(data_type, kernel_defn);

    data_type = SageBuilder::buildPointerType(data_type);

    SgExpression * init = SageBuilder::buildCastExp(
                            SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_data_sym), SageBuilder::buildIntVal(cnt++)), data_type
                          );
    SageInterface::prependStatement(SageBuilder::buildVariableDeclaration(data_name, data_type, SageBuilder::buildAssignInitializer(init), bb), bb);

    SgVariableSymbol * new_sym = bb->lookup_variable_symbol(data_name);
    assert(new_sym != NULL);

    symbol_map.data.insert(std::pair<SgVariableSymbol *, Descriptor::data_t *>(data_sym, *it));
    symbol_map.data_trans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, new_sym));
    symbol_map.data_rtrans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(new_sym, data_sym));
  }
}

} // namespace KLT::TileK

} // namespace KLT

