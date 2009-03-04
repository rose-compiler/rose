/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#include "termite.h"
#include <iostream>
#include <stdio.h>
#include <satire_rose.h>
#include "PrologToRose.h"

using namespace std;

int main(int argc, char** argv) {
  //  cout << prote->getRepresentation();
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " termfile.pl sourcefile" << endl;
    return 1;
  }
  init_termite(argc, argv);

  PrologToRose conv;
  SgNode* p = conv.toRose(argv[1]);

  //Create dot and pdf files
  //DOT generation (numbering:preoder)
//   AstDOTGeneration dotgen;
//   dotgen.generateInputFiles((SgProject*)p,AstDOTGeneration::PREORDER);

  //PDF generation
//   AstPDFGeneration pdfgen;
//   pdfgen.generateInputFiles((SgProject*)p);

  conv.unparse(argv[2], p);
  return 0;
}
