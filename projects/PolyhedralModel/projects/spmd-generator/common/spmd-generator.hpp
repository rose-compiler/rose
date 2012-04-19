
#ifndef __SPMD_GENERATOR_HPP__
#define __SPMD_GENERATOR_HPP__

#include <set>
#include <string>

class SgStatement;

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

class SPMD_Generator {
  protected:
    SPMD_Driver & driver;

  protected:
    SgStatement * codeGeneration(SPMD_Tree * tree);
    SgStatement * codeGeneration(SPMD_Root * tree);
    SgStatement * codeGeneration(SPMD_NativeStmt * tree);
    SgStatement * codeGeneration(SPMD_Loop * tree);
    SgStatement * codeGeneration(SPMD_DomainRestriction * tree);

  // Target model dependent methods
    virtual SgStatement * codeGeneration(SPMD_KernelCall * tree) = 0;
    virtual SgStatement * codeGeneration(SPMD_Comm * tree) = 0;
    virtual SgStatement * codeGeneration(SPMD_Sync * tree) = 0;
    virtual void insertInit(SPMD_Tree * root_tree, std::set<ArrayPartition *> & init_comm, SgStatement * insert_init_after) = 0;
    virtual void insertFinal(SPMD_Tree * root_tree, std::set<ArrayPartition *> & final_comm, SgStatement * insert_final_after) = 0;
    virtual void generateKernel(SPMD_Tree * root_tree, std::string filename) = 0;

  public:
    SPMD_Generator(SPMD_Driver & driver_);

    SgStatement * generate(
      SgStatement * insert_init_after,
      SgStatement * insert_final_after,
      SgStatement * first,
      SgStatement * last,
      std::string filename_for_kernels
    );
};

#endif /* __SPMD_GENERATOR_HPP__ */

