/** 
 * \file MFB/include/MFB/api.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef _MFB_API_HPP_
#define _MFB_API_HPP_

#include "MFB/Sage/driver.hpp"

#include <set>
#include <map>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>

class SgNamespaceSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgEnumSymbol;
class SgVariableSymbol;
class SgMemberFunctionSymbol;
class SgTypedefSymbol;
class SgNonrealSymbol;

class SgType;

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

template <typename CRT>
class API {
  public:
    using a_namespace = SgNamespaceDeclarationStatement *;
    using a_type      = SgType *;
    using a_variable  = SgVariableSymbol *;
    using a_function  = SgFunctionSymbol *;

    void load(Driver<Sage> & driver);
    void display(std::ostream & out) const;

  private:
    static std::string const name;

    // Static fields used to locate and open the header files
    static std::string const cache;
    static std::vector<std::string> const paths;
    static std::vector<std::string> const files;
    static std::vector<std::string> const flags;

    // Object maps used to load API elements into the CRT "sub-class" (introspection of sort)
    static std::map<std::string, a_namespace CRT::* > const namespaces;
    static std::map<std::string, a_type      CRT::* > const types;
    static std::map<std::string, a_variable  CRT::* > const variables;
    static std::map<std::string, a_function  CRT::* > const functions;

    void build_command_line(std::vector<std::string> & cmdline) const;
    void load_headers(Driver<Sage> & driver);
    void load_api(Driver<Sage> & driver);
};

/** @} */

}

#include "MFB/Sage/api.tpp"

#endif /* _MFB_API_HPP_ */

