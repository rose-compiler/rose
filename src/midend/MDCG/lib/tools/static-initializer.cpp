/** 
 * \file MDCG/lib/code-generator.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "sage3basic.h"

#include "MDCG/Tools/static-initializer.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include <cassert>

namespace MDCG {

namespace Tools {

SgVariableSymbol * StaticInitializer::instantiateDeclaration(MFB::Driver<MFB::Sage> & driver, std::string decl_name, size_t file_id, SgType * type, SgInitializer * init) {

  MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(decl_name, type, init, NULL, file_id, false, true);
  MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = driver.build<SgVariableDeclaration>(var_decl_desc);

  return var_decl_res.symbol;
}

}

}
