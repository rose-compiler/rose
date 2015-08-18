
#ifndef __KLT_TILEK_CUDA_HPP__
#define __KLT_TILEK_CUDA_HPP__

#include "KLT/TileK/api-accelerator.hpp"

#include <cstddef>

namespace KLT {

namespace TileK {

struct cuda_kernel_t : public accelerator_kernel_t {
  protected:
    SgVariableSymbol * thread_idx_var;
    SgVariableSymbol * block_idx_var;
    SgVariableSymbol * uint3_field[3];

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

#endif /* __KLT_TILEK_CUDA_HPP__ */

