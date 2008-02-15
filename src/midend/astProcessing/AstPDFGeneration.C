// Author: Markus Schordan
// $Id: AstPDFGeneration.C,v 1.5 2007/02/19 16:08:38 vuduc2 Exp $

#ifndef ASTPDFGENERATION_C
#define ASTPDFGENERATION_C

#include "sage3.h"
#include "roseInternal.h"

// #include <string>
// #include <sstream>

#include "AstPDFGeneration.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void
AstPDFGeneration::generate(string filename, SgNode* node) {
  string pdffilename=filename+".pdf";
// cout << "generating PDF file: " << pdffilename << " ... ";
  AstNodeVisitMapping addrPageMappingTrav(0);
  addrPageMappingTrav.traverse(node,preorder);
  addrPageMapping=addrPageMappingTrav.address_pagenum;
  pdf_setup(pdffilename);
  PDFInheritedAttribute pdfIA;
  traverse(node,pdfIA);
  pdf_finalize();
// cout << "done (full AST)." << endl;
}

void
AstPDFGeneration::generateInputFiles(SgProject* projectNode) {
  SgFilePtrListPtr fList = projectNode->get_fileList();
  for (SgFilePtrList::iterator fl_iter = fList->begin();
       fl_iter != fList->end(); fl_iter++) {
    ROSE_ASSERT(*fl_iter!=0);
    generateWithinFile(*fl_iter);
  }
}

void
AstPDFGeneration::generateWithinFile(const string& pdffilename, SgFile* node) {
  AstNodeVisitMapping addrPageMappingTrav(1);
  addrPageMappingTrav.traverseWithinFile(node,preorder);
  addrPageMapping=addrPageMappingTrav.address_pagenum;
// cout << "generating PDF file: " << pdffilename << " ... ";
  pdf_setup(pdffilename + ".pdf");
  PDFInheritedAttribute pdfIA;
  traverseWithinFile(node,pdfIA);
  pdf_finalize();
// cout << "done." << endl;
}

void
AstPDFGeneration::generateWithinFile(SgFile* node) {
  string pdffilename=string("./")+string(ROSE::stripPathFromFileName(ROSE::getFileName(node)));
  generateWithinFile (pdffilename, node);
}

PDFInheritedAttribute
AstPDFGeneration::evaluateInheritedAttribute(SgNode* node, PDFInheritedAttribute inheritedValue)
   {
     int bookmark;
     string nodefilename="--not initialized--";
     string bookmarktext;
        {
          ostringstream ss;
          ss << node->sage_class_name() << " ";
          SgLocatedNode* sgLocNode = dynamic_cast<SgLocatedNode*> (node);
          if(sgLocNode)
             {
               Sg_File_Info* fi = sgLocNode->get_file_info();
               if(fi)
                  {
                 // ss << "(" << fi->get_line() << "," << fi->get_col() << ") in \"" << fi->get_filename() << "\"";
                 // nodefilename=string(fi->get_filename());
                    ss << "(compilerGenerated:" << fi->isCompilerGenerated()
                       << ",transformation:" << fi->isTransformation()
                       << ",unparse:" << fi->isOutputInCodeGeneration() << ") ";
                    ss << "(" << fi->get_line() << "," << fi->get_col() << ") in \"" << fi->get_filename() << "\"";
                    nodefilename=string(fi->get_filename());
                  }
                 else
                  {
                    ss << "(BUG)";
                  }
             }
            else
             {
               ss << ""; // provide no explicit info about the lack of file_info
             }
          bookmarktext=ss.str();
        }

     string stext=text_page(node);
     const char* text=stext.c_str();
     begin_page();
     PDF_show(pdfFile, text);
     edit_page(node, inheritedValue);
     bookmark = PDF_add_bookmark(pdfFile, bookmarktext.c_str(), inheritedValue.parent, 1);
     end_page();
     PDFInheritedAttribute ia(bookmark);
     inheritedValue=ia;

     return inheritedValue;
   }

void 
AstPDFGeneration::edit_page(SgNode* node, PDFInheritedAttribute inheritedValue)
   {
     int targetpage=inheritedValue.parent;

  // JW (added by DQ 7/23/2004): Adds address of each IR node to the top of the page
     PDF_setrgbcolor(pdfFile, 1, 0, 0);
  // DQ (1/20/2006): Modified for 64 bit machines
  // ostringstream _ss; _ss << "0x" << std::hex << (int)node;
     ostringstream _ss; _ss << "pointer:" << std::hex << node;
     PDF_continue_text(pdfFile, _ss.str().c_str());

     PDF_setrgbcolor(pdfFile, 0, 1, 0);

     if (inheritedValue.parent==0)
        {
          PDF_continue_text(pdfFile, "");
          PDF_continue_text(pdfFile, "      root node");
        }
       else
        {
          PDF_continue_text(pdfFile, "");
          PDF_continue_text(pdfFile, "     ");
          create_textlink("Click here to go to the parent node", targetpage, 9);
        }
     PDF_continue_text(pdfFile, "");
     PDF_continue_text(pdfFile, "");

  // generate RTI information for SgNode
   {
     RTIReturnType rti=node->roseRTI();
     for(RTIReturnType::iterator i=rti.begin(); i<rti.end(); i++)
        {
          PDF_setrgbcolor(pdfFile, 0.5, 0, 0.1);
          PDF_continue_text(pdfFile, ((*i)->type+" ").c_str());
          PDF_setrgbcolor(pdfFile, 0.0, 0.5, 0.5);
          PDF_show(pdfFile, ((*i)->name+" : ").c_str());
          PDF_setrgbcolor(pdfFile, 0.0, 0.0, 0.0);
      
          string value=(*i)->value;
          AstNodeVisitMapping::MappingType::iterator mapit;
      
      // ensure that mapping value exists (otherwise it would be added to the map)
      // and decide whether to create a link to a page (representing a node) or not
         mapit=addrPageMapping.find(value);
         if (mapit!=addrPageMapping.end())
            {
	           create_textlink(value.c_str(), (*mapit).second /* targetpage */);
            }
           else 
            {
              PDF_show(pdfFile,value.c_str());
            }
        }
   }

  // generate AstAttribute information
   {
  // printf ("In AstPDFGeneration::edit_page(): using new attribute interface \n");
  // if (node->get_attribute() != NULL)
#if 0
     if (node->getAttribute() != NULL)
        {
          AstAttributeMechanism::AttributeIdentifiers aidents = node->attribute().getAttributeIdentifiers();
          PDF_continue_text(pdfFile, ""); // next line
          PDF_setrgbcolor(pdfFile, 0.0, 0.2, 0.7);
          PDF_continue_text(pdfFile, "AstAttributes:"); // next line
          for (AstAttributeMechanism::AttributeIdentifiers::iterator it = aidents.begin(); it != aidents.end(); it++)
             {
               PDF_continue_text(pdfFile, ""); // next line
               PDF_setrgbcolor(pdfFile, 0.0, 0.2, 0.7);
               PDF_show(pdfFile,((*it)+": ").c_str());
               PDF_setrgbcolor(pdfFile, 0.0, 0.0, 0.0);
            // float textxpos=PDF_get_value(pdfFile,"textx",0.0);
            // float textypos=PDF_get_value(pdfFile,"texty",0.0); 
               string attributeValue = (node->attribute()[*it])->toString();

            // split string into different substrings separated by newlines
               string substring="";
               int oldpos=0;
               int newpos;
               do {
                    newpos=attributeValue.find('\n', oldpos);
                    substring=attributeValue.substr(oldpos,newpos-oldpos);
                    if(oldpos==0)
                         PDF_show(pdfFile, substring.append("   ").c_str());
                      else
                         PDF_continue_text(pdfFile, substring.c_str());
                    oldpos = newpos+1; // go to next '\n' and skip it
                  }
            // DQ (8/9/2005): Suggested fix from Rich (fixes infinite loop where AST Attributes are attached)
            // while( newpos < (int) attributeValue.size());
               while( newpos < (int) attributeValue.size() && newpos >= 0 );
            // PDF_show_boxed(pdfFile, attributeValue.c_str(), textxpos, textypos, 0, 0, "left", "");
             }
        }
#else
  // DQ (4/10/2006): New AstAttribute Interface (but access the AstAttributeMechanism directly 
  // since "getAttributeIdentifiers()" is not in the interface implemented at the IR nodes).
     if (node->get_attributeMechanism() != NULL)
        {
          AstAttributeMechanism::AttributeIdentifiers aidents = node->get_attributeMechanism()->getAttributeIdentifiers();
          PDF_continue_text(pdfFile, ""); // next line
          PDF_setrgbcolor(pdfFile, 0.0, 0.2, 0.7);
          PDF_continue_text(pdfFile, "AstAttributes:"); // next line
          for (AstAttributeMechanism::AttributeIdentifiers::iterator it = aidents.begin(); it != aidents.end(); it++)
             {
               PDF_continue_text(pdfFile, ""); // next line
               PDF_setrgbcolor(pdfFile, 0.0, 0.2, 0.7);
               PDF_show(pdfFile,((*it)+": ").c_str());
               PDF_setrgbcolor(pdfFile, 0.0, 0.0, 0.0);
            // float textxpos=PDF_get_value(pdfFile,"textx",0.0);
            // float textypos=PDF_get_value(pdfFile,"texty",0.0); 
            // string attributeValue = (node->attribute()[*it])->toString();
               string attributeValue = node->getAttribute(*it)->toString();

            // split string into different substrings separated by newlines
               string substring="";
               int oldpos=0;
               int newpos;
               do {
                    newpos=attributeValue.find('\n', oldpos);
                    substring=attributeValue.substr(oldpos,newpos-oldpos);
                    if(oldpos==0)
                         PDF_show(pdfFile, substring.append("   ").c_str());
                      else
                         PDF_continue_text(pdfFile, substring.c_str());
                    oldpos = newpos+1; // go to next '\n' and skip it
                  }
            // DQ (8/9/2005): Suggested fix from Rich (fixes infinite loop where AST Attributes are attached)
            // while( newpos < (int) attributeValue.size());
               while( newpos < (int) attributeValue.size() && newpos >= 0 );
            // PDF_show_boxed(pdfFile, attributeValue.c_str(), textxpos, textypos, 0, 0, "left", "");
             }
        }
#endif
   }

   }

#endif
