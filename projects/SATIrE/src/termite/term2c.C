/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#include "termite.h"
#include <iostream>
#include <stdio.h>
#include <satire_rose.h>
#include "TermToRose.h"
#include <getopt.h>

using namespace std;

void usage(const char* me) 
{
  cerr << "Usage: " << me
       << " termfile.term [--suffix .unparsed] [--output sourcefile]\n"
       << "  Unparses a term file to its original source representation.\n"
       << "  If [sourcefile] is specified, the contents of all files will\n"
       << "  be concatenated into the same file, otherwise the original\n"
       << "  file names with the suffix will be used." 
       << endl;
}

int main(int argc, char** argv) {
  //  cout << prote->getRepresentation();
  const char *infile, *outfile, *suffix = ".unparsed";

  while (1) {
    static struct option long_options[] = {
      /* These options set a flag. */
      {"output",    required_argument, 0, 'o'},
      {"suffix",    required_argument, 0, 's'},
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;
     
    int c = getopt_long (argc, argv, "o:s:", long_options, &option_index);
     
    /* Detect the end of the options. */
    if (c == -1)
      break;
     
    switch (c) {
    case 'o':
      outfile = optarg;
      break;

    case 's':
      suffix = optarg;
      break;
     
    default:
      usage(argv[0]);
      return 1; 
    }
  }
  if (optind < argc) {
    infile = argv[optind];
  } else {
    usage(argv[0]);
    return 1;
  }

  init_termite(argc, argv);

  PrologToRose conv;
  SgNode* p = conv.toRose(infile);

//  Create dot and pdf files
//    DOT generation (numbering:preoder)
//    AstDOTGeneration dotgen;
//    dotgen.generateInputFiles((SgProject*)p,AstDOTGeneration::PREORDER);

//  PDF generation
//    AstPDFGeneration pdfgen;
//    pdfgen.generateInputFiles((SgProject*)p);

  conv.unparse(outfile, suffix, p);
  return 0;
}
