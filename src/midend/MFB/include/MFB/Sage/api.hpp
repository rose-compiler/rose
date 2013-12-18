/** 
 * \file MFB/include/MFB/api.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef _MFB_API_HPP_
#define _MFB_API_HPP_

#include <set>

class SgNamespaceSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgVariableSymbol;
class SgMemberFunctionSymbol;

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_api
 * @{
*/

struct api_t {
  std::set<SgNamespaceSymbol *>      namespace_symbols;
  std::set<SgFunctionSymbol *>       function_symbols;
  std::set<SgClassSymbol *>          class_symbols;
  std::set<SgVariableSymbol *>       variable_symbols;
  std::set<SgMemberFunctionSymbol *> member_function_symbols;
};

void dump_api(const api_t * api);

void merge_api(api_t * base_api, const api_t * api);

/** @} */

}

#endif /* _MFB_API_HPP_ */

