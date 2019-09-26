/** 
 * \file lib/sage/class-declaration.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MFB/Sage/api.hpp"

#include "sage3basic.h"

#include <boost/filesystem.hpp>

#include <iostream>

namespace MFB {

void dump_api(const api_t * api) {
  std::cout << "Namespaces:" << std::endl; 
  for (std::set<SgNamespaceSymbol *>::const_iterator it = api->namespace_symbols.begin(); it != api->namespace_symbols.end(); it++) {
    SgNamespaceSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);
    std::cout << "  " << sym->get_name() << " ( " << sym->class_name() << " ) " << std::endl;
  }

  std::cout << "Functions:" << std::endl;
  for (std::set<SgFunctionSymbol *>::const_iterator it = api->function_symbols.begin(); it != api->function_symbols.end(); it++) {
    SgFunctionSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);
    std::cout << "  " << sym->get_name() << " ( " << sym->class_name() << " ) " << std::endl;
  }

  std::cout << "Classes:" << std::endl;
  for (std::set<SgClassSymbol *>::const_iterator it = api->class_symbols.begin(); it != api->class_symbols.end(); it++) {
    SgClassSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);
    std::cout << "  " << sym->get_name() << " ( " << sym->class_name() << " ) " << std::endl;
  }

  std::cout << "Enumerations:" << std::endl;
  for (std::set<SgEnumSymbol *>::const_iterator it = api->enum_symbols.begin(); it != api->enum_symbols.end(); it++) {
    SgEnumSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);
    std::cout << "  " << sym->get_name() << " ( " << sym->class_name() << " ) " << std::endl;
  }

  std::cout << "Variables:" << std::endl;
  for (std::set<SgVariableSymbol *>::const_iterator it = api->variable_symbols.begin(); it != api->variable_symbols.end(); it++) {
    SgVariableSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);
    std::cout << "  " << sym->get_name() << " ( " << sym->class_name() << " ) " << std::endl;
  }

  std::cout << "Member Functions:" << std::endl;
  for (std::set<SgMemberFunctionSymbol *>::const_iterator it = api->member_function_symbols.begin(); it != api->member_function_symbols.end(); it++) {
    SgMemberFunctionSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);
    std::cout << "  " << sym->get_name() << " ( " << sym->class_name() << " ) " << std::endl;  
  }

  std::cout << "Typedef:" << std::endl;
  for (std::set<SgTypedefSymbol *>::const_iterator it = api->typedef_symbols.begin(); it != api->typedef_symbols.end(); it++) {
    SgTypedefSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);
    std::cout << "  " << sym->get_name() << " ( " << sym->class_name() << " ) " << std::endl;
  }

  std::cout << "Nonreal:" << std::endl;
  for (std::set<SgNonrealSymbol *>::const_iterator it = api->nonreal_symbols.begin(); it != api->nonreal_symbols.end(); it++) {
    SgNonrealSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);
    std::cout << "  " << sym->get_name() << " ( " << sym->class_name() << " ) " << std::endl;
  }
}

void merge_api(api_t * base_api, const api_t * api) {
  base_api->namespace_symbols.insert(api->namespace_symbols.begin(), api->namespace_symbols.end());
  base_api->function_symbols.insert(api->function_symbols.begin(), api->function_symbols.end());
  base_api->class_symbols.insert(api->class_symbols.begin(), api->class_symbols.end());
  base_api->enum_symbols.insert(api->enum_symbols.begin(), api->enum_symbols.end());
  base_api->variable_symbols.insert(api->variable_symbols.begin(), api->variable_symbols.end());
  base_api->member_function_symbols.insert(api->member_function_symbols.begin(), api->member_function_symbols.end());
  base_api->typedef_symbols.insert(api->typedef_symbols.begin(), api->typedef_symbols.end());
  base_api->nonreal_symbols.insert(api->nonreal_symbols.begin(), api->nonreal_symbols.end());
}

}

