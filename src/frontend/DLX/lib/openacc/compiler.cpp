
#include "DLX/Core/compiler.hpp"
#include "DLX/OpenACC/language.hpp"

#include "transformation-handler.hpp"

namespace DLX {

namespace Compiler {

template <>
Compiler<OpenACC::language_t, Handles::TransformationHandler>::Compiler(
  Handles::TransformationHandler & transformation_handler,
  OpenACC::language_t::compiler_module_handlers_t & module_handlers
) :
  p_transformation_handler(transformation_handler),
  p_module_handlers(module_handlers)
{}

template <>
bool Compiler<DLX::OpenACC::language_t, Handles::TransformationHandler>::compile(SgNode * node) {
  // TODO Compiler<DLX::OpenACC::language_t, Handles::TransformationHandler>::compile(SgNode * node)
  return false;
}

}

}

