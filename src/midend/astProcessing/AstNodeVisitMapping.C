// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.
#ifdef _MSC_VER
// seems to cause problems under Linux
#include "rose.h"
#endif

// Author: Markus Schordan
// $Id: AstNodeVisitMapping.C,v 1.2 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTNODEVISITMAPPING_C
#define ASTNODEVISITMAPPING_C

#include <sstream>
#include "sage3.h"
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
