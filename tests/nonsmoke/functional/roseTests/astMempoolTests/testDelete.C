
#include "sage3basic.h"
#include "Rose/AST/io.h"

int main_to_repeat(std::vector<std::string> & args) {
  SgProject * project = args.size() > 1 ? frontend(args) : new SgProject(); // TODO this behavior should be part of ::frontend(std::vector<std::string> const &)
  auto status = backend(project);
  Rose::AST::IO::free();
  return status;
}

int main( int argc, char * argv[] ) {
  ROSE_INITIALIZE;
  std::vector<std::string> args(argv, argv+argc);
  auto status = main_to_repeat(args);
  status += main_to_repeat(args);
  return status;
}

