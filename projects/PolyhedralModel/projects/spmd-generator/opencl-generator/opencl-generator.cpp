
#include "opencl-generator/opencl-generator.hpp"
#include "common/spmd-driver.hpp"
#include "common/spmd-tree.hpp"
#include "common/placement.hpp"
#include "common/comm-analysis.hpp"
#include "compute-systems/gpu-system.hpp"
#include "toolboxes/algebra-container.hpp"

#include <iostream>
#include <sstream>

#include "rose.h"

#define DEBUG 1

class Domain;

SgType * OpenCL_Alias::buffer_type = NULL;

OpenCL_Alias::OpenCL_Alias(ArrayPartition * original_array_, GPU * gpu, SgScopeStatement * scope, bool read_and_write) :
  ArrayAlias(original_array_),
  init_name(NULL),
  kernel_param(NULL)
{
  assert(scope != NULL);

  ComputeNode * compute_node = dynamic_cast<ComputeNode *>(gpu->getParent());
  assert(compute_node != NULL);

  std::ostringstream oss;
  oss << "dev_" << original_array->getUniqueName() << "_" << compute_node->getAcceleratorID(gpu);

  if (buffer_type == NULL) {
    SgTypedefDeclaration * buffer_type_decl = SageBuilder::buildTypedefDeclaration("cl_mem", SageBuilder::buildIntType(), scope);
    buffer_type = SgTypedefType::createType(buffer_type_decl);
  }

  SgName ro_buf = "createROBuffer";
  SgName rw_buf = "createRWBuffer";

  // FIXME if multiple context, find the one assiciate to 'compute_node->getAcceleratorID(gpu)'
  SgExprListExp * params = SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp("ocl_context", scope), original_array->getSize());
  SgFunctionCallExp * create_buffer_call = NULL;
  if (!read_and_write)
    create_buffer_call = SageBuilder::buildFunctionCallExp(ro_buf, buffer_type, params, scope);
  else
    create_buffer_call = SageBuilder::buildFunctionCallExp(rw_buf, buffer_type, params, scope);
  SgInitializer * init = SageBuilder::buildAssignInitializer(create_buffer_call);

  init_name = SageBuilder::buildInitializedName(oss.str(), buffer_type, init);

  SgType * param_type = NULL;
  if (original_array->getDimensions().size() == 0)
    param_type = original_array->getType();
  else {
    param_type = SageBuilder::buildPointerType(original_array->getType());
    param_type = SageBuilder::buildModifierType(param_type);
    ((SgModifierType *)param_type)->get_typeModifier().setOpenclGlobal();
  }

  kernel_param = SageBuilder::buildInitializedName(oss.str(), param_type);
}

OpenCL_Alias::~OpenCL_Alias() {}

SgInitializedName * OpenCL_Alias::getKernelParam() const { return kernel_param; }

SgPntrArrRefExp * OpenCL_Alias::propagateArr(SgPntrArrRefExp * arr_ref) const {
  std::vector<SgExpression *> subscripts;
  SgVarRefExp * var_ref_ = NULL;
  SgPntrArrRefExp * arr_ref_ = arr_ref;
  while (arr_ref_ != NULL) {
    subscripts.insert(subscripts.begin(), arr_ref_->get_rhs_operand_i());
    var_ref_ = isSgVarRefExp(arr_ref_->get_lhs_operand_i());
    arr_ref_ = isSgPntrArrRefExp(arr_ref_->get_lhs_operand_i());
  }
  assert(var_ref_ != NULL);

  if (var_ref_->get_symbol()->get_name() != original_array->getUniqueName())
    return arr_ref;

  assert(original_array->getDimensions().size() > 0);
  assert(subscripts.size() == original_array->getDimensions().size());

  SgVarRefExp * var_ref = SageBuilder::buildVarRefExp(kernel_param, SageBuilder::topScopeStack());
  SgExpression * subscript = SageInterface::copyExpression(subscripts[0]);
  for (unsigned i = 1; i < subscripts.size(); i++) {
    subscript = SageBuilder::buildAddOp(
        SageBuilder::buildMultiplyOp(
            SageInterface::copyExpression(subscript),
            SageBuilder::buildIntVal(original_array->getDimensions()[i])
        ),
        subscripts[i]
    );
  }
  return SageBuilder::buildPntrArrRefExp(var_ref, subscript);
}

SgVarRefExp * OpenCL_Alias::propagateVar(SgVarRefExp * var_ref) const {
  if (var_ref->get_symbol()->get_name() == original_array->getUniqueName())
    return SageBuilder::buildVarRefExp(kernel_param, SageBuilder::topScopeStack());
  else
    return var_ref;
}

SgInitializedName * OpenCL_Alias::getInitName() const { return init_name; }

IdentityAlias::IdentityAlias(ArrayPartition * original_array_) :
  ArrayAlias(original_array_)
{}

IdentityAlias::~IdentityAlias() {}

SgPntrArrRefExp * IdentityAlias::propagateArr(SgPntrArrRefExp * arr_ref) const { return arr_ref; }
SgVarRefExp * IdentityAlias::propagateVar(SgVarRefExp * var_ref) const { return var_ref; }
SgInitializedName * IdentityAlias::getInitName() const { return original_array->getOriginalVariable().getInitializedName(); }

SgStatement * OpenCL_Generator::codeGeneration(SPMD_KernelCall * tree) {
  // Kernel name
  std::ostringstream oss_kernel_name;
  oss_kernel_name << "ocl_kernel_" << tree->getID();
  std::string kernel_name = oss_kernel_name.str();  

  // Data to passed as argument
  std::set<ArrayPartition *> * datas = driver.getArrayAnalysis().get_partitions(tree);

  // Placement which part of the kernel on which GPU (FIXME extremely restrictive now: only one global placement)
  ComputeSystem * compute_system = driver.getPlacement().assigned(tree);
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
  SgType * size_type = NULL;
  if (size_type_symbol == NULL) {
    SgTypedefDeclaration * size_type_decl = SageBuilder::buildTypedefDeclaration("size_t", SageBuilder::buildIntType(), top_scope);
    size_type = SgTypedefType::createType(size_type_decl);
  }
  else
    size_type = size_type_symbol->get_type();
  ROSE_ASSERT(size_type != NULL);

  // surrounding iterators
  std::vector<RoseVariable> ordered_iterators;
  SPMD_Tree * parent = tree->getParent();
  while (dynamic_cast<SPMD_Root *>(parent) == NULL) {
    SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(parent);
    if (loop != NULL)
      ordered_iterators.insert(ordered_iterators.begin(), loop->getIterator());
    parent = parent->getParent();
  }

  // work size
  SgVariableDeclaration * work_size_decl = NULL;
  unsigned nbr_work_dim = 0;
  {
    std::vector<SgExpression *> * expr_vect = tree->generateDimensionSizes();
    SgType * type = SageBuilder::buildArrayType(size_type, SageBuilder::buildIntVal(expr_vect->size()));
    SgExprListExp * expr_list = SageBuilder::buildExprListExp(*expr_vect);
    SgAggregateInitializer * init = SageBuilder::buildAggregateInitializer(expr_list, type);
    work_size_decl = SageBuilder::buildVariableDeclaration(kernel_name + "_work_size", type, init, scope);
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
        param_list.push_back(SageBuilder::buildVarRefExp("ocl_program", top_scope)); // FIXME isn't the program associate to one GPU?
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
              kernel_obj,
              SageBuilder::buildIntVal(arg_cnt),
              SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(ordered_iterators[i].getInitializedName())),
              size
          );
          SgFunctionCallExp * set_kernel_arg_value_call = SageBuilder::buildFunctionCallExp(
              set_kernel_arg_value, SageBuilder::buildVoidType(), params, top_scope
          );
          bb->append_statement(SageBuilder::buildExprStatement(set_kernel_arg_value_call));
          arg_cnt++;
        }

        // Data
        std::map<SPMD_KernelCall *, std::vector<ArrayPartition *> >::iterator it_kernel_data_param = 
            kernel_data_param.insert(std::pair<SPMD_KernelCall *, std::vector<ArrayPartition *> >(
                tree, std::vector<ArrayPartition *>()
            )).first;
        std::vector<ArrayPartition *> & kernel_params = it_kernel_data_param->second;
        std::set<ArrayPartition *>::iterator it_data;
        for (it_data = datas->begin(); it_data != datas->end(); it_data++) {
          ArrayPartition * array_partition = *it_data;

          kernel_params.push_back(array_partition);

          const std::vector<unsigned> & dimensions = array_partition->getDimensions();
          if (dimensions.size() == 0) { // scalar
            SgExprListExp * params = SageBuilder::buildExprListExp(
                kernel_obj,
                SageBuilder::buildIntVal(arg_cnt),
//                SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(array_partition->getOriginalVariable().getInitializedName(), top_scope)),
                SageBuilder::buildAddressOfOp(array_partition->getOriginalVariable().generate(top_scope)),
                SageBuilder::buildSizeOfOp(array_partition->getType())
            );
            SgFunctionCallExp * set_kernel_arg_value_call = SageBuilder::buildFunctionCallExp(
                set_kernel_arg_value, SageBuilder::buildVoidType(), params, top_scope
            );
            bb->append_statement(SageBuilder::buildExprStatement(set_kernel_arg_value_call));
            arg_cnt++;
          }
          else { // array
            ArrayAlias * alias = genAlias(array_partition, gpu);
            OpenCL_Alias * ocl_alias = dynamic_cast<OpenCL_Alias *>(alias);
            assert(ocl_alias != NULL);
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
  bb->append_statement(call_stmt);

  delete datas;

  return bb;
}

SgStatement * OpenCL_Generator::codeGeneration(SPMD_Comm * tree) {
  // Scopes and Block
  assert(top_scope != NULL);
  SgScopeStatement * scope = SageBuilder::topScopeStack();
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();

  SgStatement * res = NULL;

  // Conditions
  const std::vector<Conditions *> & conditions = tree->getConditons();
  if (conditions.size() == 0) return bb;
  else if (conditions.size() == 1 && conditions[0]->isTrue()) res = bb;
  else {
    std::vector<Conditions *>::const_iterator it_cond;
    SgExpression * or_cond = conditions[0]->generate();
    for (it_cond = conditions.begin()+1; it_cond != conditions.end(); it_cond++)
      or_cond = SageBuilder::buildOrOp(or_cond, (*it_cond)->generate());
    res = SageBuilder::buildIfStmt(or_cond, bb, NULL);
  }
  

  // Retrieve communication description
  CommDescriptor * comm_desc = tree->getCommDescriptor();
  ComputeSystem * source = comm_desc->getSource();
  ComputeSystem * destination = comm_desc->getDestination();
  ArrayPartition * array_partition = comm_desc->getArrayPartition();
  SyncRequired sync = comm_desc->getSyncRequired();

  // Find out the type of communication
  Core * core_source = dynamic_cast<Core *>(source);
  GPU * gpu_source = dynamic_cast<GPU *>(source);
  Core * core_destination = dynamic_cast<Core *>(destination);
  GPU * gpu_destination = dynamic_cast<GPU *>(destination);

  bool ocl_read = gpu_source != NULL && core_destination != NULL;
  bool ocl_write = core_source != NULL && gpu_destination != NULL;

  assert(ocl_read || ocl_write); // FIXME comm GPU <-> GPU
  assert(!(ocl_read && ocl_write)); // Cannot be both!

  if (array_partition->getDimensions().size() == 0) return bb; // No explicit transfert of scalar from CPU to GPU (kernel arg)

  if (sync == before) {
    // TODO
  }

  if (ocl_read) {
    // TODO multi-gpu queues through arrays
    std::ostringstream oss_queue;
    ComputeNode * compute_node = dynamic_cast<ComputeNode *>(gpu_source->getParent());
    assert(compute_node != NULL);
    oss_queue << "ocl_queue_" << compute_node->getAcceleratorID(gpu_source);

    SgName func_name = "readBuffer";
    SgExprListExp * params = SageBuilder::buildExprListExp(
        SageBuilder::buildVarRefExp(oss_queue.str(), top_scope),
        SageBuilder::buildVarRefExp(genAlias(array_partition, gpu_source)->getInitName()),
        array_partition->getSize(),
        SageBuilder::buildVarRefExp(genAlias(array_partition, core_destination)->getInitName())
    );
    SgFunctionCallExp * call = SageBuilder::buildFunctionCallExp(func_name, SageBuilder::buildVoidType(), params, top_scope);
    bb->append_statement(SageBuilder::buildExprStatement(call));

    // FIXME should be automated (and coalesced)
    {
      SgName sync = "clFinish";
      SgExprListExp * params = SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(oss_queue.str(), top_scope));
      SgFunctionCallExp * sync_call = SageBuilder::buildFunctionCallExp(sync, SageBuilder::buildVoidType(), params, scope);
      bb->append_statement(SageBuilder::buildExprStatement(sync_call));
    }
  }

  if (ocl_write) {
    // TODO multi-gpu queues through arrays
    std::ostringstream oss_queue;
    ComputeNode * compute_node = dynamic_cast<ComputeNode *>(gpu_destination->getParent());
    assert(compute_node != NULL);
    oss_queue << "ocl_queue_" << compute_node->getAcceleratorID(gpu_destination);

    SgName func_name = "writeBuffer";
    SgExprListExp * params = SageBuilder::buildExprListExp(
        SageBuilder::buildVarRefExp(oss_queue.str(), top_scope),
        SageBuilder::buildVarRefExp(genAlias(array_partition, gpu_destination)->getInitName()),
        array_partition->getSize(),
        SageBuilder::buildVarRefExp(genAlias(array_partition, core_source)->getInitName())
    );
    SgFunctionCallExp * call = SageBuilder::buildFunctionCallExp(func_name, SageBuilder::buildVoidType(), params, top_scope);
    bb->append_statement(SageBuilder::buildExprStatement(call));
  }

  if (sync == after) {
    // TODO
  }

  return res;
}

SgStatement * OpenCL_Generator::codeGeneration(SPMD_Sync * tree) {
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();

  assert(false); // FIXME right now there is no explicit sync for OpenCL

  return bb;
}

ArrayAlias * OpenCL_Generator::genAlias(ArrayPartition * array_partition, ComputeSystem * compute_system, bool read_and_write) {
  SageBuilder::pushScopeStack(top_scope);
  std::map<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> >::iterator it_cs = array_aliases.find(compute_system);
  assert(it_cs != array_aliases.end());
  std::map<ArrayPartition *, ArrayAlias *>::iterator it_ap = it_cs->second.find(array_partition);
  if (it_ap == it_cs->second.end()) {
    GPU * gpu = dynamic_cast<GPU *>(compute_system);
    if (gpu != NULL)
      it_ap = it_cs->second.insert(
        std::pair<ArrayPartition *, ArrayAlias *>(array_partition, new OpenCL_Alias(array_partition, gpu, top_scope, read_and_write))
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
  std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased,
  SgStatement * insert_init_after,
  std::string kernel_file_name
) {
  SgScopeStatement * scope = top_scope;

#if DEBUG
  std::cerr << "[OpenCL_Generator::insertInit] Start" << std::endl;
#endif

  unsigned nbr_gpu = 0;
  {
    std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >::iterator it_map;
    std::set<ArrayPartition *>::iterator it_set;
    for (it_map = to_be_aliased.begin(); it_map != to_be_aliased.end(); it_map++) {
      std::map<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> >::iterator it_cs = array_aliases.find(it_map->first);
      if (it_cs == array_aliases.end()) {
        array_aliases.insert(
          std::pair<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> >(it_map->first, std::map<ArrayPartition *, ArrayAlias *>())
        );
      }
      for (it_set = it_map->second.first.begin(); it_set != it_map->second.first.end(); it_set++)
        genAlias(*it_set, it_map->first, false);
      for (it_set = it_map->second.second.begin(); it_set != it_map->second.second.end(); it_set++)
        genAlias(*it_set, it_map->first, true);
      if (dynamic_cast<GPU *>(it_map->first) != NULL)
        nbr_gpu++;
    }
    assert(nbr_gpu == 1); // FIXME will have to modify OpenCL wrapper for this...
  }

  SageInterface::attachComment(insert_init_after, "Declaration and instantiation of basic OpenCL objects", PreprocessingInfo::after);

#if DEBUG
    std::cerr << "[OpenCL_Generator::insertInit] OpenCL init generation" << std::endl;
#endif

  SgVariableDeclaration * context_decl = NULL;
  {
    SgTypedefDeclaration * context_type_decl = SageBuilder::buildTypedefDeclaration("cl_context", SageBuilder::buildIntType(), scope);
    SgType * context_type = SgTypedefType::createType(context_type_decl);
    context_decl = SageBuilder::buildVariableDeclaration("ocl_context", context_type, NULL, scope);
    SageInterface::insertStatementAfter(insert_init_after, context_decl, false);
    insert_init_after = context_decl;
  }

  SgVariableDeclaration * program_decl = NULL;
  {
    SgTypedefDeclaration * program_type_decl = SageBuilder::buildTypedefDeclaration("cl_program", SageBuilder::buildIntType(), scope);
    SgType * program_type = SgTypedefType::createType(program_type_decl);
    program_decl = SageBuilder::buildVariableDeclaration("ocl_program", program_type, NULL, scope);
    SageInterface::insertStatementAfter(insert_init_after, program_decl, false);
    insert_init_after = program_decl;
  }

  SgVariableDeclaration * queue_decl = NULL;
  {
    SgTypedefDeclaration * queue_type_decl = SageBuilder::buildTypedefDeclaration("cl_command_queue", SageBuilder::buildIntType(), scope);
    SgType * queue_type = SgTypedefType::createType(queue_type_decl);
    queue_decl = SageBuilder::buildVariableDeclaration("ocl_queue_0", queue_type, NULL, scope);
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
    std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >::iterator it1;
    std::set<ArrayPartition *>::iterator it2;
#if DEBUG
    std::cerr << "[OpenCL_Generator::insertInit] Buffers generation" << std::endl;
#endif
    for (it1 = to_be_aliased.begin(); it1 != to_be_aliased.end(); it1++) {
      for (it2 = it1->second.first.begin(); it2 != it1->second.first.end(); it2++) {
        OpenCL_Alias * ocl_alias = dynamic_cast<OpenCL_Alias *>(genAlias(*it2, it1->first));
        if (ocl_alias != NULL) {
          SgInitializedName * init_name = ocl_alias->getInitName();
          SgVariableDeclaration * var_decl = new SgVariableDeclaration(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
          var_decl->get_variables().push_back(init_name);
          init_name->set_parent(var_decl);
          SageInterface::insertStatementAfter(insert_init_after, var_decl);
          insert_init_after = var_decl;
        }
      }
      for (it2 = it1->second.second.begin(); it2 != it1->second.second.end(); it2++) {
        OpenCL_Alias * ocl_alias = dynamic_cast<OpenCL_Alias *>(genAlias(*it2, it1->first));
        if (ocl_alias != NULL) {
          SgInitializedName * init_name = ocl_alias->getInitName();
          SgVariableDeclaration * var_decl = new SgVariableDeclaration(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
          var_decl->get_variables().push_back(init_name);
          init_name->set_parent(var_decl);
          SageInterface::insertStatementAfter(insert_init_after, var_decl);
          insert_init_after = var_decl;
        }
      }
    }
  }

  SageInterface::attachComment(insert_init_after, "Declaration of the kernels objects", PreprocessingInfo::after);

  {
    SgTypedefDeclaration * kernel_type_decl = SageBuilder::buildTypedefDeclaration("cl_kernel", SageBuilder::buildIntType(), scope);
    SgType * kernel_type = SgTypedefType::createType(kernel_type_decl);

    std::vector<SPMD_KernelCall *> kernels;
    {
      std::queue<SPMD_Tree *> tree_queue;
      tree_queue.push(root_tree);
      while (tree_queue.size() > 0) {
        SPMD_Tree * tree = tree_queue.front();
        tree_queue.pop();
        SPMD_KernelCall * kernel = dynamic_cast<SPMD_KernelCall *>(tree);
        if (kernel != NULL) kernels.push_back(kernel);
        else {
          std::vector<SPMD_Tree *> & children = tree->getChildren();
          std::vector<SPMD_Tree *>::iterator it_child;
          for (it_child = children.begin(); it_child != children.end(); it_child++)
            tree_queue.push(*it_child);
        }
      }
    }

    std::vector<SPMD_KernelCall *>::iterator it_kernel;
    for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
      SPMD_KernelCall * kernel_call = *it_kernel;

      // Kernel name
      std::ostringstream oss_kernel_name;
      oss_kernel_name << "ocl_kernel_" << kernel_call->getID();
      std::string kernel_name = oss_kernel_name.str();

      //
      SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration(kernel_name, kernel_type, NULL, scope);
      SageInterface::insertStatementAfter(insert_init_after, kernel_decl, false);
      insert_init_after = kernel_decl;
    }
  }

  // Build additionnal OpenCL API types
  {
    SgTypedefDeclaration * kernel_type_decl = SageBuilder::buildTypedefDeclaration("cl_kernel", SageBuilder::buildIntType(), scope);
  }
#if DEBUG
    std::cerr << "[OpenCL_Generator::insertInit] Done" << std::endl;
#endif
}

void OpenCL_Generator::insertFinal(SPMD_Tree * root_tree, SgStatement * insert_final_after) {
  // TODO
}

void OpenCL_Generator::generateKernel(SPMD_Tree * root_tree, SgSourceFile * kernel_file) {
  SgGlobal * ocl_scope = kernel_file->get_globalScope();
  assert(ocl_scope != NULL);
  SageBuilder::pushScopeStack(ocl_scope);

  std::vector<SPMD_KernelCall *> kernels;
  {
    std::queue<SPMD_Tree *> tree_queue;
    tree_queue.push(root_tree);
    while (tree_queue.size() > 0) {
      SPMD_Tree * tree = tree_queue.front();
      tree_queue.pop();
      SPMD_KernelCall * kernel = dynamic_cast<SPMD_KernelCall *>(tree);
      if (kernel != NULL) kernels.push_back(kernel);
      else {
        std::vector<SPMD_Tree *> & children = tree->getChildren();
        std::vector<SPMD_Tree *>::iterator it_child;
        for (it_child = children.begin(); it_child != children.end(); it_child++)
          tree_queue.push(*it_child);
      }
    }
  }

  std::vector<SPMD_KernelCall *>::iterator it_kernel;
  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
    SPMD_KernelCall * kernel_call = *it_kernel;

    // Kernel name
    std::ostringstream oss_kernel_name;
    oss_kernel_name << "ocl_kernel_" << kernel_call->getID();
    std::string kernel_name = oss_kernel_name.str();

    // Data to passed as argument
    // TODO use it 
    const std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > & datas = driver.getArrayAnalysis().get(kernel_call);

    // Placement
    ComputeSystem * compute_system = driver.getPlacement().assigned(kernel_call);
    GPU * gpu = dynamic_cast<GPU *>(compute_system);
    assert(gpu != NULL);

    // surrounding iterators
    std::vector<RoseVariable> ordered_iterators;
    SPMD_Tree * parent = kernel_call->getParent();
    while (dynamic_cast<SPMD_Root *>(parent) == NULL) {
      SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(parent);
      if (loop != NULL)
        ordered_iterators.insert(ordered_iterators.begin(), loop->getIterator());
      parent = parent->getParent();
    }

    std::map<SPMD_KernelCall *, std::vector<ArrayPartition *> >::iterator it_kernel_data_param =
            kernel_data_param.find(kernel_call);
    assert(it_kernel_data_param != kernel_data_param.end());
    std::vector<ArrayPartition *>::iterator it_params;

    SgFunctionParameterList * param_list = SageBuilder::buildFunctionParameterList();
    for (int i = 0; i < ordered_iterators.size(); i++) {
      SgInitializedName * init_name = SageBuilder::buildInitializedName(
          ordered_iterators[i].getString(), SageBuilder::buildIntType()
      );
      param_list->append_arg(init_name);
    }
    for (it_params = it_kernel_data_param->second.begin(); it_params != it_kernel_data_param->second.end(); it_params++) {
      ArrayAlias * alias = genAlias(*it_params, gpu);
      OpenCL_Alias * ocl_alias = dynamic_cast<OpenCL_Alias *>(alias);
      assert(ocl_alias != NULL);

      param_list->append_arg(ocl_alias->getKernelParam());
    }

    SgFunctionDeclaration * kernel_decl = SageBuilder::buildDefiningFunctionDeclaration(
        SgName(kernel_name), SageBuilder::buildVoidType(), param_list
    );
    kernel_decl->get_functionModifier().setOpenclKernel();
    SgFunctionDefinition * kernel_def = kernel_decl->get_definition();
    ROSE_ASSERT(kernel_def != NULL);
    ocl_scope->append_statement(kernel_decl);
    SageBuilder::pushScopeStack(kernel_def);

    SgBasicBlock * kernel_body = kernel_def->get_body();
    ROSE_ASSERT(kernel_body != NULL);
    SageBuilder::pushScopeStack(kernel_body);

    SgName global_id = "get_global_id";
    const std::vector<RoseVariable> & iterators = kernel_call->getOrderedIterators();
    std::vector<RoseVariable>::const_iterator it_iterator;
    const std::map<RoseVariable, Domain *> & iterators_map = kernel_call->getIterators();
    std::map<RoseVariable, Domain *>::const_iterator it_iterator_map;
    unsigned cnt_dim = 0;
    for (it_iterator = iterators.begin(); it_iterator != iterators.end(); it_iterator++) {
      SgExprListExp * params = SageBuilder::buildExprListExp(SageBuilder::buildUnsignedIntVal(cnt_dim++));
      SgFunctionCallExp * get_global_id_call = SageBuilder::buildFunctionCallExp(global_id, SageBuilder::buildUnsignedIntType(), params, kernel_def);

      it_iterator_map = iterators_map.find(*it_iterator);
      assert(it_iterator_map != iterators_map.end());
      int stride = it_iterator_map->second->getStride();
      SgExpression * loc_it = SageBuilder::buildMultiplyOp(get_global_id_call, SageBuilder::buildIntVal(stride > 0 ? stride : -stride));
      loc_it = SageBuilder::buildAddOp(loc_it, it_iterator_map->second->genLowerBound());
      SgInitializer * init = SageBuilder::buildAssignInitializer(loc_it, SageBuilder::buildIntType());

      SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(
          it_iterator->getString(),
          SageBuilder::buildIntType(),
          init,
          kernel_def
      );

      kernel_body->append_statement(var_decl);
    }


    std::set<RoseVariable> inside_iterators;
    {
      std::vector<SPMD_Tree *>::iterator it_child;
      std::queue<SPMD_Tree *> tree_queue;
      tree_queue.push(kernel_call);
      while (tree_queue.size() > 0) {
        SPMD_Tree * tree = tree_queue.front();
        tree_queue.pop();
        SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(tree);
        if (loop != NULL)
          inside_iterators.insert(loop->getIterator());
        std::vector<SPMD_Tree *> & children = tree->getChildren();
        for (it_child = children.begin(); it_child != children.end(); it_child++)
          tree_queue.push(*it_child);
      }
    }
    std::set<RoseVariable>::iterator it_inside_iterator;
    for (it_inside_iterator = inside_iterators.begin(); it_inside_iterator != inside_iterators.end(); it_inside_iterator++) {
      SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(
          it_inside_iterator->getString(),
          SageBuilder::buildIntType(),
          NULL,
          kernel_def
      );
      kernel_body->append_statement(var_decl);
    }

    const std::vector<std::pair<Expression *, bool> > & restrictions = kernel_call->getRestrictions();
    std::vector<std::pair<Expression *, bool> >::const_iterator it_restriction;
    SgExpression * bounds_control_exp = SageBuilder::buildBoolValExp(true);
    for (it_iterator_map = iterators_map.begin(); it_iterator_map != iterators_map.end(); it_iterator_map++) {
      bounds_control_exp = SageBuilder::buildAndOp(bounds_control_exp, SageBuilder::buildAndOp(
        SageBuilder::buildGreaterOrEqualOp(
          SageBuilder::buildVarRefExp(it_iterator_map->first.getString()),
          it_iterator_map->second->genLowerBound()
        ),
        SageBuilder::buildLessOrEqualOp(
          SageBuilder::buildVarRefExp(it_iterator_map->first.getString()),
          it_iterator_map->second->genUpperBound()
        )
      ));
    }
    for (it_restriction = restrictions.begin(); it_restriction != restrictions.end(); it_restriction++) {
      SgExpression * tmp = it_restriction->first->generate();
      bounds_control_exp = SageBuilder::buildAndOp(bounds_control_exp, it_restriction->second ?
          (SgExpression *) SageBuilder::buildGreaterOrEqualOp(tmp, SageBuilder::buildIntVal(0)) :
          (SgExpression *) SageBuilder::buildEqualityOp(tmp, SageBuilder::buildIntVal(0)) 
      );
    }

    SgBasicBlock * bb = SageBuilder::buildBasicBlock(); // FIXME should be cond body

    SgIfStmt * if_stmt = SageBuilder::buildIfStmt(bounds_control_exp, bb, NULL);
    kernel_body->append_statement(if_stmt);

    SageBuilder::pushScopeStack(bb);

    std::vector<SPMD_Tree *> & children = kernel_call->getChildren();
    std::vector<SPMD_Tree *>::iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++) {
      bb->append_statement(SPMD_Generator::codeGeneration(*it_child));
    }

    SageBuilder::popScopeStack();
    SageBuilder::popScopeStack();
    SageBuilder::popScopeStack();
  }

  SageBuilder::popScopeStack();
}

OpenCL_Generator::OpenCL_Generator(SPMD_Driver & driver_) :
  SPMD_Generator(driver_),
  ocl_file_name(),
  kernel_data_param()
{
  assert(driver.hasArrayAnalysis() && driver.hasPlacement());
}

OpenCL_Generator::~OpenCL_Generator() {}

SgSourceFile * OpenCL_Generator::buildKernelFile(std::string kernel_file_name) {
  SgSourceFile * kernel_file = new SgSourceFile();
  {
    kernel_file->set_unparse_output_filename(kernel_file_name);
    kernel_file->set_file_info(new Sg_File_Info(kernel_file_name));

    SageInterface::getProject()->get_fileList_ptr()->get_listOfFiles().push_back(kernel_file);

    SgGlobal * gs = new SgGlobal();
    gs->set_file_info(new Sg_File_Info(kernel_file_name));
    kernel_file->set_globalScope(gs);

    kernel_file->set_OpenCL_only(true);
    kernel_file->set_outputLanguage(SgFile::e_C_output_language); // FIXME could be remove with older version of rose
  }
  ROSE_ASSERT(kernel_file != NULL);
  return kernel_file;
}

