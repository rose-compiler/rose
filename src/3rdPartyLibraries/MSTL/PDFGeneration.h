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

template <class NodeType>
class PDFGeneration : public SgTopDownProcessing<PDFInheritedAttribute> {
public:
  PDFGeneration();
  virtual void generate(std::string filename, NodeType* node);
protected:
  virtual std::string text_page(NodeType* node);
  virtual void edit_page(NodeType* node, PDFInheritedAttribute inheritedValue);
  PDFInheritedAttribute evaluateInheritedAttribute(NodeType* node, PDFInheritedAttribute inheritedValue);
  void create_textlink(const char* text, int targetpage,int hitboxextender=0);
  PDF* pdfFile;
  void pdf_setup(std::string filename);
  void pdf_finalize();
  void begin_page();
  void end_page();
  int topMargin;
  int leftMargin;
private:
};

// #ifdef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 
   #include "PDFGeneration.C" 
// #endif 

#endif
