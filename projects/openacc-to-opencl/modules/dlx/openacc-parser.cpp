
#include "DLX/Core/frontend.hpp"
#include "DLX/Core/compiler.hpp"

#include "DLX/OpenACC/language.hpp"

#include "handles.hpp"
#include "hooks.hpp"
#include "transformation-handler.hpp"

#include "rose.h"

#include <cassert>

int main(int argc, char ** argv) {
  DLX::OpenACC::language_t::init();

  DLX::Frontend::Frontend<DLX::OpenACC::language_t> frontend;
  DLX::Compiler::Compiler<DLX::OpenACC::language_t> compiler;

  SgProject * project = new SgProject(argc, argv);

  assert(frontend.parseDirectives(project));
  assert(compiler.compile(project));

  return backend(project);
}

