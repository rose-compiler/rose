
#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"

#include "sage3basic.h"

#include <cassert>

int main(int argc, char ** argv) {

  assert(argc == 3);

  std::string libopenacc_dir(argv[1]);
  std::string opencl_dir(argv[2]);

  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
    arglist.push_back("-c");
    arglist.push_back(std::string("-I") + libopenacc_dir);
    arglist.push_back(std::string("-I") + opencl_dir);
  project->set_originalCommandLineArgumentList(arglist);

  MFB::Driver<MFB::Sage> driver(project);

  MDCG::ModelBuilder model_builder(driver);

  unsigned openacc_model = model_builder.create();

  model_builder.add(openacc_model, "compiler",     libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "region",       libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "kernel",       libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "loop",         libopenacc_dir + "/OpenACC/internal", "h");
  model_builder.add(openacc_model, "api",          libopenacc_dir + "/OpenACC/device",   "cl");

  std::set<MDCG::Model::variable_t>  variable_set;
  std::set<MDCG::Model::function_t>  function_set;
  std::set<MDCG::Model::field_t>     field_set;
  std::set<MDCG::Model::method_t>    method_set;
  std::set<MDCG::Model::type_t>      type_set;
  std::set<MDCG::Model::class_t>     class_set;
  std::set<MDCG::Model::namespace_t> namespace_set;

  std::ofstream ofs;

  ofs.open("openacc_model_kernel_desc.dot");
  assert(ofs.is_open());
  
  model_builder.get(openacc_model).lookup<MDCG::Model::class_t>("acc_kernel_desc_t_", class_set);
  MDCG::Model::toDot(ofs, variable_set, function_set, field_set, method_set, type_set, class_set, namespace_set);

  ofs.close();
  class_set.clear();

  ofs.open("openacc_model_region_desc.dot");
  assert(ofs.is_open());
  
  model_builder.get(openacc_model).lookup<MDCG::Model::class_t>("acc_region_desc_t_", class_set);
  MDCG::Model::toDot(ofs, variable_set, function_set, field_set, method_set, type_set, class_set, namespace_set);

  ofs.close();
  class_set.clear();

  ofs.open("openacc_model_compiler_data.dot");
  assert(ofs.is_open());
  
  model_builder.get(openacc_model).lookup<MDCG::Model::class_t>("acc_compiler_data_t_", class_set);
  MDCG::Model::toDot(ofs, variable_set, function_set, field_set, method_set, type_set, class_set, namespace_set);

  ofs.close();
  class_set.clear();

  return 0;
}

