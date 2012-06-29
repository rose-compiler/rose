
#ifndef __OPENCL_GENERATOR_HPP__
#define __OPENCL_GENERATOR_HPP__

#include "common/spmd-generator.hpp"
#include "common/array-analysis.hpp"
#include "compute-systems/compute-node.hpp"
#include "compute-systems/gpu-system.hpp"

// OpenCL buffer: Accessed as a flatenned array
class OpenCL_Alias : public ArrayAlias {
  protected:
    SgInitializedName * init_name;
    SgInitializedName * kernel_param;

  public:
    OpenCL_Alias(ArrayPartition * original_array_, GPU * gpu, SgScopeStatement * scope, bool read_and_write);
    virtual ~OpenCL_Alias();

    virtual SgPntrArrRefExp * propagateArr(SgPntrArrRefExp * arr_ref) const;
    virtual SgVarRefExp * propagateVar(SgVarRefExp * var_ref) const;
    virtual SgInitializedName * getInitName() const;

    SgInitializedName * getKernelParam() const;

  static SgType * buffer_type;
};

class IdentityAlias : public ArrayAlias {
  public:
    IdentityAlias(ArrayPartition * original_array_);
    virtual ~IdentityAlias();

    virtual SgPntrArrRefExp * propagateArr(SgPntrArrRefExp * arr_ref) const;
    virtual SgVarRefExp * propagateVar(SgVarRefExp * var_ref) const;
    virtual SgInitializedName * getInitName() const;
};

class OpenCL_Generator : public SPMD_Generator {
  protected:
    std::string ocl_file_name;
    std::map<SPMD_KernelCall *, std::vector<ArrayPartition *> > kernel_data_param;

  protected:
    virtual SgSourceFile * buildKernelFile(std::string);
    virtual ArrayAlias * genAlias(ArrayPartition * array_partition, ComputeSystem * compute_system, bool read_and_write = true);
    virtual SgStatement * codeGeneration(SPMD_KernelCall * tree);
    virtual SgStatement * codeGeneration(SPMD_Comm * tree);
    virtual SgStatement * codeGeneration(SPMD_Sync * tree);
    virtual void insertInit(
      SPMD_Tree * root_tree,
      std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased,
      SgStatement * insert_init_after,
      std::string kernel_file_name
    );
    virtual void insertFinal(SPMD_Tree * root_tree, SgStatement * insert_final_after);
    virtual void generateKernel(SPMD_Tree * root_tree, SgSourceFile * kernel_file);

  public:
    OpenCL_Generator(SPMD_Driver & driver_);
    virtual ~OpenCL_Generator();
};

#endif /* __OPENCL_GENERATOR_HPP__ */

