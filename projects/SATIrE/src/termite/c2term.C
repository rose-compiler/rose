/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2009 Adrian Prantl <adrian@complang.tuwien.ac.at>

 * Purpose: create a TERMITE representation of a given AST
 */


#include <iostream>
#include <fstream>
#include <satire_rose.h>

// GB (2009-02-25): We want to build c2term without ICFG stuff to avoid
// having to link against libsatire. Also, want to avoid PAG DFI stuff.
#define HAVE_SATIRE_ICFG 0
#undef HAVE_PAG
#include "TermPrinter.h"

using namespace std;

int main ( int argc, char ** argv ) {
  //frontend processing
  if (argc < 3) {
    cerr << "Usage: " << argv[0] 
	 << " [frontend options] [--strip-headers] src1.c src2... termfile.pl"
         << endl
         << "       [frontend options] will be passed to the C/C++ frontend. " 
	 << endl
         << "       Default behaviour is to include header files in the term"
	 << " representation." << endl;
    return 1;
  }

  bool strip_headers = false;
  for (int i = 0; i < argc; ++i)
    if (string(argv[i]) == "--strip-headers") {
      char empty[] = "";
      argv[i] = empty;
      strip_headers = true;
    }

  char *outputFileName = argv[argc-1];
  // GB (2008-11-04): Now that this argument is unused, we can use it
  // to turn off the frontend's warnings; they are distracting in the
  // automated test outputs.
  char warningFlag[] = "-edg:w";
  argv[argc-1] = warningFlag;

  SgProject * project = frontend(argc,argv);

  //Create dot and pdf files
  //DOT generation (numbering:preoder)
  //AstDOTGeneration dotgen;

  //dotgen.generateInputFiles(project,AstDOTGeneration::PREORDER);
  //PDF generation
  //AstPDFGeneration pdfgen;
  //pdfgen.generateInputFiles(project);

  init_termite(argc, argv);
  
  //create prolog term
  BasicTermPrinter tp;
  if (strip_headers) 
    tp.traverseInputFiles(project); // Without headers
  else
    tp.traverse(project); // With headers

  PrologTerm* genTerm = tp.getTerm();

  ofstream ofile(outputFileName);
  ofile << genTerm->getRepresentation() << "." << endl;
  ofile.close();
		
  return 0;
}
