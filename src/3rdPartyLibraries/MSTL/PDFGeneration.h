// Author: Markus Schordan
// $Id: PDFGeneration.h,v 1.2 2006/04/24 00:21:27 dquinlan Exp $

#ifndef PDFGENERATION_H
#define PDFGENERATION_H

#include <iostream>
#include <typeinfo>
#include "AstProcessing.h"

#undef PACKAGE
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include "hpdf.h"

class PDFInheritedAttribute {
 public:
  explicit PDFInheritedAttribute(HPDF_Outline currentOutline, HPDF_Destination parentPage):currentOutline(currentOutline), parentPage(parentPage) {}
  explicit PDFInheritedAttribute(HPDF_Doc pdfFile):currentOutline(NULL), parentPage(NULL) {}
  ~PDFInheritedAttribute() {}
  HPDF_Outline currentOutline;
  HPDF_Destination parentPage;
};

class PDFGeneration : public SgTopDownProcessing<PDFInheritedAttribute> {
public:
  PDFGeneration(): currentPageNumber(0), topMargin(0), leftMargin(0) {}
  virtual void generate(std::string filename, SgNode* node);
protected:
  virtual PDFInheritedAttribute evaluateInheritedAttribute(SgNode* node, PDFInheritedAttribute inheritedValue);
  HPDF_Doc pdfFile;
  HPDF_Font theFont;
  std::vector<HPDF_Page> pages;
  std::vector<HPDF_Destination> pageDests;
  std::string filename;
  size_t currentPageNumber;
  HPDF_Page currentPage;
  int topMargin;
  int leftMargin;
  HPDF_Rect fontBBox;

  std::string text_page(SgNode* node);
  virtual void edit_page(size_t pageNumber, SgNode* node, PDFInheritedAttribute inheritedValue);
  virtual std::string get_bookmark_name(SgNode* node);
  void create_textlink(const std::string& text, HPDF_Destination target,int hitboxextender=0);
  void pdf_setup(std::string filename, size_t numPages);
  void pdf_finalize();
  void begin_page();
  void end_page();
private:
};

#endif
