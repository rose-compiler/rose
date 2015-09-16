#ifndef HH_FINDONEPRAGMA_HH
#define HH_FINDONEPRAGMA_HH

#include <iostream>
#include <utility>
#include <string>
#include "rose.h"

typedef std::pair<SgPragmaDeclaration*, SgPragmaDeclaration*> pragma_pair;
void initGraph(SgProject*);
pragma_pair get_named_pragma(SgProject*,std::string);
int findPragmaPaths(pragma_pair,SgProject*);
#endif

