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
  std::vector<std::string> elaborated_type_modifiers;
  std::vector<std::string> declaration_modifiers;
  std::vector<std::string> storage_modifiers;
  std::vector<std::string> access_modifiers;
  std::vector<std::string> upc_access_modifiers;
  std::vector<std::string> cv_modifiers;
  std::vector<std::string> class_types;
  std::vector<std::string> throw_kinds;
  std::vector<std::string> cast_types;
  std::vector<std::string> static_flags;
  std::vector<std::string> ellipses_flags;

  // TERMITE -> ROSE
  std::map<std::string, int> function_modifier;
  std::map<std::string, int> special_function_modifier;
  std::map<std::string, int> type_modifier;
  std::map<std::string, int> elaborated_type_modifier;
  std::map<std::string, int> declaration_modifier;
  std::map<std::string, int> storage_modifier;
  std::map<std::string, int> access_modifier;
  std::map<std::string, int> upc_access_modifier;
  std::map<std::string, int> cv_modifier;
  std::map<std::string, int> class_type;
  std::map<std::string, int> throw_kind;
  std::map<std::string, int> cast_type;
  std::map<std::string, int> static_flag;
  std::map<std::string, int> ellipses_flag;
};

#endif
