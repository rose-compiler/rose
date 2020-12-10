/** 
 * \file MFB/include/MFB/api.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef _MFB_API_HPP_
#define _MFB_API_HPP_

#include <set>
#include <map>
#include <boost/filesystem.hpp>

class SgNamespaceSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgEnumSymbol;
class SgVariableSymbol;
class SgMemberFunctionSymbol;
class SgTypedefSymbol;
class SgNonrealSymbol;

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_api
 * @{
*/

struct api_t {
  std::set<SgNamespaceSymbol *>      namespace_symbols;
  std::set<SgFunctionSymbol *>       function_symbols;
  std::set<SgClassSymbol *>          class_symbols;
  std::set<SgEnumSymbol *>           enum_symbols;
  std::set<SgVariableSymbol *>       variable_symbols;
  std::set<SgMemberFunctionSymbol *> member_function_symbols;
  std::set<SgTypedefSymbol *>        typedef_symbols;
  std::set<SgNonrealSymbol *>        nonreal_symbols;
};

struct symbol_map_t {
  std::map<std::string, SgNamespaceSymbol *>      namespace_map;
  std::map<std::string, SgFunctionSymbol *>       function_map;
  std::map<std::string, SgClassSymbol *>          class_map;
  std::map<std::string, SgEnumSymbol *>           enum_map;
  std::map<std::string, SgVariableSymbol *>       variable_map;
  std::map<std::string, SgMemberFunctionSymbol *> member_function_map;
  std::map<std::string, SgTypedefSymbol *>        typedef_map;
  std::map<std::string, SgNonrealSymbol *>        nonreal_map;

  symbol_map_t(api_t * api);
};

void dump_api(const api_t * api);

void merge_api(api_t * base_api, const api_t * api);

/** @} */

}

#endif /* _MFB_API_HPP_ */

