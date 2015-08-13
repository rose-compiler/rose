
#ifndef __KLT_TILEK_THREADS_HPP__
#define __KLT_TILEK_THREADS_HPP__

#include "KLT/TileK/api-basic.hpp"

namespace KLT {

namespace TileK {

struct threads_host_t : public ::KLT::API::host_t {
  protected:
    SgVariableSymbol * num_threads_field;

  public:
    virtual void loadUser(const ::MDCG::Model::model_t & model);

    SgStatement * buildNumThreadsAssign(SgVariableSymbol * kernel_sym, SgExpression * rhs) const;
};

struct threads_call_interface_t : public basic_call_interface_t {
  protected:
    SgVariableSymbol * tid_symbol;

  public:
    threads_call_interface_t(::MFB::Driver< ::MFB::Sage> & driver, ::KLT::API::kernel_t * kernel_api);

  public:
    virtual void prependUserArguments(SgFunctionParameterList * param_list) const;

    virtual void getSymbolForUserArguments(SgFunctionDefinition * kernel_defn, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb);

    virtual SgExpression * getTileIdx(const Descriptor::tile_t & tile) const;
};

} // namespace KLT::TileK

} // namespace KLT

#endif /* __KLT_TILEK_THREADS_HPP__ */

