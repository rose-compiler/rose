
#ifndef __SPMD_GENERATOR_HPP__
#define __SPMD_GENERATOR_HPP__

#include <set>
#include <map>
#include <string>

class SgStatement;
class SgScopeStatement;
class SgSourceFile;

class SPMD_Driver;

class SPMD_Tree;
class SPMD_NativeStmt;
class SPMD_Loop;
class SPMD_DomainRestriction;
class SPMD_KernelCall;
class SPMD_Comm;
class SPMD_Sync;
class SPMD_Root;

class ArrayPartition;
class ComputeSystem;
class ArrayAlias;

class SPMD_Generator {
  protected:
    SPMD_Driver & driver;
    SgScopeStatement * top_scope;

    std::map<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> > array_aliases;

  protected:
    SgStatement * codeGeneration(SPMD_Tree * tree);
    SgStatement * codeGeneration(SPMD_Root * tree);
    SgStatement * codeGeneration(SPMD_NativeStmt * tree);
    SgStatement * codeGeneration(SPMD_Loop * tree);
    SgStatement * codeGeneration(SPMD_DomainRestriction * tree);

  // Target model dependent methods
    virtual SgSourceFile * buildKernelFile(std::string kernel_file_name) = 0;

    virtual ArrayAlias * genAlias(ArrayPartition * array_partition, ComputeSystem * compute_system, bool read_and_write) = 0;

    virtual SgStatement * codeGeneration(SPMD_KernelCall * tree) = 0;
    virtual SgStatement * codeGeneration(SPMD_Comm * tree) = 0;
    virtual SgStatement * codeGeneration(SPMD_Sync * tree) = 0;

    virtual void insertInit(
      SPMD_Tree * root_tree,
      std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased,
      SgStatement * insert_init_after,
      std::string kernel_file_name
    ) = 0;
    virtual void insertFinal(SPMD_Tree * root_tree, SgStatement * insert_final_after) = 0;
    virtual void generateKernel(SPMD_Tree * root_tree, SgSourceFile * kernel_file) = 0;

  public:
    SPMD_Generator(SPMD_Driver & driver_);
    virtual ~SPMD_Generator();

    SgStatement * generate(
      SgStatement * insert_init_after,
      SgStatement * insert_final_after,
      SgStatement * first,
      SgStatement * last,
      SgScopeStatement * top_scope_ = NULL,
      std::string kernel_file_name = std::string()
    );
};

#endif /* __SPMD_GENERATOR_HPP__ */

