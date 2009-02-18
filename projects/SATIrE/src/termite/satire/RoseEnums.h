/* -*- C++ -*-
Copyright 2009 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#ifndef ROSEENUMS_H_
#define ROSEENUMS_H_

#include <map>
#include <string>
#include <vector>

/* This is needed for the SgBitVector translation and should help to
   keep the information at one location for both directions */
class RoseEnums {
 public:
  RoseEnums();

  // ROSE -> TERMITE
  std::vector<std::string> function_modifiers;
  std::vector<std::string> special_function_modifiers;
  std::vector<std::string> type_modifiers;
  std::vector<std::string> declaration_modifiers;
  std::vector<std::string> storage_modifiers;
  std::vector<std::string> access_modifiers;
  std::vector<std::string> cv_modifiers;

  // TERMITE -> ROSE
  std::map<std::string, int> function_modifier;
  std::map<std::string, int> special_function_modifier;
  std::map<std::string, int> type_modifier;
  std::map<std::string, int> declaration_modifier;
  std::map<std::string, int> storage_modifier;
  std::map<std::string, int> access_modifier;
  std::map<std::string, int> cv_modifier;
};

#endif
