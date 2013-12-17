/** 
 * \file MFB/include/MFB.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef _MFB_API_HPP_
#define _MFB_API_HPP_

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_api
 * @{
*/

struct API {
  std::set<SgNamespaceSymbol *>      namespace_symbols;
  std::set<SgFunctionSymbol *>       function_symbols;
  std::set<SgClassSymbol *>          class_symbols;
  std::set<SgVariableSymbol *>       variable_symbols;
  std::set<SgMemberFunctionSymbol *> member_function_symbols;
};

void dump_api(const API * api);

void merge_api(API * base_api, const API * api);

/** @} */

}

#endif /* _MFB_API_HPP_ */

