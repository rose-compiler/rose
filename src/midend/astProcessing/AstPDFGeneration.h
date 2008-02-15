// Author: Markus Schordan
// $Id: AstPDFGeneration.h,v 1.3 2007/02/19 16:08:38 vuduc2 Exp $

#ifndef ASTPDFGENERATION_H
#define ASTPDFGENERATION_H

#include <iostream>
#include "PDFGeneration.h"
#include "AstNodeVisitMapping.h"

class AstPDFGeneration : public PDFGeneration {
public:
  virtual void generate(std::string filename, SgNode* node);
  void generateInputFiles(SgProject* projectNode);
  void generateWithinFile(const std::string& filename, SgFile* node); // ****
  void generateWithinFile(SgFile* node); // ****
 protected:
  PDFInheritedAttribute evaluateInheritedAttribute(SgNode* node, PDFInheritedAttribute inheritedValue);
  void edit_page(SgNode* node, PDFInheritedAttribute inheritedValue);
  AstNodeVisitMapping::MappingType addrPageMapping;
};

#endif
