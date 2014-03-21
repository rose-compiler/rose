
#include "openacc_spec.hpp"

#include "klt_itmap.hpp"
#include "mdcg_openacc.hpp"

typedef ::KLT::Language::OpenCL Language;
typedef ::KLT::Runtime::OpenACC Runtime;
typedef ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t> Annotation;
typedef ::KLT::LoopTrees<Annotation> LoopTrees;
typedef ::KLT::Kernel<Annotation, Language, Runtime> Kernel;

unsigned readOpenaccModel(MDCG::ModelBuilder & model_builder, const std::string & libopenacc_dir) {
  unsigned openacc_model = model_builder.create();

  model_builder.add(openacc_model, "compiler",     libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "region",       libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "kernel",       libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "loop",         libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "api",          libopenacc_dir + "/OpenACC/device",   "cl");
/*
  model_builder.add(openacc_model, "data-env",     libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "init",         libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "mem-manager",  libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "opencl-debug", libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "opencl-init",  libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "runtime",      libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "data-env",     libopenacc_dir + "/OpenACC/private",  "h");
  model_builder.add(openacc_model, "debug",        libopenacc_dir + "/OpenACC/private",  "h");
  model_builder.add(openacc_model, "init",         libopenacc_dir + "/OpenACC/private",  "h");
  model_builder.add(openacc_model, "kernel",       libopenacc_dir + "/OpenACC/private",  "h");
  model_builder.add(openacc_model, "loop",         libopenacc_dir + "/OpenACC/private",  "h");
  model_builder.add(openacc_model, "region",       libopenacc_dir + "/OpenACC/private",  "h");
  model_builder.add(openacc_model, "runtime",      libopenacc_dir + "/OpenACC/private",  "h");
  model_builder.add(openacc_model, "openacc",      libopenacc_dir + "/OpenACC/",         "h");
*/
  return openacc_model;
}

int main(int argc, char ** argv) {

  // Arguments
  assert(argc == 7 || argc == 10);

  std::string libopenacc_dir(argv[2]);

  std::string opencl_dir(argv[3]);

  long t_0_0 = atol(argv[4]);
  long t_0_1 = atol(argv[5]);
  long t_0_2 = atol(argv[6]);
  
  long t_1_0 = 0;
  long t_1_1 = 0;
  long t_1_2 = 0;
  if (argc == 10) {
    t_1_0 = atol(argv[7]);
    t_1_1 = atol(argv[8]);
    t_1_2 = atol(argv[9]);
  }

  // Build a default ROSE project
  SgProject * project = new SgProject::SgProject();
  { // Add default command line to an empty project
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
      arglist.push_back(std::string("-I") + libopenacc_dir);
      arglist.push_back(std::string("-I") + opencl_dir);
      arglist.push_back("-c");
    project->set_originalCommandLineArgumentList (arglist);
  }

  // Initialize DLX for OpenACC
  DLX::OpenACC::language_t::init();

  // Initialize MFB to use with KLT
  MFB::KLT_Driver driver(project);

  // Initialize MDCG's ModelBuilder and Code Generator
  MDCG::ModelBuilder model_builder(driver);
  MDCG::CodeGenerator codegen(driver);

  // Initialize KLT's Generator
  KLT::Generator<Annotation, Language, Runtime, MFB::KLT_Driver> generator(driver, "kernels.cl");

  // Read input LoopTrees
  LoopTrees loop_trees;
  loop_trees.read(argv[1]);

  // Read OpenACC Model
  unsigned model = readOpenaccModel(model_builder, libopenacc_dir);

  unsigned host_data_file_id = driver.add(boost::filesystem::path(std::string("host-data.c")));
  driver.setUnparsedFile(host_data_file_id);

  // Load OpenACC API for KLT
  KLT::Runtime::OpenACC::loadAPI(driver, libopenacc_dir);

  // Create a Code Generation Configuration
  KLT::CG_Config<Annotation, Language, Runtime> cg_config(
      new KLT::LoopMapper<Annotation, Language, Runtime>(),
      new KLT::SingleVersionItMapper(t_0_0, t_0_1, t_0_2, t_1_0, t_1_1, t_1_2),
      new KLT::DataFlow<Annotation, Language, Runtime>()
  );

  // Call the generator
  MDCG::OpenACC::RegionDesc::input_t input_region;
    input_region.id = 0;
    input_region.file = std::string("kernels.cl");
    generator.generate(loop_trees, input_region.kernel_lists, cg_config);

  MDCG::OpenACC::CompilerData::input_t input;
    input.runtime_dir = SageBuilder::buildVarRefExp("LIBOPENACC_DIR");
    input.ocl_runtime = SageBuilder::buildStringVal("lib/opencl/libopenacc.cl");
    input.kernels_dir = SageBuilder::buildVarRefExp("KERNEL_DIR");
    input.regions.push_back(input_region);

  // Get model element for Region Descriptor
  std::set<MDCG::Model::class_t> classes;
//model_builder.get(model).lookup<MDCG::Model::class_t>("acc_region_desc_t_", classes);
  model_builder.get(model).lookup<MDCG::Model::class_t>("acc_compiler_data_t_", classes);
  assert(classes.size() == 1);
  MDCG::Model::class_t region_desc_class = *(classes.begin());

//codegen.addDeclaration<MDCG::OpenACC::RegionDesc>(region_desc_class, input, host_data_file_id, "regions");
  codegen.addDeclaration<MDCG::OpenACC::CompilerData>(region_desc_class, input, host_data_file_id, "compiler_data");

  project->unparse(); // Cannot call the backend directly because of OpenCL files. There is a warning when trying, just have to trace it.

  return 0;
}

