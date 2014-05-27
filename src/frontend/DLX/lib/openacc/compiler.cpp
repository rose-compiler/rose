/*!
 * 
 * \file lib/openacc/compiler.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "DLX/Core/compiler.hpp"
#include "DLX/OpenACC/language.hpp"

namespace DLX {

namespace Compiler {

/*!
 * \addtogroup grp_dlx_openacc_compiler
 * @{
 */

template <>
bool Compiler<DLX::OpenACC::language_t>::compile(
  const Compiler<DLX::OpenACC::language_t>::directives_ptr_set_t & directives,
  const Compiler<DLX::OpenACC::language_t>::directives_ptr_set_t & graph_entry,
  const Compiler<DLX::OpenACC::language_t>::directives_ptr_set_t & graph_final
) {
  /// \todo verify that it is correct OpenACC.....

  /// \todo generate LoopTrees from parallel/kernel regions

  return false;
}

/** @} */
}

}

