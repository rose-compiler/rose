
#include "DLX/Tooling/language.hpp"
#include "DLX/Tooling/compiler.hpp"

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);

  // Build ROSE project
  SgProject * project = new SgProject::SgProject(args);

  DLX::Tooling::language_t::init();
  DLX::Frontend::Frontend<DLX::Tooling::language_t> frontend;
  assert(frontend.parseDirectives(project));

  DLX::Tooling::compiler_modules_t compiler_modules(project);
  DLX::Compiler::Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t> compiler(compiler_modules);
  assert(compiler.compile(frontend.directives, frontend.graph_entry, frontend.graph_final));

  return backend(project);
}

