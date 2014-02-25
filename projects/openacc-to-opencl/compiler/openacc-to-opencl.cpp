
#include "KLT/loop-trees.hpp"
#include "KLT/data.hpp"

#include "KLT/iteration-mapper.hpp"
#include "KLT/loop-mapper.hpp"
#include "KLT/data-flow.hpp"
#include "KLT/cg-config.hpp"
#include "KLT/generator.hpp"
#include "KLT/kernel.hpp"
#include "KLT/mfb-klt.hpp"

#include "KLT/mfb-acc-ocl.hpp"

#include "KLT/dlx-openacc.hpp"

#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"
#include "MDCG/code-generator.hpp"

#include <cassert>

#include "sage3basic.h"

typedef ::KLT::Language::OpenCL Language;
typedef ::KLT::Runtime::OpenACC Runtime;
typedef ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t> Annotation;
typedef ::KLT::LoopTrees<Annotation> LoopTrees;
typedef ::KLT::Kernel<Annotation, Language, Runtime> Kernel;

namespace MDCG {

namespace OpenACC {

struct RegionDesc {
  struct input_t {
    
  };

  static SgExpression * createFieldInitializer(
    const MDCG::CodeGenerator & codegen,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    unsigned file_id
  ) {
    switch (field_id) {
      case 0:
        /// \todo
      case 1:
        /// todo
      default:
        assert(false);
    }
  }
};

}

}

unsigned readOpenaccModel(MDCG::ModelBuilder & model_builder, const std::string & libopenacc_dir) {
  unsigned openacc_model = model_builder.create();

  model_builder.add(openacc_model, "openacc",      libopenacc_dir + "include/OpenACC/",         "h");
  model_builder.add(openacc_model, "data-env",     libopenacc_dir + "include/OpenACC/private",  "h");
  model_builder.add(openacc_model, "debug",        libopenacc_dir + "include/OpenACC/private",  "h");
  model_builder.add(openacc_model, "init",         libopenacc_dir + "include/OpenACC/private",  "h");
  model_builder.add(openacc_model, "kernel",       libopenacc_dir + "include/OpenACC/private",  "h");
  model_builder.add(openacc_model, "loop",         libopenacc_dir + "include/OpenACC/private",  "h");
  model_builder.add(openacc_model, "region",       libopenacc_dir + "include/OpenACC/private",  "h");
  model_builder.add(openacc_model, "runtime",      libopenacc_dir + "include/OpenACC/private",  "h");
  model_builder.add(openacc_model, "compiler",     libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "data-env",     libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "init",         libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "kernel",       libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "loop",         libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "mem-manager",  libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "opencl-debug", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "opencl-init",  libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "region",       libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "runtime",      libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "api",          libopenacc_dir + "include/OpenACC/device",   "cl");

  return openacc_model;
}

int main(int argc, char ** argv) {

  // Arguments
  assert(argc == 4);
  std::string libopenacc_dir(argv[2]);
  std::string opencl_dir(argv[3]);

  // Build a default ROSE project
  SgProject * project = new SgProject::SgProject();
  { // Add default command line to an empty project
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
      arglist.push_back(std::string("-I") + libopenacc_dir + "/include/");
      arglist.push_back(std::string("-I") + opencl_dir + "/include/");
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

  unsigned host_data_file_id = driver.add(boost::filesystem::path(std::string("host_data.c")));
  driver.setUnparsedFile(host_data_file_id);

  // Load OpenACC API for KLT
  KLT::Runtime::OpenACC::loadAPI(driver, libopenacc_dir + "/include/");

  // Create a Code Generation Configuration
  KLT::CG_Config<Annotation, Language, Runtime> cg_config(
      new KLT::LoopMapper<Annotation, Language, Runtime>(),
      new KLT::IterationMapper<Annotation, Language, Runtime>(),
      new KLT::DataFlow<Annotation, Language, Runtime>()
  );

  // Call the generator
  std::set<std::list<Kernel *> > kernel_lists;
  generator.generate(loop_trees, kernel_lists, cg_config);

  // Get model element for Region Descriptor
  std::set<MDCG::Model::class_t> classes;
  model_builder.get(model).lookup<MDCG::Model::class_t>("acc_region_desc_t_", classes);
  assert(classes.size() == 1);
  MDCG::Model::class_t region_desc_class = *(classes.begin());

  MDCG::OpenACC::RegionDesc::input_t input;

  codegen.addDeclaration<MDCG::OpenACC::RegionDesc>(region_desc_class, input, host_data_file_id, "regions");
/*
  std::set<std::list<Kernel *> >::const_iterator it_kernel_list;
  std::list<Kernel *>::const_iterator it_kernel;
  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    for (it_kernel = it_kernel_list->begin(); it_kernel != it_kernel_list->end(); it_kernel++) {
      Kernel * kernel = *it_kernel;
      const std::vector<Kernel::a_kernel *> & kernels = kernel->getKernels();

      /// \todo print info about generated kernel
    }
  }
*/
  project->unparse(); // Cannot call the backend directly because of OpenCL files. There is a warning when trying, just have to trace it.

  return 0;
}

