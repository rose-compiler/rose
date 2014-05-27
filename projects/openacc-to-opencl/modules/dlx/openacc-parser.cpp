
#include "DLX/Core/frontend.hpp"
#include "DLX/Core/compiler.hpp"

#include "DLX/OpenACC/language.hpp"

#include "rose.h"

#include <cassert>

int main(int argc, char ** argv) {
  SgProject * project = new SgProject(argc, argv);

  DLX::OpenACC::language_t::init();

  DLX::Frontend::Frontend<DLX::OpenACC::language_t> frontend;
  DLX::Compiler::Compiler<DLX::OpenACC::language_t> compiler;

  assert(frontend.parseDirectives(project));

  frontend.toDot(std::cout);

  assert(compiler.compile(frontend.directives, frontend.graph_entry, frontend.graph_final));

  return backend(project);
}

