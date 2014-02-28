/** 
 * \file MDCG/lib/code-generator.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MDCG/code-generator.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include <cassert>

namespace MDCG {

/*!
 * \addtogroup grp_codegen
 * @{
*/

unsigned CodeGenerator::s_var_gen_cnt = 0;

CodeGenerator::CodeGenerator(MFB::Driver<MFB::Sage> & mfb_driver) :
  p_mfb_driver(mfb_driver)
{}

MFB::Driver<MFB::Sage> & CodeGenerator::getDriver() const {
  return p_mfb_driver;
}

SgVariableSymbol * CodeGenerator::instantiateDeclaration(std::string decl_name, unsigned file_id, SgType * type, SgInitializer * init) const {

  MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(decl_name, type, init, NULL, file_id, false, true);
  MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = p_mfb_driver.build<SgVariableDeclaration>(var_decl_desc);

  return var_decl_res.symbol;
}

/** @} */

}
