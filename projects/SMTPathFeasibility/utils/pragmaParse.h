#ifndef HH_PRAGMAPARSE_HH
#define HH_PRAGMAPARSE_HH

#include <string>
#include <vector>
#include <algorithm>
#include "rose.h"

std::string get_pragma_string(SgPragmaDeclaration* prdecl);
std::string get_name(std::string pstring);
std::string get_position(std::string prag_str);
std::vector<std::string> get_assumptions(std::string prag_str);
//void remove_white_space(std::string);
//std::vector<std::pair<std::string,std::string> > defined_vars;
#endif
