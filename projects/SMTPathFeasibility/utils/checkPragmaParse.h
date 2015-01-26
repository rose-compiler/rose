#ifndef HH_CHECKPRAGMAPARSE_HH 
#define HH_CHECKPRAGMAPARSE_HH
#include <vector>
#include "rose.h"
#include <string>
#include <iostream>


std::string get_pragma_string(SgPragmaDeclaration*);
std::string get_position(std::string);
std::string get_name(std::string);
std::vector<std::pair<std::string, std::string> >  get_vars(std::string);
std::vector<std::string> get_assumptions(std::string);
bool isSMTGeneral(std::string);
std::string parseSMTGeneral(std::string);
#endif
