/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
/*
 * generatePrologTerm.C
 * Purpose: create a PROLOG representation of a given AST
 * Author: Christoph Bonitz (after looking at the documentedExamples) 
 */


#include <iostream>
#include <fstream>
#include <satire_rose.h>
#include "TermPrinter.h"
#include "PrologTerm.h"

int main ( int argc, char ** argv ) {
  //frontend processing
  // GB (2008-10-20): This used to call the frontend on the entire command
  // line, including the output term file name. That's wrong, and ROSE
  // 0.9.3a started complaining. Therefore, the argument vector is now
  // filtered before we pass it to the frontend.
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " sourcefile.[cC] termfile.pl" << endl;
    return 1;
  }
  char *outputFileName = argv[2];
  argc = 2;
  argv[argc] = NULL;

  SgProject * root = frontend(argc,argv);
	

  //Create dot and pdf files
  //DOT generation (numbering:preoder)
  //AstDOTGeneration dotgen;

  //dotgen.generateInputFiles(root,AstDOTGeneration::PREORDER);
  //PDF generation
  //AstPDFGeneration pdfgen;
  //pdfgen.generateInputFiles(root);
  
  //create prolog term
  BasicTermPrinter tp;
  //tp.traverse(root);
  tp.traverseInputFiles(root);
  PrologTerm* genTerm = tp.getTerm();

  ofstream ofile(outputFileName);
  ofile << genTerm->getRepresentation() << "." << endl;
  ofile.close();
		
  return 0;
}
