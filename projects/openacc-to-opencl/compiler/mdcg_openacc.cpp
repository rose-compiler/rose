
#include "mdcg_openacc.hpp"

namespace MDCG {

namespace OpenACC {

SgExpression * LoopDesc::createFieldInitializer(
  const MDCG::CodeGenerator & codegen,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  unsigned file_id
) {

  switch (field_id) {
    case 0:
      /// /todo unsigned long tiles[7];
      return SageBuilder::buildAggregateInitializer(
               SageBuilder::buildExprListExp(
                 SageBuilder::buildLongIntVal(input.tile_0),
                 SageBuilder::buildLongIntVal(input.gang),
                 SageBuilder::buildLongIntVal(input.tile_1),
                 SageBuilder::buildLongIntVal(input.worker),
                 SageBuilder::buildLongIntVal(input.tile_2),
                 SageBuilder::buildLongIntVal(input.vector),
                 SageBuilder::buildLongIntVal(input.tile_3)
               )
             );
    default:
      assert(false);
  }
}

SgExpression * KernelVersion::createFieldInitializer(
  const MDCG::CodeGenerator & codegen,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  unsigned file_id
) {
  switch (field_id) {
    case 0:
      /// /todo unsigned long num_gang;
      return SageBuilder::buildIntVal(0);
    case 1:
      /// /todo unsigned long num_worker;
      return SageBuilder::buildIntVal(0);
    case 2:
      /// /todo unsigned long vector_length;
      return SageBuilder::buildIntVal(1);
    case 3:
    {
      /// struct acc_loop_t_ * loops;
      std::ostringstream decl_name;
        decl_name << "loops_" << input;

      MDCG::Model::type_t type = element->node->type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
      return codegen.createArrayPointer<LoopDesc>(
               type->node->base_class,
               input->loops.size(),
               input->loops.begin(),
               input->loops.end(),
               file_id,
               decl_name.str()
             );
    }
    case 4:
      /// char * suffix;
      return SageBuilder::buildStringVal(input->kernel_name);
    default:
      assert(false);
  }
}

SgExpression * createArrayOfTypeSize(
  const MDCG::CodeGenerator & codegen,
  const std::list<SgVariableSymbol *> & input,
  std::string array_name,
  unsigned file_id
) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();
  SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);

  std::list<SgVariableSymbol *>::const_iterator it;
  for (it = input.begin(); it != input.end(); it++)
    expr_list->append_expression(SageBuilder::buildSizeOfOp((*it)->get_type()));

  SgGlobal * global_scope_across_files = codegen.getDriver().project->get_globalScopeAcrossFiles();
  assert(global_scope_across_files != NULL);
  SgTypedefSymbol * size_t_symbol = SageInterface::lookupTypedefSymbolInParentScopes("size_t", global_scope_across_files);
  assert(size_t_symbol != NULL);
  SgType * size_t_type = isSgType(size_t_symbol->get_type());
  assert(size_t_type != NULL);
  size_t_type = SageBuilder::buildArrayType(size_t_type, SageBuilder::buildIntVal(input.size()));

  MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(array_name, size_t_type, init, NULL, file_id, false, true);
  MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = codegen.getDriver().build<SgVariableDeclaration>(var_decl_desc);

  return SageBuilder::buildVarRefExp(var_decl_res.symbol);
}

SgExpression * KernelDesc::createFieldInitializer(
  const MDCG::CodeGenerator & codegen,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  unsigned file_id
) {
  const Kernel::arguments_t & args = input->getArguments();
  const std::vector<Kernel::a_kernel *> & versions = input->getKernels();

  std::ostringstream names_suffix;
    names_suffix << "_" << input;

  switch (field_id) {
    case 0:
      /// unsigned id;
      return SageBuilder::buildIntVal(input->id);
    case 1:
      /// char * name;
      return SageBuilder::buildStringVal("");
    case 2:
      /// size_t num_params;
      return SageBuilder::buildIntVal(args.parameters.size());
    case 3:
      /// size_t * size_params;
      return createArrayOfTypeSize(codegen, args.parameters, std::string("param_sizes") + names_suffix.str(), file_id);
    case 4:
      /// size_t num_scalars;
      return SageBuilder::buildIntVal(args.scalars.size());
    case 5:
      /// size_t * size_scalars;
      return createArrayOfTypeSize(codegen, args.scalars, std::string("scalar_sizes") + names_suffix.str(), file_id);
    case 6:
      /// size_t num_datas;
      return SageBuilder::buildIntVal(args.datas.size());
    case 7:
      /// size_t num_loops;
      return SageBuilder::buildIntVal(input->num_loops);
    case 8:
      /// unsigned num_versions;
      return SageBuilder::buildIntVal(versions.size());
    case 9:
    {
      /// acc_kernel_version_t * versions;
      MDCG::Model::type_t type = element->node->type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
      return codegen.createPointerArrayPointer<KernelVersion>(
               type->node->base_class,
               versions.size(),
               versions.begin(),
               versions.end(),
               file_id,
               "versions",
               "version"
             );
    }
    case 10:
    {
      /// \todo acc_loop_splitter_t * splitted_loop;
      return SageBuilder::buildIntVal(0);
    }
    default:
      assert(false);
  }
}

SgExpression * RegionDesc::createFieldInitializer(
  const MDCG::CodeGenerator & codegen,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  unsigned file_id
) {
  assert(input.kernel_lists.size() == 1);
  const std::list<Kernel *> & kernels = *(input.kernel_lists.begin());

  std::ostringstream decl_name;
    decl_name << "kernels_" << input.id;
  std::ostringstream decl_prefix;
    decl_prefix << "kernel_desc_" << input.id;

  switch (field_id) {
    case 0:
      /// unsigned id;
      return SageBuilder::buildIntVal(input.id);
    case 1:
      /// char * file;
      return SageBuilder::buildStringVal(input.file.c_str());
    case 2:
      /// \todo size_t num_options;
      return SageBuilder::buildIntVal(1);
    case 3:
      /// \todo char ** options;
    {
      SgExprListExp * expr_list = SageBuilder::buildExprListExp();
      SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);

      expr_list->append_expression(SageBuilder::buildStringVal("-g"));

      MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(
             "ocl_compiler_opts",
             SageBuilder::buildArrayType(
               SageBuilder::buildPointerType(SageBuilder::buildCharType()),
               SageBuilder::buildIntVal(1)
             ),
             init, NULL, file_id, false, true
      );

      MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = codegen.getDriver().build<SgVariableDeclaration>(var_decl_desc);

      return SageBuilder::buildVarRefExp(var_decl_res.symbol);
    }
    case 4:
      /// size_t num_kernels;
      return SageBuilder::buildIntVal(kernels.size());
    case 5:
    {
      /// acc_kernel_desc_t * kernels;
      MDCG::Model::type_t type = element->node->type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
      return codegen.createPointerArrayPointer<KernelDesc>(
               type->node->base_class,
               kernels.size(),
               kernels.begin(),
               kernels.end(),
               file_id,
               decl_name.str(),
               decl_prefix.str()
             );
    }
    case 6:
      /// \todo size_t num_devices;
      return SageBuilder::buildIntVal(1);
    case 7:
      /// \todo struct { acc_device_t kind; size_t num; } * devices;
      return SageBuilder::buildIntVal(0); // NULL
    case 8:
      /// \todo size_t num_distributed_datas;
      return SageBuilder::buildIntVal(0);
    case 9:
      /// \todo struct acc_data_distribution_t_ * data_distributions;
      return SageBuilder::buildIntVal(0); // NULL
    default:
      assert(false);
  }
}

SgExpression * CompilerData::createFieldInitializer(
  const MDCG::CodeGenerator & codegen,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  unsigned file_id
) {
  switch (field_id) {
    case 0:
      /// const char * acc_runtime_dir;
      return input.runtime_dir;
    case 1:
      /// const char * acc_runtime_ocl;
      return input.ocl_runtime;
    case 2:
      /// const char * acc_kernels_dir;
      return input.kernels_dir;
    case 3:
      /// const unsigned long num_regions;
      return SageBuilder::buildIntVal(input.regions.size());
    case 4:
    {
      /// const acc_region_desc_t * regions;
      MDCG::Model::type_t type = element->node->type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
      type = type->node->base_type;
      assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
      return codegen.createPointerArrayPointer<RegionDesc>(
               type->node->base_class,
               input.regions.size(),
               input.regions.begin(),
               input.regions.end(),
               file_id,
               "regions",
               "region_desc"
             );
    }
    default:
      assert(false);
  }
}

}

}
