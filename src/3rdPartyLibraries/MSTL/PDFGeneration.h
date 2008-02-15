// Author: Markus Schordan
// $Id: PDFGeneration.h,v 1.2 2006/04/24 00:21:27 dquinlan Exp $

#ifndef PDFGENERATION_H
#define PDFGENERATION_H

#include <iostream>
#include <typeinfo>
#include "AstProcessing.h"
#include "pdflib.h"

class PDFInheritedAttribute {
 public:
  PDFInheritedAttribute():parent(0) {};
  ~PDFInheritedAttribute() {};
  PDFInheritedAttribute(unsigned int p):parent(p) {};
  unsigned int parent;
};

namespace PDFGenerationHelpers {
}

class PDFGeneration : public SgTopDownProcessing<PDFInheritedAttribute> {
public:
  PDFGeneration(): pdfFile(NULL) {}
  virtual void generate(std::string filename, SgNode* node);
protected:
  virtual PDFInheritedAttribute evaluateInheritedAttribute(SgNode* node, PDFInheritedAttribute inheritedValue);
  PDF* pdfFile;
  int topMargin;
  int leftMargin;

  std::string text_page(SgNode* node);
  virtual void edit_page(SgNode* node, PDFInheritedAttribute inheritedValue);
  void create_textlink(const char* text, int targetpage,int hitboxextender=0);
  void pdf_setup(std::string filename);
  void pdf_finalize();
  void begin_page();
  void end_page();
private:
};

#endif
