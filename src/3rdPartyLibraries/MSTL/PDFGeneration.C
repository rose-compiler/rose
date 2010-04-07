// Author: Markus Schordan
// $Id: PDFGeneration.C,v 1.4 2008/01/08 02:56:19 dquinlan Exp $

// DQ (3/6/2003): added from AstProcessing.h to avoid referencing
// the traversal classes in AstFixes.h before they are defined.
#include "sage3basic.h"
#include "roseInternal.h"

#include "PDFGeneration.h"

#include <iostream>
#include <iomanip>
using namespace std;

static void pdfErrorHandler(HPDF_STATUS errorNumber, HPDF_STATUS detail, void*) {
  cerr << "HPDF error 0x" << hex << errorNumber << " detail " << detail << endl;
  abort();
}

// Setup PDF file
void PDFGeneration::pdf_setup(std::string filename, size_t numPages)
{

  pdfFile = HPDF_New(&pdfErrorHandler, NULL); // Build a PDF file
  theFont = HPDF_GetFont(pdfFile, "Helvetica", NULL);
  this->filename = filename;
  HPDF_SetPagesConfiguration(pdfFile, 100);
  HPDF_SetPageMode(pdfFile, HPDF_PAGE_MODE_USE_OUTLINE);
  HPDF_SetCompressionMode(pdfFile, HPDF_COMP_ALL);

  // Initialize properties stored in PDF file
  HPDF_SetInfoAttr(pdfFile, HPDF_INFO_KEYWORDS, "Abstract Syntax Tree (AST) ROSE");
  HPDF_SetInfoAttr(pdfFile, HPDF_INFO_SUBJECT, "Display of AST for ROSE");
  HPDF_SetInfoAttr(pdfFile, HPDF_INFO_TITLE, "AST for program code");
  HPDF_SetInfoAttr(pdfFile, HPDF_INFO_CREATOR, "ROSE");
  HPDF_SetInfoAttr(pdfFile, HPDF_INFO_AUTHOR, "generic");

  for (size_t i = 0; i < numPages; ++i) {
    HPDF_Page page = HPDF_AddPage(pdfFile);
    HPDF_Page_SetWidth(page, 8.5 * 72);
    HPDF_Page_SetHeight(page, 11 * 72);
    this->pages.push_back(page);
    this->pageDests.push_back(HPDF_Page_CreateDestination(page));
  }
}

void PDFGeneration::begin_page()
{
  // Specify the page size (can be much larger than letter size)
  assert (currentPageNumber < pages.size());
  currentPage = pages[currentPageNumber];

  // use default coordinates
  float fontSize = 10; // measured in pt
  // setup the font to be useed in this file
  HPDF_Page_SetFontAndSize(currentPage, theFont, fontSize);
  HPDF_Rect bbox = HPDF_Font_GetBBox(theFont);
  bbox.top *= fontSize / 1000.;
  bbox.bottom *= fontSize / 1000.;
  bbox.left *= fontSize / 1000.;
  bbox.right *= fontSize / 1000.;
  fontBBox = bbox; // Scaled for font size
  HPDF_Page_SetRGBFill(currentPage, 0, 0, 1);
  HPDF_Page_SetRGBStroke(currentPage, 0, 0, 1);

  HPDF_Page_BeginText(currentPage);
  HPDF_Page_MoveTextPos(currentPage, 2, 11 * 72);
  HPDF_Page_SetTextLeading(currentPage, (fontBBox.top - fontBBox.bottom) * 1.0);
}

void PDFGeneration::create_textlink(const string& text, HPDF_Destination target, int hitboxextender)
{
  HPDF_TextWidth tl = HPDF_Font_TextWidth(theFont, (const HPDF_BYTE*)text.c_str(), text.size());
  float fontSize = 10; // measured in pt (match the one in begin_page)
  double textlength = tl.width * (fontSize / 1000.);
  HPDF_Point textpos = HPDF_Page_GetCurrentTextPos(currentPage);
  // add local link rectangle
  HPDF_Rect linkRect;
  linkRect.left = textpos.x - hitboxextender;
  linkRect.bottom = textpos.y + fontBBox.bottom - hitboxextender;
  linkRect.right = textpos.x + textlength + hitboxextender;
  linkRect.top = textpos.y + fontBBox.top + hitboxextender;
  HPDF_Page_CreateLinkAnnot(currentPage, linkRect, target);
  // add text
  HPDF_Page_ShowText(currentPage, text.c_str());
}

void PDFGeneration::end_page()
{
  HPDF_Page_EndText(currentPage);
  ++currentPageNumber;
}

// Close pdf file representing Sage's AST
void PDFGeneration::pdf_finalize()
{
  // Now close the PDF file
  HPDF_SaveToFile(pdfFile, filename.c_str());
  HPDF_Free(pdfFile);
}

std::string PDFGeneration::text_page(SgNode* node)
{
  return "";
}

std::string PDFGeneration::get_bookmark_name(SgNode* node) {
  return typeid(node).name();
}

void PDFGeneration::edit_page(size_t pageNumber, SgNode *node, PDFInheritedAttribute inheritedValue)
{
}

void PDFGeneration::generate(std::string filename, SgNode* node) {
  std::string pdffilename=filename+".pdf";
  std::cout << "generating PDF file: " << pdffilename << " ... ";
  pdf_setup(pdffilename, 0);
  PDFInheritedAttribute pdfIA(pdfFile);
  traverse(node,pdfIA);
  pdf_finalize();
  std::cout << "done (full AST)." << std::endl;
}

PDFInheritedAttribute
PDFGeneration::evaluateInheritedAttribute(SgNode* node, PDFInheritedAttribute inheritedValue) {
  std::string stext=text_page(node);
  const char* text=stext.c_str();
  begin_page();
  HPDF_Page_ShowText(currentPage, text);
  std::string bookmarktext=get_bookmark_name(node);
  edit_page(currentPageNumber, node, inheritedValue);
  HPDF_Outline outline = HPDF_CreateOutline(pdfFile, inheritedValue.currentOutline, bookmarktext.c_str(), NULL);
  HPDF_Outline_SetDestination(outline, pageDests[currentPageNumber]);
  PDFInheritedAttribute ia(outline, pageDests[currentPageNumber]); // Do this before page number is incremented
  end_page();
  return ia;
}

