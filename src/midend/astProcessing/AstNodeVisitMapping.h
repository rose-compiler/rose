// Author: Markus Schordan
// $Id: AstNodeVisitMapping.h,v 1.2 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTNODEVISITMAPPING_H
#define ASTNODEVISITMAPPING_H

#include <map>
#include <string>
#include "AstProcessing.h"

class AstNodeVisitMapping : public SgSimpleProcessing {
public:
  AstNodeVisitMapping(int pn):pagenum(pn) {}
  typedef std::map<std::string,int> MappingType;
  typedef MappingType::value_type MappingPairType;
  MappingType address_pagenum;
  int pagenum;
  void visit(SgNode* node);
};

#endif
