
#include "DLX/Logger/language.hpp"
#include "DLX/Logger/compiler.hpp"

int main(int argc, char ** argv) {

  std::vector<std::string> args(argv, argv + argc);

  std::string inc_opt("-I");
  args.push_back(inc_opt + LIBLOGGER_INC_PATH);

  // Build ROSE project
  SgProject * project = new SgProject::SgProject(args);

  DLX::Logger::language_t::init();
  DLX::Frontend::Frontend<DLX::Logger::language_t> frontend;
  assert(frontend.parseDirectives(project));

  frontend.toDot(std::cout);

  DLX::Logger::compiler_modules_t compiler_modules(project, "data.c", LIBLOGGER_INC_PATH);
  DLX::Compiler::Compiler<DLX::Logger::language_t, DLX::Logger::compiler_modules_t> compiler(compiler_modules);
  assert(compiler.compile(frontend.directives, frontend.graph_entry, frontend.graph_final));

  return backend(project);
}

