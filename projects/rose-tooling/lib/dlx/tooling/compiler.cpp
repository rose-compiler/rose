
#include "DLX/Tooling/compiler.hpp"
#include "DLX/Tooling/language.hpp"

#include "sage3basic.h"

#include <cassert>

namespace DLX {

namespace Tooling {

compiler_modules_t::compiler_modules_t(SgProject * project_) :
  project(project_)
{}

}

namespace Compiler {

template <>
bool Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t>::compile(
  const Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t>::directives_ptr_set_t & directives,
  const Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t>::directives_ptr_set_t & graph_entry,
  const Compiler<DLX::Tooling::language_t, DLX::Tooling::compiler_modules_t>::directives_ptr_set_t & graph_final
) {
  return true;
}

}

}

