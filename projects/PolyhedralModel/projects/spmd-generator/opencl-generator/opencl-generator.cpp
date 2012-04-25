
#include "opencl-generator/opencl-generator.hpp"
#include "common/spmd-driver.hpp"
#include "common/spmd-tree.hpp"
#include "common/placement.hpp"
#include "compute-systems/gpu-system.hpp"

#include <iostream>
#include <sstream>

#include "rose.h"

class Domain;

OpenCL_Alias::OpenCL_Alias(ArrayPartition * original_array_, GPU * gpu) :
  ArrayAlias(original_array_),
  init_name(NULL)
{
  ComputeNode * compute_node = dynamic_cast<ComputeNode *>(gpu->getParent());
  assert(compute_node != NULL);

  std::ostringstream oss;
  oss << "dev_" << original_array->getUniqueName() << "_" << compute_node->getAcceleratorID(gpu);

  init_name = SageBuilder::buildInitializedName(oss.str(), original_array->getType());
}

OpenCL_Alias::~OpenCL_Alias() {}

SgPntrArrRefExp * OpenCL_Alias::propagate(SgPntrArrRefExp * arr_ref) const {
  // TODO
}

SgVarRefExp * OpenCL_Alias::propagate(SgVarRefExp * var_ref) const {
  // TODO
}

SgInitializedName * OpenCL_Alias::getInitName() const { return init_name; }

IdentityAlias::IdentityAlias(ArrayPartition * original_array_) :
  ArrayAlias(original_array_)
{}

IdentityAlias::~IdentityAlias() {}

SgPntrArrRefExp * IdentityAlias::propagate(SgPntrArrRefExp * arr_ref) const { return arr_ref; }
SgVarRefExp * IdentityAlias::propagate(SgVarRefExp * var_ref) const { return var_ref; }

SgStatement * OpenCL_Generator::codeGeneration(SPMD_KernelCall * tree) {
  // Kernel name
  std::ostringstream oss_kernel_name;
  oss_kernel_name << "ocl_kernel_" << tree->getID();
  std::string kernel_name = oss_kernel_name.str();  

  // Data to passed as argument
  std::set<ArrayPartition *> * data_in    = driver.getArrayAnalysis().get_in(tree);
  std::set<ArrayPartition *> * data_out   = driver.getArrayAnalysis().get_out(tree);
  std::set<ArrayPartition *> * data_inout = driver.getArrayAnalysis().get_inout(tree);
  assert(data_in != NULL && data_out != NULL && data_inout != NULL);
  std::set<ArrayPartition *> datas;
    datas.insert(data_in->begin(), data_in->end());
    datas.insert(data_out->begin(), data_out->end());
    datas.insert(data_inout->begin(), data_inout->end());
  delete data_in;
  delete data_out;
  delete data_inout;

  // Placement which part of the kernel on which GPU (FIXME extremely restrictive now: only one global placement)
  std::vector<std::pair<ComputeSystem *, Domain *> > * placement = driver.getPlacement().assigned(tree);
  assert(placement != NULL && placement->size() == 1 && (*placement)[0].second == NULL);
  ComputeSystem * compute_system = (*placement)[0].first;
  GPU * gpu = dynamic_cast<GPU *>(compute_system);
  assert(gpu != NULL);
  ComputeNode * compute_node = dynamic_cast<ComputeNode *>(gpu->getParent());
  assert(compute_node != NULL);

  // Queue name
  unsigned gpu_id = compute_node->getAcceleratorID(gpu);
  std::ostringstream oss_queue_name;
  oss_queue_name << "ocl_queue_" << gpu_id;
  std::string queue_name = oss_queue_name.str();

  // Some Useful objects for the code generation
  assert(top_scope != NULL);
  SgScopeStatement * scope = SageBuilder::topScopeStack();
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();

  SgName size_t_name = "size_t";
  SgTypedefSymbol * size_type_symbol = SageInterface::lookupTypedefSymbolInParentScopes(size_t_name, top_scope);
  ROSE_ASSERT(size_type_symbol != NULL);
  SgType * size_type = size_type_symbol->get_type();
  ROSE_ASSERT(size_type != NULL);

  // surrounding iterators
  std::vector<SgInitializedName *> ordered_iterators;
  // TODO

  // work size
  SgVariableDeclaration * work_size_decl = NULL;
  unsigned nbr_work_dim = 0;
  {
    std::vector<SgExpression *> * expr_vect = tree->generateDimensionSizes();
    SgType * type = SageBuilder::buildArrayType(size_type, SageBuilder::buildIntVal(expr_vect->size()));
    SgExprListExp * expr_list = SageBuilder::buildExprListExp(*expr_vect);
    SgAggregateInitializer * init = SageBuilder::buildAggregateInitializer(expr_list, type);
    work_size_decl = SageBuilder::buildVariableDeclaration("work_size", type, init, scope);
    nbr_work_dim = expr_vect->size();
    delete expr_vect;
  }
  bb->append_statement(work_size_decl);

  // Instantiate the kernel
  {
    SgVarRefExp * kernel_obj = SageBuilder::buildVarRefExp(kernel_name, top_scope);
    {
      // Obtains the SgType for "cl_kernel"
      SgName kernel_type_name = "cl_kernel";
      SgTypedefSymbol * sym = top_scope->lookup_typedef_symbol(kernel_type_name);
      ROSE_ASSERT(sym != NULL);
      SgType * kernel_type = sym->get_type();
      ROSE_ASSERT(kernel_type != NULL);

      // create the function call
      SgName create_kernel = "createKernel";
      std::vector<SgExpression *> param_list;
      {
        param_list.push_back(SageBuilder::buildVarRefExp("program", top_scope)); // FIXME isn't the program associate to one GPU?
        param_list.push_back(SageBuilder::buildStringVal(kernel_name));
      }
      SgExprListExp * params = SageBuilder::buildExprListExp(param_list);
      SgFunctionCallExp * create_kernel_call = SageBuilder::buildFunctionCallExp(create_kernel, kernel_type, params, top_scope);
      SgStatement * kernel_creation = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(kernel_obj, create_kernel_call));
      bb->append_statement(kernel_creation);
    }

    {
      unsigned arg_cnt = 0;
      {
        SgName set_kernel_arg_value = "setKernelArgToValue";
        SgName set_kernel_arg_buffer = "setKernelArgToBuffer";

        // Iterators
        SgExpression * size = SageBuilder::buildSizeOfOp(SageBuilder::buildIntType());
        for (int i = 0; i < ordered_iterators.size(); i++) {
          SgExprListExp * params = SageBuilder::buildExprListExp(
              kernel_obj, SageBuilder::buildIntVal(arg_cnt), SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(ordered_iterators[i])), size
          );
          SgFunctionCallExp * set_kernel_arg_value_call = SageBuilder::buildFunctionCallExp(
              set_kernel_arg_value, SageBuilder::buildVoidType(), params, top_scope
          );
          bb->append_statement(SageBuilder::buildExprStatement(set_kernel_arg_value_call));
          arg_cnt++;
        }

        // Data
        std::set<ArrayPartition *>::iterator it_data;
        for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
          ArrayPartition * array_partition = *it_data;
          ArrayAlias * alias = genAlias(array_partition, gpu);
          OpenCL_Alias * ocl_alias = dynamic_cast<OpenCL_Alias *>(alias);
          assert(ocl_alias != NULL);

          const std::vector<unsigned> & dimensions = array_partition->getDimensions();
          if (dimensions.size() == 0) { // scalar
            SgExprListExp * params = SageBuilder::buildExprListExp(
                kernel_obj,
                SageBuilder::buildIntVal(arg_cnt),
                SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(ocl_alias->getInitName(), top_scope)),
                SageBuilder::buildSizeOfOp(array_partition->getType())
            );
            SgFunctionCallExp * set_kernel_arg_value_call = SageBuilder::buildFunctionCallExp(
                set_kernel_arg_value, SageBuilder::buildVoidType(), params, top_scope
            );
            bb->append_statement(SageBuilder::buildExprStatement(set_kernel_arg_value_call));
            arg_cnt++;
          }
          else { // array
            SgExprListExp * params = SageBuilder::buildExprListExp(
                kernel_obj, SageBuilder::buildIntVal(arg_cnt), SageBuilder::buildVarRefExp(ocl_alias->getInitName())
            );
            SgFunctionCallExp * set_kernel_arg_buffer_call = SageBuilder::buildFunctionCallExp(
                set_kernel_arg_buffer, SageBuilder::buildVoidType(), params, top_scope
            );
            bb->append_statement(SageBuilder::buildExprStatement(set_kernel_arg_buffer_call));
            arg_cnt++;
          }
        }
      }
    }
  }

  // Call to the enqueue function
  SgStatement * call_stmt = NULL;
  {
    SgName nd_range = "enqueueKernel";
    std::vector<SgExpression *> param_list;
    {
      param_list.push_back(SageBuilder::buildVarRefExp(queue_name, top_scope)); // queue
      param_list.push_back(SageBuilder::buildVarRefExp(kernel_name, top_scope)); // kernel
      param_list.push_back(SageBuilder::buildIntVal(nbr_work_dim)); // work_dim
      param_list.push_back(SageBuilder::buildVarRefExp(work_size_decl)); // global_work_size
      param_list.push_back(SageBuilder::buildIntVal(0)); // local_work_size
    }
    SgExprListExp * params = SageBuilder::buildExprListExp(param_list);
    SgFunctionCallExp * nd_range_call = SageBuilder::buildFunctionCallExp(nd_range, SageBuilder::buildVoidType(), params, top_scope);
    call_stmt = SageBuilder::buildExprStatement(nd_range_call);
  }

  return bb;
}

SgStatement * OpenCL_Generator::codeGeneration(SPMD_Comm * tree) {
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();

  // TODO

  return bb;
}

SgStatement * OpenCL_Generator::codeGeneration(SPMD_Sync * tree) {
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();

  // TODO

  return bb;
}

ArrayAlias * OpenCL_Generator::genAlias(ArrayPartition * array_partition, ComputeSystem * compute_system) {
  SageBuilder::pushScopeStack(top_scope);
  std::map<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> >::iterator it_cs = array_aliases.find(compute_system);
  if (it_cs == array_aliases.end()) {
    it_cs = array_aliases.insert(
      std::pair<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> >(compute_system, std::map<ArrayPartition *, ArrayAlias *>())
    ).first;
  }
  std::map<ArrayPartition *, ArrayAlias *>::iterator it_ap = it_cs->second.find(array_partition);
  if (it_ap == it_cs->second.end()) {
    GPU * gpu = dynamic_cast<GPU *>(compute_system);
    if (gpu != NULL)
      it_ap = it_cs->second.insert(
        std::pair<ArrayPartition *, ArrayAlias *>(array_partition, new OpenCL_Alias(array_partition, gpu))
      ).first;
    else
      it_ap = it_cs->second.insert(
        std::pair<ArrayPartition *, ArrayAlias *>(array_partition, new IdentityAlias(array_partition))
      ).first;
  }
  SageBuilder::popScopeStack();
  return it_ap->second;
}

void OpenCL_Generator::insertInit(
  SPMD_Tree * root_tree,
  std::map<ComputeSystem *, std::set<ArrayPartition *> > & to_be_aliased,
  SgStatement * insert_init_after,
  std::string kernel_file_name
) {
  SgScopeStatement * scope = top_scope;

  unsigned nbr_gpu = 0;
  {
    std::map<ComputeSystem *, std::set<ArrayPartition *> >::iterator it;
    for (it = to_be_aliased.begin(); it != to_be_aliased.end(); it++)
      if (dynamic_cast<GPU *>(it->first) != NULL)
        nbr_gpu++;
    assert(nbr_gpu == 1); // FIXME will have to modify OpenCL wrapper for this...
  }

  SageInterface::attachComment(insert_init_after, "Declaration and instantiation of basic OpenCL objects", PreprocessingInfo::after);

  SgVariableDeclaration * context_decl = NULL;
  {
    SgTypedefDeclaration * context_type_decl = SageBuilder::buildTypedefDeclaration("cl_context", SageBuilder::buildIntType(), scope);
    SgType * context_type = SgTypedefType::createType(context_type_decl);
    context_decl = SageBuilder::buildVariableDeclaration("context", context_type, NULL, scope);
    SageInterface::insertStatementAfter(insert_init_after, context_decl, false);
    insert_init_after = context_decl;
  }

  SgVariableDeclaration * program_decl = NULL;
  {
    SgTypedefDeclaration * program_type_decl = SageBuilder::buildTypedefDeclaration("cl_program", SageBuilder::buildIntType(), scope);
    SgType * program_type = SgTypedefType::createType(program_type_decl);
    program_decl = SageBuilder::buildVariableDeclaration("program", program_type, NULL, scope);
    SageInterface::insertStatementAfter(insert_init_after, program_decl, false);
    insert_init_after = program_decl;
  }

  SgVariableDeclaration * queue_decl = NULL;
  {
    SgTypedefDeclaration * queue_type_decl = SageBuilder::buildTypedefDeclaration("cl_command_queue", SageBuilder::buildIntType(), scope);
    SgType * queue_type = SgTypedefType::createType(queue_type_decl);
    queue_decl = SageBuilder::buildVariableDeclaration("queue_0", queue_type, NULL, scope);
    SageInterface::insertStatementAfter(insert_init_after, queue_decl, false);
    insert_init_after = queue_decl;
  }

  {
    SgName init_opencl = "initOpenCL";
    SgExprListExp * params = SageBuilder::buildExprListExp(
        SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(context_decl)),
        SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(queue_decl)),
        SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(program_decl)),
        SageBuilder::buildStringVal(kernel_file_name)
    );
    SgFunctionCallExp * init_opencl_call = SageBuilder::buildFunctionCallExp(init_opencl, SageBuilder::buildVoidType(), params, scope);
    SgExprStatement * expr_stmt = SageBuilder::buildExprStatement(init_opencl_call);
    SageInterface::insertStatementAfter(insert_init_after, expr_stmt, false);
    insert_init_after = expr_stmt;
  }

  SageInterface::attachComment(insert_init_after, "Creation of the buffers on the device.", PreprocessingInfo::after);

  {
    std::map<ComputeSystem *, std::set<ArrayPartition *> >::iterator it1;
    std::set<ArrayPartition *>::iterator it2;
    for (it1 = to_be_aliased.begin(); it1 != to_be_aliased.end(); it1++)
      for (it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
        OpenCL_Alias * ocl_alias = dynamic_cast<OpenCL_Alias *>(genAlias(*it2, it1->first));
        if (ocl_alias != NULL) {
          SgInitializedName * init_name = ocl_alias->getInitName();

          // TODO build initialization

          SgVariableDeclaration * var_decl = new SgVariableDeclaration(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
          var_decl->get_variables().push_back(init_name);
          SageInterface::insertStatementAfter(insert_init_after, var_decl);
          insert_init_after = var_decl;
        }
      }
  }

  SageInterface::attachComment(insert_init_after, "Declaration of the kernels objects", PreprocessingInfo::after);

  // TODO kernels objects

  // Build additionnal OpenCL API types
  {
    SgTypedefDeclaration * kernel_type_decl = SageBuilder::buildTypedefDeclaration("cl_kernel", SageBuilder::buildIntType(), scope);
  }
}

void OpenCL_Generator::insertFinal(SPMD_Tree * root_tree, SgStatement * insert_final_after) {
  // TODO
}

void OpenCL_Generator::generateKernel(SPMD_Tree * root_tree, SgSourceFile * kernel_file) {
  // TODO
}

OpenCL_Generator::OpenCL_Generator(SPMD_Driver & driver_) :
  SPMD_Generator(driver_)
{
  assert(driver.hasArrayAnalysis() && driver.hasPlacement());
}

OpenCL_Generator::~OpenCL_Generator() {}

SgSourceFile * OpenCL_Generator::buildKernelFile(std::string kernel_file_name) {
  assert(false); // TODO
  return NULL;
}

