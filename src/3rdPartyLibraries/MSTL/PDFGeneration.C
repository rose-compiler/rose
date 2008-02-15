// Author: Markus Schordan
// $Id: PDFGeneration.C,v 1.4 2008/01/08 02:56:19 dquinlan Exp $

#ifndef PDFGENERATION_C
#define PDFGENERATION_C

// DQ (3/6/2003): added from AstProcessing.h to avoid referencing
// the traversal classes in AstFixes.h before they are defined.
#include "sage3.h"
#include "roseInternal.h"

#include "PDFGeneration.h"

template<class NodeType> 
PDFGeneration<NodeType>::PDFGeneration() : pdfFile(NULL) {
}

template<class NodeType> 
void PDFGeneration<NodeType>::generate(std::string filename, NodeType* node) {
  PDFInheritedAttribute pdfIA;
  std::string pdffilename=filename+".pdf";
  std::cout << "generating PDF file: " << pdffilename << " ... ";
  pdf_setup(pdffilename);
  traverse(node,pdfIA);
  pdf_finalize();
  std::cout << "done (full AST)." << std::endl;
}


template<class NodeType>
PDFInheritedAttribute
PDFGeneration<NodeType>::evaluateInheritedAttribute(NodeType* node, PDFInheritedAttribute inheritedValue) {
  int bookmark;

  std::string stext=text_page(node);
  const char* text=stext.c_str();
  begin_page();
  PDF_show(pdfFile, text);
  std::string bookmarktext=typeid(node).name();
  edit_page(node, inheritedValue);
  bookmark = PDF_add_bookmark(pdfFile, bookmarktext.c_str(), inheritedValue.parent, 1);
  end_page();
  PDFInheritedAttribute ia(bookmark);
  inheritedValue=ia;

  return inheritedValue;;
}

// Setup PDF file
template<class NodeType>
void PDFGeneration<NodeType>::pdf_setup(std::string filename)
{

  char* pdffilename=const_cast<char*>(filename.c_str());
  PDF_boot(); // Initialize the PDFLib library
  pdfFile = PDF_new(); // Build a PDF file

  // Open the pdf file for writing
  if (PDF_open_file(pdfFile, pdffilename) == -1) {
    std::cerr << "Couldn't open PDF file \"" << pdffilename << "\"" << std::endl;
    ROSE_ABORT();
  }
  
  // Initialize properties stored in PDF file
  PDF_set_info(pdfFile, "Keywords", "Abstract Syntax Tree (AST) ROSE");
  PDF_set_info(pdfFile, "Subject", "Display of AST for ROSE");
  PDF_set_info(pdfFile, "Title", "AST for program code");
  PDF_set_info(pdfFile, "Creator", "ROSE");
  PDF_set_info(pdfFile, "Author", "generic");
}

template<class NodeType>
void PDFGeneration<NodeType>::begin_page()
{
  // Specify the page size (can be much larger than letter size)
  PDF_begin_page(pdfFile,letter_width, letter_height);

  // use default coordinates
  float fontSize = 10; // measured in pt
  int font = PDF_findfont(pdfFile, "Helvetica", "host", 0);
  // setup the font to be useed in this file
  PDF_setfont(pdfFile, font, fontSize);
  PDF_set_text_pos(pdfFile,0,letter_height);
  PDF_setrgbcolor(pdfFile, 0, 0, 1); // set some color for the first text
}

template<class NodeType>
std::string PDFGeneration<NodeType>::text_page(NodeType* node)
{
  return "";
}

template<class NodeType>
void PDFGeneration<NodeType>::edit_page(NodeType *node, PDFInheritedAttribute inheritedValue)
{
}

template<class NodeType>
void PDFGeneration<NodeType>::create_textlink(const char* text, int targetpage, int hitboxextender)
{
  int font=(int)PDF_get_value(pdfFile,"font",0.0);
  double fontsize=PDF_get_value(pdfFile,"fontsize",0.0);
  double textlength=PDF_stringwidth(pdfFile,text,font,fontsize);
  double textxpos=PDF_get_value(pdfFile,"textx",0.0);
  double textypos=PDF_get_value(pdfFile,"texty",0.0); 
  // add local link rectangle
  int lowerleftx=textxpos-hitboxextender;
  int lowerlefty=textypos+fontsize-2+hitboxextender;
  int upperrightx=textxpos+textlength+hitboxextender;
  int upperrighty=textypos-2-hitboxextender;
  PDF_add_locallink(pdfFile, lowerleftx, lowerlefty, upperrightx, upperrighty,targetpage,"retain");
  // add text
  //PDF_set_parameter(pdfFile,"underline","true");
  PDF_show(pdfFile, text);
  //PDF_set_parameter(pdfFile,"underline","false");
}

template<class NodeType>
void PDFGeneration<NodeType>::end_page()
{
  PDF_end_page(pdfFile);
}

// Close pdf file representing Sage's AST
template<class NodeType>
void PDFGeneration<NodeType>::pdf_finalize()
{
  // Now close the PDF file
  PDF_close(pdfFile);
  PDF_delete(pdfFile);
  
  // finalize the last use of the PDF library
  PDF_shutdown();
}

#endif
