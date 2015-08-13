
#ifndef __KLT_TILEK_ACCELERATOR_HPP__
#define __KLT_TILEK_ACCELERATOR_HPP__

#include "KLT/Core/api.hpp"
#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/utils.hpp"

namespace KLT {

namespace TileK {

struct accelerator_host_t : public ::KLT::API::host_t {
  protected:
    SgVariableSymbol * num_gangs_field;
    SgVariableSymbol * num_workers_field;

  public:
    virtual void loadUser(const ::MDCG::Model::model_t & model);

    SgStatement * buildNumGangsAssign(SgVariableSymbol * kernel_sym, size_t gang_id, SgExpression * rhs) const;
    SgStatement * buildNumWorkersAssign(SgVariableSymbol * kernel_sym, size_t worker_id, SgExpression * rhs) const;
};

struct accelerator_kernel_t : public ::KLT::API::kernel_t {
  public:
    virtual SgExpression * buildGangsTileIdx(size_t lvl) const = 0;
    virtual SgExpression * buildWorkersTileIdx(size_t lvl) const = 0;
};

struct accelerator_call_interface_t : public ::KLT::API::call_interface_t {
  public:
    accelerator_call_interface_t(::MFB::Driver< ::MFB::Sage> & driver, ::KLT::API::kernel_t * kernel_api);

  public:
    virtual void addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const;
    virtual void addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<Descriptor::data_t *> & data) const;

    virtual void getSymbolForParameter(SgFunctionDefinition * kernel_defn, const std::vector<SgVariableSymbol *> & parameters, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;
    virtual void getSymbolForData(SgFunctionDefinition * kernel_defn, const std::vector<Descriptor::data_t *> & data, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;

    virtual SgExpression * getTileIdx(const Descriptor::tile_t & tile) const;
};

} // namespace KLT::TileK

} // namespace KLT

#endif /* __KLT_TILEK_ACCELERATOR_HPP__ */

