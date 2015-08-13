
#ifndef __KLT_TILEK_OPENCL_HPP__
#define __KLT_TILEK_OPENCL_HPP__

#include "KLT/TileK/api-accelerator.hpp"

namespace KLT {

namespace TileK {

struct opencl_kernel_t : public accelerator_kernel_t {
  protected:
    SgFunctionSymbol * get_group_id_func;
    SgFunctionSymbol * get_local_id_func;

  public:
    virtual SgInitializedName * buildGlobalVariable(const std::string & name, SgType * type, SgInitializer * init) const;
    virtual SgInitializedName * buildLocalVariable(const std::string & name, SgType * type, SgInitializer * init) const;
    virtual SgInitializedName * buildConstantVariable(const std::string & name, SgType * type, SgInitializer * init) const;

    virtual void applyKernelModifiers(SgFunctionDeclaration * kernel_decl) const;

    virtual void loadUser(const ::MDCG::Model::model_t & model);

    virtual SgExpression * buildGangsTileIdx(size_t lvl) const;
    virtual SgExpression * buildWorkersTileIdx(size_t lvl) const;
};

} // namespace KLT::TileK

} // namespace KLT

#endif /* __KLT_TILEK_OPENCL_HPP__ */

