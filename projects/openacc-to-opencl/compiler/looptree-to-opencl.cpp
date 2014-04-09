
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

  return openacc_model;
}

int main(int argc, char ** argv) {

  // Arguments
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " LIBOPENACC_DIR OPENCL_DIR versions.db [looptree.lt [...] ]" << std::endl;
    exit(-1);
  }

  // argv[1]: LoopTrees file

  std::string libopenacc_dir(argv[1]);

  std::string opencl_dir(argv[2]);

  std::string db_file(argv[3]);

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

  // Read OpenACC Model
  unsigned model = readOpenaccModel(model_builder, libopenacc_dir);

  unsigned host_data_file_id = driver.add(boost::filesystem::path(std::string("host-data.c")));
  driver.setUnparsedFile(host_data_file_id);

  // Load OpenACC API for KLT
  KLT::Runtime::OpenACC::loadAPI(driver, libopenacc_dir);

  std::vector<unsigned> tiling_sizes;/*
    tiling_sizes.push_back(2);
    tiling_sizes.push_back(4);
    tiling_sizes.push_back(8);*/
    tiling_sizes.push_back(16);
    tiling_sizes.push_back(32);
    tiling_sizes.push_back(64);
    tiling_sizes.push_back(128);

  // Create a Code Generation Configuration
  KLT::CG_Config<Annotation, Language, Runtime> cg_config(
      new KLT::LoopMapper<Annotation, Language, Runtime>(),
      new KLT::IterationMapperOpenACC(tiling_sizes),
      new KLT::DataFlow<Annotation, Language, Runtime>()
  );

  // Call the generator for each input file and merge the results (Assume that only one kernel is produced)
  Kernel * kernel = NULL;
  for (size_t arg_idx = 4; arg_idx < argc; arg_idx++) {
    // Read input LoopTrees
    LoopTrees loop_trees;
    loop_trees.read(argv[arg_idx]);

    // Generate kernels
    std::set<std::list<Kernel *> > kernel_lists;
    generator.generate(loop_trees, kernel_lists, cg_config);

    // Merge results
    assert(kernel_lists.size() == 1);
    const std::list<Kernel *> & tmp_kernel_list = *(kernel_lists.begin());
    assert(tmp_kernel_list.size() == 1);
    if (kernel == NULL)
      kernel = tmp_kernel_list.front();
    else {
      const std::vector<Kernel::a_kernel *> & versions = tmp_kernel_list.front()->getKernels();
      std::vector<Kernel::a_kernel *>::const_iterator it;
      for (it = versions.begin(); it != versions.end(); it++)
        kernel->addKernel(*it);
    }
  }

  // Build input for static data generation
  MDCG::OpenACC::RegionDesc::input_t input_region;
    input_region.id = 0;
    input_region.file = std::string("kernels.cl");
    input_region.kernel_lists.insert(std::list<Kernel *>(1, kernel));

  MDCG::OpenACC::CompilerData::input_t input;
    input.runtime_dir = SageBuilder::buildVarRefExp("LIBOPENACC_DIR");
    input.ocl_runtime = SageBuilder::buildStringVal("lib/opencl/libopenacc.cl");
    input.kernels_dir = SageBuilder::buildVarRefExp("KERNEL_DIR");
    input.regions.push_back(input_region);

  // Get model element for Region Descriptor
  std::set<MDCG::Model::class_t> classes;
  model_builder.get(model).lookup<MDCG::Model::class_t>("acc_compiler_data_t_", classes);
  assert(classes.size() == 1);
  MDCG::Model::class_t region_desc_class = *(classes.begin());

  // Generate static data
  codegen.addDeclaration<MDCG::OpenACC::CompilerData>(region_desc_class, input, host_data_file_id, "compiler_data");

  // Store static data in data-base
  MDCG::OpenACC::CompilerData::storeToDB(db_file, input);

  project->unparse(); // Cannot call the backend directly because of OpenCL files. There is a warning when trying, just have to trace it.

  return 0;
}

