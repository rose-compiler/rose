
#include "openacc_spec.hpp"

#include "KLT/OpenACC/iteration-mapper.hpp"
#include "MDCG/OpenACC/model.hpp"

typedef ::KLT::Language::OpenCL Language;
typedef ::KLT::Runtime::OpenACC Runtime;
typedef ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t> Annotation;
typedef ::KLT::LoopTrees<Annotation> LoopTrees;
typedef ::KLT::Kernel<Annotation, Language, Runtime> Kernel;

void help(std::ostream & out) {
  out << "[Help]  > ---------------------------" << std::endl;
  out << "[Help]  > RoseACC: LoopTree to OpenCL" << std::endl;
  out << "[Help]  > ---------------------------" << std::endl;
  out << "[Help]  > Transforms LoopTree (RoseACC's intermediate format) into OpenCL C kernels." << std::endl;
  out << "[Help]  > It also produces kernel descriptions to be used by libOpenACC (stored in both a database and a C file)." << std::endl;
  out << "[Help]  > ---------------------------" << std::endl;
  out << "[Help]  > Options: (all options should appear at most once)" << std::endl;
  out << "[Help]  >   -i input.lt [...] : Add one or more loop-tree representation for the current kernel." << std::endl;
  out << "[Help]  >   -cl kernels.cl    : Output file for generated OpenCL C kernels (default='kernels.cl')." << std::endl;
  out << "[Help]  >   -hd host-data.c   : Output file for the C descriptions of the generated kernels (default='host-data.c')." << std::endl;
  out << "[Help]  >   -db versions.db   : Database where detailled descriptions of the generated kernels are stored." << std::endl;
  out << "[Help]  >   --tile-size 2 4 8 : Tile sizes to be tried, followed by a list of positive integer." << std::endl;
  out << "[Help]  > ---------------------------" << std::endl;
  out << "[Help]  > Environment Variables:" << std::endl;
  out << "[Help]  >   ROSEACC_LIBOPENACC_INCLUDE_DIR : Path to libOpenACC header files." << std::endl;
  out << "[Help]  >   ROSEACC_OPENCL_INCLUDE_DIR     : Path to OpenCL header files." << std::endl;
}

void init_error(const std::string & err_msg) {
  std::cerr << "[Error] > " << err_msg << std::endl << std::endl;
  help(std::cerr);
  exit(-1);
}

void init(
  int argc, char ** argv,
  std::string & ocl_kernels_file,
  std::string & kernels_desc_file,
  std::string & version_db_file,
  std::string & libopenacc_inc_dir,
  std::vector<std::string> & lt_inputs,
  SgProject * & project,
  std::vector<unsigned> & tile_sizes,
  size_t & region_id
) {
  // Arguments

  region_id = -1;

  int arg_idx = 1;
  while (arg_idx < argc) {
    if (strcmp(argv[arg_idx], "-i") == 0) {
      arg_idx++;
      if (arg_idx >= argc && argv[arg_idx][0] != '-')
        init_error("Missing parameter for -i");

      if (lt_inputs.size() > 0)
        init_error("Option '-i' should only be provided once.");

      do {
        lt_inputs.push_back(std::string(argv[arg_idx]));
        arg_idx++;
      } while (arg_idx >= argc && argv[arg_idx][0] != '-');
    }
    else if (strcmp(argv[arg_idx], "-cl") == 0) {
      arg_idx++;
      if (arg_idx >= argc && argv[arg_idx][0] != '-')
        init_error("Missing parameter for -cl");

      if (ocl_kernels_file.size() > 0)
        init_error("Option '-cl' should only be provided once.");
      ocl_kernels_file = argv[arg_idx];
      arg_idx++;
    }
    else if (strcmp(argv[arg_idx], "-hd") == 0) {
      arg_idx++;
      if (arg_idx >= argc && argv[arg_idx][0] != '-')
        init_error("Missing parameter for -hd");

      if (kernels_desc_file.size() > 0)
        init_error("Option '-hd' should only be provided once.");
      kernels_desc_file = argv[arg_idx];
      arg_idx++;
    }
    else if (strcmp(argv[arg_idx], "-db") == 0) {
      arg_idx++;
      if (arg_idx >= argc && argv[arg_idx][0] != '-')
        init_error("Missing parameter for -db");

      if (version_db_file.size() > 0)
        init_error("Option '-db' should only be provided once.");
      version_db_file = argv[arg_idx];
      arg_idx++;
    }
    else if (strcmp(argv[arg_idx], "--tile-sizes") == 0) {
      arg_idx++;
      if (arg_idx >= argc && argv[arg_idx][0] != '-')
        init_error("Missing parameter for --tile-sizes");

      if (tile_sizes.size() > 0)
        init_error("Option '--tile-sizes' should only be provided once.");

      do {
        tile_sizes.push_back(atoi(argv[arg_idx]));
        arg_idx++;
      } while (arg_idx >= argc && argv[arg_idx][0] != '-');
    }
    else if (strcmp(argv[arg_idx], "--region-id") == 0) {
      arg_idx++;
      if (arg_idx >= argc && argv[arg_idx][0] != '-')
        init_error("Missing parameter for --region-id");

      if (region_id != (size_t)-1)
        init_error("Option '--region-id' should only be provided once.");

      region_id = atoi(argv[arg_idx]);
      arg_idx++;
    }
    else if (strcmp(argv[arg_idx], "-h") == 0) {
      help(std::cout);
      exit(0);
    }
    else init_error("Unrecognized option.");
  }

  if (lt_inputs.size() == 0) init_error("No input file.");

  if (ocl_kernels_file.size()  == 0) ocl_kernels_file  = "kernels.cl";
  if (kernels_desc_file.size() == 0) kernels_desc_file = "host-data.c";
  if (version_db_file.size()   == 0) version_db_file   = "versions.db";

  if (region_id == (size_t)-1) region_id = 0;

  // Environment Variables

  char * env_libopenacc_inc_dir = getenv("ROSEACC_LIBOPENACC_INCLUDE_DIR");
  if (env_libopenacc_inc_dir == NULL || env_libopenacc_inc_dir[0] == '\0')
    init_error("Environment variable ROSEACC_LIBOPENACC_INCLUDE_DIR need to be set");
  libopenacc_inc_dir = env_libopenacc_inc_dir;

  char * env_opencl_include_dir = getenv("ROSEACC_OPENCL_INCLUDE_DIR");
  if (env_opencl_include_dir == NULL || env_opencl_include_dir[0] == '\0')
    init_error("Environment variable ROSEACC_OPENCL_INCLUDE_DIR need to be set");
  std::string opencl_inc_dir = env_opencl_include_dir;

  // Build a default ROSE project
  project = new SgProject::SgProject();
  { // Add default command line to an empty project
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
      arglist.push_back(std::string("-I") + libopenacc_inc_dir);
      arglist.push_back(std::string("-I") + opencl_inc_dir);
      arglist.push_back("-c");
    project->set_originalCommandLineArgumentList (arglist);
  }

  // Initialize DLX for OpenACC
  DLX::OpenACC::language_t::init(); 
}

int main(int argc, char ** argv) {
  std::string ocl_kernels_file;
  std::string kernels_desc_file;
  std::string version_db_file;
  std::string libopenacc_inc_dir;
  std::vector<std::string> lt_inputs;
  SgProject * project;
  std::vector<unsigned> tiling_sizes;
  size_t region_id;
  init(argc, argv, ocl_kernels_file, kernels_desc_file, version_db_file, libopenacc_inc_dir, lt_inputs, project, tiling_sizes, region_id);

  // Initialize MFB to use with KLT
  MFB::KLT_Driver driver(project);

  // Initialize MDCG's ModelBuilder and Code Generator
  MDCG::ModelBuilder model_builder(driver);
  MDCG::CodeGenerator codegen(driver);

  // Initialize KLT's Generator
  KLT::Generator<Annotation, Language, Runtime, MFB::KLT_Driver> generator(driver, ocl_kernels_file);

  // Read OpenACC Model
  unsigned model = MDCG::OpenACC::readOpenaccModel(model_builder, libopenacc_inc_dir);

  unsigned host_data_file_id = driver.add(boost::filesystem::path(kernels_desc_file));
  driver.setUnparsedFile(host_data_file_id);

  // Load OpenACC API for KLT
  KLT::Runtime::OpenACC::loadAPI(driver, libopenacc_inc_dir);

  // Create a Code Generation Configuration
  KLT::CG_Config<Annotation, Language, Runtime> cg_config(
      new KLT::LoopMapper<Annotation, Language, Runtime>(),
      new KLT::OpenACC::IterationMapper(tiling_sizes),
      new KLT::DataFlow<Annotation, Language, Runtime>()
  );

  // Call the generator for each input file and merge the results (Assume that only one kernel is produced)
  Kernel * kernel = NULL;
  std::vector<std::string>::const_iterator it_input;
  for (it_input = lt_inputs.begin(); it_input != lt_inputs.end(); it_input++) {

    // Read input LoopTrees
    LoopTrees loop_trees;
    loop_trees.read(*it_input);

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
  assert(kernel != NULL);

  // Build input for static data generation
  MDCG::OpenACC::RegionDesc::input_t input_region;
    input_region.id = region_id;
    input_region.file = ocl_kernels_file;
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
  MDCG::OpenACC::CompilerData::storeToDB(version_db_file, input);

  project->unparse(); // Cannot call the backend directly because of OpenCL files. There is a warning when trying, just have to trace it.

  return 0;
}

