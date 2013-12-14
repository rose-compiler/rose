/*!
 * \addtogroup grp_rose_acc2ocl
 * @{
 * 
 * \file openacc-to-opencl.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

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
  Handles::TransformationHandler transformation_handler;
  DLX::OpenACC::language_t::compiler_module_handlers_t compiler_module_handlers;
//DLX::Compiler::Compiler<DLX::OpenACC::language_t, Handles::TransformationHandler> compiler(transformation_handler, compiler_module_handlers);

  SgProject * project = new SgProject(argc, argv);

  assert(frontend.parseDirectives(project));
//assert(compiler.compile(project));

//assert(transformation_handler.process());

  return backend(project);
}

/** @} */

