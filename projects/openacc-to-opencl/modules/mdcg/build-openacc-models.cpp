
#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"

#include "sage3basic.h"

#include <cassert>

/**
 * \note The test files represent sequences of call to either ModelBuilder::addOne and ModelBuilder::addPair.
 *       Each line starts with the method to call followed by the arguments.
 */

int main(int argc, char ** argv) {

  assert(argc == 3);

  std::string libopenacc_dir(argv[1]);
  std::string opencl_dir(argv[2]);

  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
    arglist.push_back("-c");
    arglist.push_back(std::string("-I") + libopenacc_dir + "/include/");
    arglist.push_back(std::string("-I") + opencl_dir + "/include/");
  project->set_originalCommandLineArgumentList(arglist);

  MFB::Driver<MFB::Sage> driver(project);

  MDCG::ModelBuilder model_builder(driver);

  std::ifstream data;
  data.open(argv[1]);
  assert(data.is_open());

  unsigned openacc_model = model_builder.create();

  model_builder.add(openacc_model, "openacc", libopenacc_dir + "include/OpenACC/", "h");
  model_builder.add(openacc_model, "data-env", libopenacc_dir + "include/OpenACC/private", "h");
  model_builder.add(openacc_model, "debug", libopenacc_dir + "include/OpenACC/private", "h");
  model_builder.add(openacc_model, "init", libopenacc_dir + "include/OpenACC/private", "h");
  model_builder.add(openacc_model, "kernel", libopenacc_dir + "include/OpenACC/private", "h");
  model_builder.add(openacc_model, "loop", libopenacc_dir + "include/OpenACC/private", "h");
  model_builder.add(openacc_model, "region", libopenacc_dir + "include/OpenACC/private", "h");
  model_builder.add(openacc_model, "runtime", libopenacc_dir + "include/OpenACC/private", "h");
  model_builder.add(openacc_model, "compiler", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "data-env", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "init", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "kernel", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "loop", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "mem-manager", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "opencl-debug", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "opencl-init", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "region", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "runtime", libopenacc_dir + "include/OpenACC/internal", "h");
  model_builder.add(openacc_model, "api", libopenacc_dir + "include/OpenACC/device", "cl");

  std::ofstream ofs;

  ofs.open("openacc_model_var.dot");
  assert(ofs.is_open());
  model_builder.get(openacc_model).toDot(ofs, "acc_", true, false, false);
  ofs.close();

  ofs.open("openacc_model_func.dot");
  assert(ofs.is_open());
  model_builder.get(openacc_model).toDot(ofs, "acc_", false, true, false);
  ofs.close();

  ofs.open("openacc_model_class.dot");
  assert(ofs.is_open());
  model_builder.get(openacc_model).toDot(ofs, "acc_", false, false, true);
  ofs.close();

  ofs.open("openacc_model_kernel.dot");
  assert(ofs.is_open());
  model_builder.get(openacc_model).toDot(ofs, "acc_kernel", false, false, true);
  ofs.close();

  ofs.open("openacc_model_region.dot");
  assert(ofs.is_open());
  model_builder.get(openacc_model).toDot(ofs, "acc_region", false, false, true);
  ofs.close();

  ofs.open("openacc_model_loop.dot");
  assert(ofs.is_open());
  model_builder.get(openacc_model).toDot(ofs, "acc_loop", false, false, true);
  ofs.close();

  ofs.open("openacc_model_kernel_desc.dot");
  assert(ofs.is_open());
  model_builder.get(openacc_model).toDot(ofs, "acc_kernel_desc", false, false, true);
  ofs.close();

  ofs.open("openacc_model_region_desc.dot");
  assert(ofs.is_open());
  model_builder.get(openacc_model).toDot(ofs, "acc_region_desc", false, false, true);
  ofs.close();

  return 0;
}

