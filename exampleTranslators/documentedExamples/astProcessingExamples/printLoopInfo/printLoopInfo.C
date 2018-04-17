// Author: Markus Schordan, Vienna University of Technology, 2004.
// $Id: printLoopInfo.C,v 1.2 2006/04/24 00:21:26 dquinlan Exp $

#include "rose.h"
#include "LoopNestingInfoProcessing.h"

int main ( int argc, char** argv) {

   // DQ (4/6/2017): This will not fail if we skip calling ROSE_INITIALIZE (but
   // any warning message using the message looging feature in ROSE will fail).
   ROSE_INITIALIZE;

   // command line parameters are passed to EDG
   // non-EDG parameters are passed (through) to ROSE (and the vendor compiler)
   SgProject* root=frontend(argc,argv); 
   LoopLevelProcessing t;

   // traverse only C++ files specified on command line
   // do not traverse included header files
   cout << endl << "Output:" << endl;
   t.traverseInputFiles(root,0);
   cout << "Max loop nesting level: " << t.getMaxNestingLevel() << endl;
   cout << endl;

   // Generate a dot file showing the AST
   AstDOTGeneration astdotgen;
   astdotgen.generateInputFiles(root,AstDOTGeneration::PREORDER);

   // Generate a pdf file showing the AST
   AstPDFGeneration astpdfgen;
   astpdfgen.generateInputFiles(root);
   
   return 0;
}   
