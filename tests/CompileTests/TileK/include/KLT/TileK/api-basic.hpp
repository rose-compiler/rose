
#ifndef __KLT_TILEK_BASIC_HPP__
#define __KLT_TILEK_BASIC_HPP__

#include "KLT/Core/api.hpp"
#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/utils.hpp"

namespace KLT {

namespace TileK {

struct basic_call_interface_t : public ::KLT::API::call_interface_t {
  public:
    basic_call_interface_t(::MFB::Driver< ::MFB::Sage> & driver, ::KLT::API::kernel_t * kernel_api);

  protected:
    // adds one arguments for each parameter
    virtual void addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const;

    // adds one arguments for each data
    virtual void addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<Descriptor::data_t *> & data) const;

    // get parameters argument's symbol
    virtual void getSymbolForParameter(SgFunctionDefinition * kernel_defn, const std::vector<SgVariableSymbol *> & parameters, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;

    // get data argument's symbol
    virtual void getSymbolForData(SgFunctionDefinition * kernel_defn, const std::vector<Descriptor::data_t *> & data, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;
};

} // namespace KLT::TileK

} // namespace KLT

#endif /* __KLT_TILEK_BASIC_HPP__ */

