// Author: Markus Schordan, Vienna University of Technology, 2004.
// $Id: printVars.C,v 1.3 2006/04/24 00:21:26 dquinlan Exp $

#include "MyTraversal.h"

int main ( int argc, char** argv) {

   // DQ (4/6/2017): This will not fail if we skip calling ROSE_INITIALIZE (but
   // any warning message using the message looging feature in ROSE will fail).
   ROSE_INITIALIZE;

   // command line parameters are passed to EDG
   // non-EDG parameters are passed (through) to ROSE (and the vendor compiler)
   SgProject* root=frontend(argc,argv); 
   MyTraversal t;

   /*! traverse only C files specified on command line
       do not traverse included header files
       perform a preorder traversal
   */
   cout << endl << "Output:" << endl;
   t.traverseInputFiles(root,preorder);
   cout << "Number of binary operators: " << t.getBinaryOpCount() << endl;

   /*! Generate a dot file showing the AST
       The nodes are numbered by the order in which they are traversed in
       a preorder traversal (by using AstDOTGeneration::PREORDER as argument)
   */
   AstDOTGeneration astdotgen;
   astdotgen.generateInputFiles(root,AstDOTGeneration::PREORDER);

   /*! The dot file can be translated to a postscript file by using dot
       e.g.: dot -Tps testInput1.C.dot > testInput1.ps
       use gv to view testInput1.ps (select BBox to enable scrolling of the graph)
   */

   return 0;
}   


