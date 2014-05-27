
#include "openacc_spec.hpp"

#include "DLX/Core/frontend.hpp"
#include "DLX/Core/compiler.hpp"

#include "KLT/OpenACC/iteration-mapper.hpp"

#include "MDCG/OpenACC/model.hpp"

#include "DLX/OpenACC/language.hpp"
#include "DLX/OpenACC/compiler.hpp"

#include <cassert>

int main(int argc, char ** argv) {
  // Build ROSE project
  SgProject * project = new SgProject::SgProject(argc, argv);

  // Initialize DLX for OpenACC
  DLX::OpenACC::language_t::init(); 

  DLX::Frontend::Frontend<DLX::OpenACC::language_t> frontend;
  assert(frontend.parseDirectives(project));
  frontend.toDot(std::cout);

  DLX::OpenACC::compiler_modules_t compiler_module(project, "kernels.cl", "host-data.c", "/media/ssd/projects/currents/acc2ocl-multidev/libopenacc/include/");

  DLX::Compiler::Compiler<DLX::OpenACC::language_t, DLX::OpenACC::compiler_modules_t> compiler(compiler_module);
  assert(compiler.compile(frontend.directives, frontend.graph_entry, frontend.graph_final));

  return backend(project);
}

