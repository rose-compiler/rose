
#include "MDCG/OpenACC/model.hpp"

namespace MDCG {

namespace OpenACC {

#ifndef DEBUG_FLAG
bool add_debug_flag = false;
const char * debug_flag = NULL;
#else
bool add_debug_flag = true;
const char * debug_flag = DEBUG_FLAG;
#endif

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

size_t s_version_id = 0;

SgExpression * KernelVersion::createFieldInitializer(
  const MDCG::CodeGenerator & codegen,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  unsigned file_id
) {
  switch (field_id) {
    case 0:
      return SageBuilder::buildIntVal(s_version_id++);
    case 1:
      /// /todo unsigned long num_gang;
      return SageBuilder::buildIntVal(0);
    case 2:
      /// /todo unsigned long num_worker;
      return SageBuilder::buildIntVal(0);
    case 3:
      /// /todo unsigned long vector_length;
      return SageBuilder::buildIntVal(1);
    case 4:
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
    case 5:
      /// char * suffix;
      return SageBuilder::buildStringVal(input->kernel_name);
    case 6:
      /// acc_device_t device_affinity;
      return SageBuilder::buildIntVal(0); /// \todo use 'acc_device_any'
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
      s_version_id = 0;
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
               std::string("versions") + names_suffix.str(),
               std::string("version") + names_suffix.str()
             );
    }
    case 10:
    {
      /// \todo acc_loop_splitter_t * splitted_loop;
      return SageBuilder::buildIntVal(0);
    }
    case 11:
    {
      /// \todo size_t * version_by_devices; 
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
      if (add_debug_flag)
        return SageBuilder::buildIntVal(1);
      else
        return SageBuilder::buildIntVal(0);
    case 3:
      if (add_debug_flag) {
        SgExprListExp * expr_list = SageBuilder::buildExprListExp();
        SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);

        expr_list->append_expression(SageBuilder::buildStringVal(debug_flag));

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
      else
        return SageBuilder::buildIntVal(0);
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

void LoopDesc::storeToDB(sqlite3 * db_file, unsigned region_id, unsigned kernel_id, unsigned version_id, unsigned loop_id, const input_t & input) {
  char * err_msg;
  char * query = (char *)malloc(200 * sizeof(char));
  sprintf(query, "INSERT INTO Loops VALUES ( '%u', '%u', '%u' , '%u' , '%lu' , '%lu' , '%lu' , '%lu' , '%lu' , '%lu' , '%lu' , '%d' , '%d' , '%d' , '%d' );",
                 region_id, kernel_id, version_id, loop_id,
                 input.tile_0, input.gang, input.tile_1, input.worker, input.tile_2, input.vector, input.tile_3,
                 input.unroll_tile_0, input.unroll_tile_1, input.unroll_tile_2, input.unroll_tile_3
         );
  int status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
  assert (status == SQLITE_OK);
  free(query);
}

void KernelVersion::storeToDB(sqlite3 * db_file, unsigned region_id, unsigned kernel_id, unsigned version_id, const input_t & input) {
  std::vector< ::KLT::Runtime::OpenACC::a_loop>::const_iterator it;
  unsigned cnt_loop = 0;
  for (it = input->loops.begin(); it != input->loops.end(); it++)
    LoopDesc::storeToDB(db_file, region_id, kernel_id, version_id, cnt_loop++, *it);

  char * err_msg;
  char * query = (char *)malloc(140 * sizeof(char));
  sprintf(query, "INSERT INTO Versions VALUES ( '%u', '%u', '%u' , '%s' );", region_id, kernel_id, version_id, input->kernel_name.c_str());
  int status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
  assert (status == SQLITE_OK);
  free(query);
}

size_t ocl_sizeof(SgType * type) {
  if (isSgTypeInt(type))         return sizeof(int);
  else if (isSgTypeLong(type))  return sizeof(long);
  else if (isSgTypeFloat(type))  return sizeof(float);
  else if (isSgTypeDouble(type)) return sizeof(double);
  else assert(0);

  return 0;
}

void KernelDesc::storeToDB(sqlite3 * db_file, unsigned region_id, unsigned kernel_id, const input_t & input) {
  const std::vector<Kernel::a_kernel *> & versions = input->getKernels();
  std::vector<Kernel::a_kernel *>::const_iterator it;
  unsigned cnt_version = 0;
  for (it = versions.begin(); it != versions.end(); it++)
    KernelVersion::storeToDB(db_file, region_id, kernel_id, cnt_version++, *it);

  unsigned num_loops = input->num_loops;

  char * err_msg;
  char * query = (char *)malloc(140 * sizeof(char));

  sprintf(query, "INSERT INTO Kernels VALUES ( '%u', '%u', '%s' , '%u' , '%u' );", region_id, kernel_id, "", cnt_version, num_loops);
  int status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
  assert (status == SQLITE_OK);

  const Kernel::arguments_t & args = input->getArguments();
  
  std::list<SgVariableSymbol *>::const_iterator it_sym;
  size_t cnt = 0;
  for (it_sym = args.parameters.begin(); it_sym != args.parameters.end(); it_sym++) {
    sprintf(query, "INSERT INTO Parameters VALUES ( '%u', '%u', '%zd' , '%zd' );", region_id, kernel_id, cnt++, ocl_sizeof((*it_sym)->get_type()));
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);
  }
  cnt = 0;
  for (it_sym = args.scalars.begin(); it_sym != args.scalars.end(); it_sym++) {
    sprintf(query, "INSERT INTO Scalars VALUES ( '%u', '%u', '%zd' , '%zd' );", region_id, kernel_id, cnt++, ocl_sizeof((*it_sym)->get_type()));
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);
  }
  
  std::list< ::KLT::Data<Annotation> *>::const_iterator it_data;
  cnt = 0;
  for (it_data = args.datas.begin(); it_data != args.datas.end(); it_data++) {
    sprintf(query, "INSERT INTO Datas VALUES ( '%u', '%u', '%zd' );", region_id, kernel_id, cnt++);
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);
  }

  free(query);
}

void RegionDesc::storeToDB(sqlite3 * db_file, const input_t & input) {
  assert(input.kernel_lists.size() == 1);
  const std::list<Kernel *> & kernels = *(input.kernel_lists.begin());

  std::list<Kernel *>::const_iterator it;
  unsigned cnt_kernel = 0;
  for (it = kernels.begin(); it != kernels.end(); it++)
    KernelDesc::storeToDB(db_file, input.id, cnt_kernel++, *it);

  char * err_msg;
  char * query = (char *)malloc(120 * sizeof(char));
  sprintf(query, "INSERT INTO Regions VALUES ( '%u', '%s' , '%u' );", input.id, input.file.c_str(), cnt_kernel);
  int status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
  assert (status == SQLITE_OK);
  free(query);
}

void CompilerData::storeToDB(const std::string & db_file_name, const input_t & input) {
  bool exist = boost::filesystem::exists(boost::filesystem::path(db_file_name));

  sqlite3 * db_file;
  int status = sqlite3_open(db_file_name.c_str(), &db_file);
  assert(status == SQLITE_OK);

  if (!exist) {
    char * err_msg;
    char * query;

    query = "CREATE TABLE Regions ( region_id INT , opencl_file CHAR(50) , num_kernels INT );";
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);

    query = "CREATE TABLE Kernels ( region_id INT , kernel_id INT , name CHAR(30) , num_versions INT , num_loops INT );";
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);

    query = "CREATE TABLE Versions ( region_id INT , kernel_id INT , version_id INT , suffix CHAR(30) );";
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);

    query = "CREATE TABLE Loops ( region_id INT , kernel_id INT , version_id INT , loop_id INT , tile_0 INT , gang INT , tile_1 INT , worker INT , tile_2 INT , vector INT , tile_3 INT , unroll_tile_0 INT , unroll_tile_1 INT , unroll_tile_2 INT , unroll_tile_3 INT);";
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);

    query = "CREATE TABLE Parameters ( region_id INT , kernel_id INT, idx INT, size INT );";
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);

    query = "CREATE TABLE Scalars ( region_id INT , kernel_id INT, idx INT, size INT );";
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);

    query = "CREATE TABLE Datas ( region_id INT , kernel_id INT, idx INT );";
    status = sqlite3_exec (db_file, query, NULL, NULL, &err_msg);
    assert (status == SQLITE_OK);
  }

  std::vector<RegionDesc::input_t>::const_iterator it;
  for (it = input.regions.begin(); it != input.regions.end(); it++)
    RegionDesc::storeToDB(db_file, *it);

  sqlite3_close(db_file);
}

unsigned readOpenaccModel(MDCG::ModelBuilder & model_builder, const std::string & libopenacc_inc_dir) {
  unsigned openacc_model = model_builder.create();

  model_builder.add(openacc_model, "compiler",     libopenacc_inc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "region",       libopenacc_inc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "kernel",       libopenacc_inc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "loop",         libopenacc_inc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "api",          libopenacc_inc_dir + "/OpenACC/device",   "cl");

  return openacc_model;
}

}

}

