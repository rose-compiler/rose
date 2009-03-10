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
       << " termfile.term\n"
       << "  Unparses a term file to its original source representation.\n\n"

       << "  Options:\n"
       << "  -o, --output sourcefile.c\n"
       << "    If specified, the contents of all files will be concatenated\n"
       << "    into the sourcefile.\n\n"

       << "  -s, --suffix '.suffix'  Default: '.unparsed'\n"

       << "    Use the original file names with the additional suffix.\n\n"

       << "  -d, --dir DIRECTORY\n"
       << "    Create the unparsed files in DIRECTORY.\n"
       << endl;
}

int main(int argc, char** argv) {
  //  cout << prote->getRepresentation();
  const char* infile = "";
  const char* outfile= "";
  const char* outdir = "";
  const char* suffix = ".unparsed";

  while (1) {
    static struct option long_options[] = {
      /* These options set a flag. */
      {"dir",    required_argument, 0, 'd'},
      {"output", required_argument, 0, 'o'},
      {"suffix", required_argument, 0, 's'},
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;
     
    int c = getopt_long (argc, argv, "d:o:s:", long_options, &option_index);
     
    /* Detect the end of the options. */
    if (c == -1)
      break;
     
    switch (c) {
    case 'd': outdir  = optarg; break;
    case 'o': outfile = optarg; break;
    case 's': suffix  = optarg; break;
     
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

  conv.unparse(outfile, outdir, suffix, p);
  return 0;
}
