/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2008 Adrian Prantl <adrian@complang.tuwien.ac.at>

 * Purpose: create a PROLOG representation of a given AST
 */


#include <iostream>
#include <fstream>
#include <satire_rose.h>
#include "TermPrinter.h"

using namespace std;

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
  // GB (2008-11-04): Now that argv[2] is unused, we can use it to turn off
  // the frontend's warnings; they are distracting in the automated test
  // outputs.
  char warningFlag[] = "-edg:w";
  argv[2] = warningFlag;

  SgProject * root = frontend(argc,argv);
	

  //Create dot and pdf files
  //DOT generation (numbering:preoder)
  //AstDOTGeneration dotgen;

  //dotgen.generateInputFiles(root,AstDOTGeneration::PREORDER);
  //PDF generation
  //AstPDFGeneration pdfgen;
  //pdfgen.generateInputFiles(root);

  init_termite(argc, argv);
  
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
