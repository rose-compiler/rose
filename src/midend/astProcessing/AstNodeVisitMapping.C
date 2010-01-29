
// Author: Markus Schordan
// $Id: AstNodeVisitMapping.C,v 1.2 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTNODEVISITMAPPING_C
#define ASTNODEVISITMAPPING_C

#include "sage3basic.h"
#include <sstream>
#include "AstNodeVisitMapping.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void
AstNodeVisitMapping::visit(SgNode* node) {
  ostringstream ss;
  ss << node;
  address_pagenum.insert(MappingPairType(ss.str(),pagenum));
  pagenum++;
}  

#endif
