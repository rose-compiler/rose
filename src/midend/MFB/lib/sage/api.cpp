/** 
 * \file lib/sage/class-declaration.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MFB/Sage/api.hpp"

#include "sage3basic.h"

#include <iostream>

namespace MFB {

void dump_api(const api_t * api) {
  std::set<SgNamespaceSymbol *>::const_iterator it_namespace_symbol;
  std::cout << "Namespaces:" << std::endl; 
  for (it_namespace_symbol = api->namespace_symbols.begin(); it_namespace_symbol != api->namespace_symbols.end(); it_namespace_symbol++) {
    std::cout << "  " << (*it_namespace_symbol)->get_name() << std::endl;
  }

  std::set<SgFunctionSymbol *>::const_iterator it_function_symbol;
  std::cout << "Functions:" << std::endl;
  for (it_function_symbol = api->function_symbols.begin(); it_function_symbol != api->function_symbols.end(); it_function_symbol++) {
    std::cout << "  " << (*it_function_symbol)->get_name() << std::endl;
  }

  std::set<SgClassSymbol *>::const_iterator it_class_symbol;
  std::cout << "Classes:" << std::endl;
  for (it_class_symbol = api->class_symbols.begin(); it_class_symbol != api->class_symbols.end(); it_class_symbol++) {
    std::cout << "  " << (*it_class_symbol)->get_name() << std::endl;
  }

  std::set<SgVariableSymbol *>::const_iterator it_variable_symbol;
  std::cout << "Variables:" << std::endl;
  for (it_variable_symbol = api->variable_symbols.begin(); it_variable_symbol != api->variable_symbols.end(); it_variable_symbol++) {
    std::cout << "  " << (*it_variable_symbol)->get_name() << std::endl;
  }

  std::set<SgMemberFunctionSymbol *>::const_iterator it_member_function_symbol;
  std::cout << "Member Functions:" << std::endl;
  for (it_member_function_symbol = api->member_function_symbols.begin(); it_member_function_symbol != api->member_function_symbols.end(); it_member_function_symbol++) {
    std::cout << "  " << (*it_member_function_symbol)->get_name() << std::endl;    
  }
}

void merge_api(api_t * base_api, const api_t * api) {
  base_api->namespace_symbols.insert(api->namespace_symbols.begin(), api->namespace_symbols.end());
  base_api->function_symbols.insert(api->function_symbols.begin(), api->function_symbols.end());
  base_api->class_symbols.insert(api->class_symbols.begin(), api->class_symbols.end());
  base_api->variable_symbols.insert(api->variable_symbols.begin(), api->variable_symbols.end());
  base_api->member_function_symbols.insert(api->member_function_symbols.begin(), api->member_function_symbols.end());
}

}

