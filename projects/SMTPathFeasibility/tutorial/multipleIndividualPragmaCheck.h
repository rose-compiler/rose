#ifndef HH_MULTIPLEINDIVIDUALPRAGMACHECK_HH
#define HH_MULTIPLEINDIVIDUALPRAGMACHECK_HH

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include "rose.h"

typedef std::pair<SgPragmaDeclaration*, SgPragmaDeclaration*> pragma_pair;
void initGraph(SgProject*);
pragma_pair get_named_pragma(SgProject*,std::string);
int findPragmaPaths(pragma_pair,SgProject*);
void disp_header();

#endif

